#include "stdafx.h"
#include "..\Public\NonAnim.h"
#include "Imgui_Manager.h"
#include "PickingMgr.h"
#include "GameInstance.h"

CNonAnim::CNonAnim(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBaseObj(pDevice, pContext)
{
}

CNonAnim::CNonAnim(const CNonAnim & rhs)
	: CBaseObj(rhs)
{
}

HRESULT CNonAnim::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNonAnim::Initialize(void * pArg)
{
	if(pArg != nullptr)
		memcpy(&m_ModelDesc, pArg, sizeof(NONANIMDESC));

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	//CPickingMgr::Get_Instance()->Add_PickingGroup(this);

	m_eObjectID = OBJ_BLOCK;

	if (pArg != nullptr)
	{
		_vector vPosition = XMLoadFloat3(&m_ModelDesc.vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		Set_Scale(m_ModelDesc.vScale);

		if(m_ModelDesc.m_fAngle != 0)
			m_pTransformCom->Turn(XMLoadFloat3(&m_ModelDesc.vRotation), m_ModelDesc.m_fAngle);
	}
	

	CModelManager::Get_Instance()->Add_CreatedModel(this);

	return S_OK;
}

int CNonAnim::Tick(_float fTimeDelta)
{
	if (__super::Tick(fTimeDelta))
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CNonAnim::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	Compute_CamDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

HRESULT CNonAnim::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderID()))
		return E_FAIL;


	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshContainers();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_eShaderID)))
			return E_FAIL;
	}

	return S_OK;
}

_bool CNonAnim::Picking(_float3 * PickingPoint)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_pModelCom->Picking(m_pTransformCom, &m_vMousePickPos) == true)
	{
		CTerrain_Manager::Get_Instance()->Set_PickingWorldPos(m_vMousePickPos);
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}
	else
	{
		ZeroMemory(&m_vMousePickPos, sizeof(_float3));
	}


	RELEASE_INSTANCE(CGameInstance);

	return false;
}

void CNonAnim::PickingTrue()
{
	CImgui_Manager::PICKING_TYPE ePickingtype = CImgui_Manager::Get_Instance()->Get_PickingType();

	switch (ePickingtype)
	{
	case Client::CImgui_Manager::PICKING_OBJECT:
		Set_Picked();
		break;
	case Client::CImgui_Manager::PICKING_TERRAIN:
		break;
	default:
		break;
	}
}

void CNonAnim::Set_Picked()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Mouse_Up(DIMK::DIMK_LBUTTON))
	{
		if (m_bPicked)
		{
			CModelManager::Get_Instance()->Set_AllPickedFalse();
			CPickingMgr::Get_Instance()->Set_PickedObj(nullptr);
		}	
		else
		{
			CModelManager::Get_Instance()->Set_AllPickedFalse();
			CPickingMgr::Get_Instance()->Set_PickedObj(this);
		}
			
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CNonAnim::Ready_Components(void* pArg)
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	_tchar			szModeltag[MAX_PATH] = TEXT("");
	MultiByteToWideChar(CP_ACP, 0, m_ModelDesc.pModeltag, strlen(m_ModelDesc.pModeltag), szModeltag, MAX_PATH);
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, szModeltag, (CComponent**)&m_pModelCom)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CNonAnim::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CNonAnim::SetUp_ShaderID()
{
	_bool m_bWireFrame = CTerrain_Manager::Get_Instance()->Get_TerrainDesc().m_bShowWireFrame;

	if (m_bWireFrame)
		m_eShaderID = SHADER_WIREFRAME;
	else if (m_bPicked)
		m_eShaderID = SHADER_PICKED;
	else
		m_eShaderID = SHADER_DEFAULT;

	return S_OK;
}

CNonAnim * CNonAnim::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CNonAnim*	pInstance = new CNonAnim(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CNonAnim"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CNonAnim::Clone(void * pArg)
{
	CNonAnim*	pInstance = new CNonAnim(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CNonAnim"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNonAnim::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}

#include "stdafx.h"
#include "..\Public\Terrain.h"
#include "GameInstance.h"
#include "PipeLine.h"
#include "Terrain_Manager.h"
#include "PickingMgr.h"

CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if(!m_bWireFrame)
		CPickingMgr::Get_Instance()->Add_PickingGroup(this);

	return S_OK;
}

int CTerrain::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);		

	if (m_bDebugTerrain)
	{
		CTerrain_Manager::TERRAINDESC TerrainDesc = CTerrain_Manager::Get_Instance()->Get_TerrainDesc();
		_vector vPosition = XMVectorSet(TerrainDesc.m_iPositionX, TerrainDesc.m_fHeight, TerrainDesc.m_iPositionZ, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

		m_bDebugShow = CTerrain_Manager::Get_Instance()->Get_TerrainShow();
	}

	return S_OK;
}

void CTerrain::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	Compute_CamDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

HRESULT CTerrain::Render()
{
	if (m_bDebugTerrain && !m_bDebugShow)
		return S_OK;

	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;


	if (FAILED(SetUp_ShaderID()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(m_eShaderID);

	m_pVIBufferCom->Render();

	return S_OK;
}

_bool CTerrain::Picking(_float3 * PickingPoint)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_pVIBufferCom->Picking(m_pTransformCom, &m_vMousePickPos) == true)
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

void CTerrain::PickingTrue()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Mouse_Up(DIMK::DIMK_LBUTTON))
	{
		if (m_bPicked)
			CPickingMgr::Get_Instance()->Set_PickedObj(nullptr);
		else
			CPickingMgr::Get_Instance()->Set_PickedObj(this);
		/*_float fHegith = CMapManager::Get_Instance()->Get_Height();
		_float fRad = CMapManager::Get_Instance()->Get_Rad();
		_float fSharp = CMapManager::Get_Instance()->Get_Sharp();*/

		_float fHegith = 1.f;
		_float fRad = 1.f;
		_float fSharp = 1.f;

		//m_pVIBufferCom->Set_Terrain_Shape(fHegith, fRad, fSharp, m_vMousePickPos, 1.f);
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CTerrain::Ready_Components(void* pArg)
{
	CVIBuffer_Terrain::TERRAINDESC TerrainDesc;
	memcpy(&TerrainDesc, pArg, sizeof(CVIBuffer_Terrain::TERRAINDESC));
	m_bDebugTerrain = TerrainDesc.m_bTestShowTerrain;
	m_bWireFrame = TerrainDesc.m_bShowWireFrame;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"), (CComponent**)&m_pTextureCom[TYPE_DIFFUSE])))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"), (CComponent**)&m_pVIBufferCom, pArg)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.fRotationPerSec = 1.f;
	TransformDesc.fSpeedPerSec = 3.f;

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::SetUp_ShaderID()
{

	if (m_bDebugTerrain)
	{
		if (CTerrain_Manager::Get_Instance()->Get_TerrainDesc().m_bShowWireFrame)
			m_eShaderID = SHADER_WIREFRAME;
		else
			m_eShaderID = SHADER_DEFAULT;
	}

	if(m_bWireFrame)
		m_eShaderID = SHADER_WIREFRAME;
	else if (m_bPicked)
		m_eShaderID = SHADER_PICKED;
	else
		m_eShaderID = SHADER_DEFAULT;

	return S_OK;
}

HRESULT CTerrain::SetUp_ShaderResources()
{

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom[TYPE_DIFFUSE]->Get_SRV(0))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CTerrain * CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTerrain*	pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTerrain::Clone(void* pArg)
{
	CTerrain*	pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

	for (_uint i = 0; i < TYPE_END; ++i)
		Safe_Release(m_pTextureCom[i]);
}

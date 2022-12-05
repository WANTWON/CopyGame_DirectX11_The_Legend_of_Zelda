#include "stdafx.h"
#include "..\Public\FieldDecoObject.h"
#include "Player.h"
#include "MonsterBullet.h"
#include "Navigation.h"

CFieldDecoObject::CFieldDecoObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBaseObj(pDevice, pContext)
{
}

HRESULT CFieldDecoObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFieldDecoObject::Initialize(void * pArg)
{
	if (pArg != nullptr)
		memcpy(&m_DecoDesc, pArg, sizeof(DECODESC));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	_vector vecPostion = XMLoadFloat3(&m_DecoDesc.vInitPos);
	vecPostion = XMVectorSetW(vecPostion, 1.f);
	

	switch (m_DecoDesc.eDecoType)
	{
	case BUTTERFLY:
		vecPostion = XMVectorSetY(vecPostion, XMVectorGetY(vecPostion)+ 2.f);
		m_eState = IDLE;
		break;
	default:
		break;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vecPostion);
	m_pNavigationCom->Compute_CurrentIndex_byDistance(vecPostion);
	Set_Scale(_float3(1.2f, 1.2f, 1.2f));
	m_fHeight = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//CData_Manager* pData_Manager = GET_INSTANCE(CData_Manager);
	//char cName[MAX_PATH];
	//ZeroMemory(cName, sizeof(char) * MAX_PATH);
	//pData_Manager->TCtoC(TEXT("Octorock"), cName);
	//pData_Manager->Conv_Bin_Model(m_pModelCom, cName, CData_Manager::DATA_ANIM);
	////ERR_MSG(TEXT("Save_Bin_Model"));
	//RELEASE_INSTANCE(CData_Manager);

	//RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

int CFieldDecoObject::Tick(_float fTimeDelta)
{
	_float3 vScale = Get_Scale();
	_float fCullingRadius = max(max(vScale.x, vScale.y), vScale.z);
	if (CGameInstance::Get_Instance()->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), fCullingRadius + 2) == false)
		return OBJ_NOEVENT;

	if (__super::Tick(fTimeDelta))
		return OBJ_DEAD;

	m_pModelCom->Set_NextAnimIndex(m_eState);
	Change_Animation(fTimeDelta);

	if (m_dwTime + rand()%5000 < GetTickCount())
	{
		m_fAngle = (rand() % 5000)*0.001f;
		m_dwTime = GetTickCount();
	}


	m_fAngle -= 0.1f;
	if (m_fAngle <= 0.f)
	{
		m_fAngle = 0.f;
		m_fHeight += rand()%2 == 0 ? 0.002f : - 0.002f ;
	}
	
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPosition = XMVectorSetY(vPosition, m_fHeight);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_pTransformCom->Turn(XMVectorSet(0.f,1.f,0.f,0.f), m_fAngle);

	return OBJ_NOEVENT;
}

void CFieldDecoObject::Late_Tick(_float fTimeDelta)
{
	
	if (Check_IsinFrustum() == false)
	{
		return;
	}

	__super::Late_Tick(fTimeDelta);


	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	SetUp_ShaderID();

	Check_Navigation(fTimeDelta);

}

HRESULT CFieldDecoObject::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pModelCom)
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

void CFieldDecoObject::Change_Animation(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CFieldDecoObject::IDLE:
	case Client::CFieldDecoObject::WALK:
		m_fAnimSpeed = 2.f;
		m_bIsLoop = true;
		m_pTransformCom->Go_StraightSliding(fTimeDelta, m_pNavigationCom);
		m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop);
		break;
	}
}

HRESULT CFieldDecoObject::Ready_Components(void * pArg)
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 3.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(1.0f);
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	switch (m_DecoDesc.eDecoType)
	{
	case BUTTERFLY:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Butterfly"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	default:
		break;
	}

	

	/* For.Com_SPHERE*/
	CCollider::COLLIDERDESC		ColliderDesc;
	ColliderDesc.vScale = _float3(1.0f, 1.0f, 1.0f);
	ColliderDesc.vRotation = _float3(0.f, XMConvertToRadians(0.0f), 0.f);
	ColliderDesc.vPosition = _float3(0.f, 0.5f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_OBB"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof NaviDesc);
	NaviDesc.iCurrentCellIndex = 0;
	if (FAILED(__super::Add_Components(TEXT("Com_Navigation_Field"), LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Field"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;



	return S_OK;
}

HRESULT CFieldDecoObject::SetUp_ShaderResources()
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



void CFieldDecoObject::Check_Navigation(_float fTimeDelta)
{
	if (m_pNavigationCom->Get_CurrentCelltype() == CCell::ACCESSIBLE)
	{
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float m_fHeight = m_pNavigationCom->Compute_Height(vPosition, 0.f);
		if (m_fHeight > XMVectorGetY(vPosition))
		{
			vPosition = XMVectorSetY(vPosition, m_fHeight);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

		}
	}
}



CFieldDecoObject * CFieldDecoObject::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFieldDecoObject*	pInstance = new CFieldDecoObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CFieldDecoObject"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFieldDecoObject::Clone(void * pArg)
{
	CFieldDecoObject*	pInstance = new CFieldDecoObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CFieldDecoObject"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFieldDecoObject::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pNavigationCom);
}


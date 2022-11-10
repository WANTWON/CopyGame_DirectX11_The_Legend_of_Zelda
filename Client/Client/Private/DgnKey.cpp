#include "stdafx.h"
#include "..\Public\DgnKey.h"
#include "GameInstance.h"
#include "Player.h"
#include "UI_Manager.h"

CDgnKey::CDgnKey(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBaseObj(pDevice, pContext)
{
}

CDgnKey::CDgnKey(const CDgnKey & rhs)
	: CBaseObj(rhs)
{
}

HRESULT CDgnKey::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDgnKey::Initialize(void * pArg)
{
	if (pArg != nullptr)
		memcpy(&m_eKeyDesc, pArg, sizeof(DGNKEYDESC));

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	//CPickingMgr::Get_Instance()->Add_PickingGroup(this);

	m_eObjectID = OBJ_KEY;

	if (pArg != nullptr)
	{
		_vector vPosition = XMLoadFloat3(&m_eKeyDesc.vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	}

	Set_Scale(_float3(3.f, 3.f, 3.f));
	CCollision_Manager::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_ITEM, this);

	return S_OK;
}

int CDgnKey::Tick(_float fTimeDelta)
{
	if (m_bDead)
	{
		CCollision_Manager::Get_Instance()->Out_CollisionGroup(CCollision_Manager::COLLISION_ITEM, this);
		return OBJ_DEAD;
	}
		

	if (m_bGet)
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(CGameInstance::Get_Instance()->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));
		_vector pPlayerPostion = pPlayer->Get_TransformState(CTransform::STATE_POSITION);
		pPlayerPostion = XMVectorSetY(pPlayerPostion, XMVectorGetY(pPlayerPostion) + 3.f );
		m_pTransformCom->Go_PosTarget(fTimeDelta, pPlayerPostion);

		if (pPlayer->Get_AnimState() == CPlayer::ITEM_GET_ED)
		{
			m_bDead = true;
			CUI_Manager::Get_Instance()->Close_Message();
			CUI_Manager::Get_Instance()->Get_Key();
		}
			
	}


	return OBJ_NOEVENT;
}

void CDgnKey::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	Compute_CamDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CBaseObj* pTarget = dynamic_cast<CBaseObj*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));
	if (m_pOBBCom != nullptr && m_pOBBCom->Collision(pTarget->Get_Collider()))
	{
		m_pTransformCom->Go_PosDir(fTimeDelta, XMVectorSet(0.f, -1.f, 0.f, 0.f));

		_vector vPostion = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (XMVectorGetY(vPostion) < 0.5f)
		{
			vPostion = XMVectorSetY(vPostion, 0.5f);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPostion);
			Safe_Release(m_pOBBCom);
			m_pOBBCom = nullptr;
		}
			
	}

	if (m_pSPHERECom->Collision(pTarget->Get_Collider()))
	{
		m_bGet = true;
		CUI_Manager::Get_Instance()->Open_Message(CUI_Manager::DGN_KEY);
		dynamic_cast<CPlayer*>(pTarget)->Set_AnimState(CPlayer::ITEM_GET_ST);
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CDgnKey::Render()
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

#ifdef _DEBUG
	//m_pAABBCom->Render();
	if(m_pOBBCom != nullptr)
			m_pOBBCom->Render();
	m_pSPHERECom->Render();
#endif

	return S_OK;

}

HRESULT CDgnKey::Ready_Components(void * pArg)
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 4.0f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(1.0f);
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_SmallKey"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;

	///* For.Com_AABB */
	//ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	//ColliderDesc.vScale = _float3(0.7f, 0.7f, 0.7f);
	//ColliderDesc.vPosition = _float3(0.f, 0.7f, 0.f);
	//if (FAILED(__super::Add_Components(TEXT("Com_AABB"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), (CComponent**)&m_pAABBCom, &ColliderDesc)))
	//	return E_FAIL;

	/* For.Com_OBB*/
	ColliderDesc.vScale = _float3(0.5f, 20.f, 0.5f);
	ColliderDesc.vRotation = _float3(0.f, XMConvertToRadians(0.0f), 0.f);
	ColliderDesc.vPosition = _float3(0.f, -0.3f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_OBB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_SPHERE */
	ColliderDesc.vScale = _float3(0.1f, 0.1f, 0.1f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDgnKey::SetUp_ShaderResources()
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

HRESULT CDgnKey::SetUp_ShaderID()
{
	return S_OK;
}

CDgnKey * CDgnKey::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDgnKey*	pInstance = new CDgnKey(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CDgnKey"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDgnKey::Clone(void * pArg)
{
	CDgnKey*	pInstance = new CDgnKey(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CDgnKey"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDgnKey::Free()
{
	__super::Free();

	Safe_Release(m_pAABBCom);
	Safe_Release(m_pOBBCom);
	Safe_Release(m_pSPHERECom);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}

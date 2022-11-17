#include "stdafx.h"
#include "..\Public\PrizeItem.h"
#include "GameInstance.h"
#include "Player.h"
#include "UI_Manager.h"

CPrizeItem::CPrizeItem(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CNonAnim(pDevice, pContext)
{
}

CPrizeItem::CPrizeItem(const CPrizeItem & rhs)
	: CNonAnim(rhs)
{
}


HRESULT CPrizeItem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPrizeItem::Initialize(void * pArg)
{
	if (pArg != nullptr)
		memcpy(&m_ItemDesc, pArg, sizeof(ITEMDESC));

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	//CPickingMgr::Get_Instance()->Add_PickingGroup(this);

	m_eObjectID = OBJ_KEY;

	if (pArg != nullptr)
	{
		_vector vPosition = XMLoadFloat3(&m_ItemDesc.vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	}

	Set_Scale(_float3(3.f, 3.f, 3.f));
	CCollision_Manager::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_ITEM, this);

	return S_OK;
}

int CPrizeItem::Tick(_float fTimeDelta)
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
		}
			
	}


	return OBJ_NOEVENT;
}

void CPrizeItem::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	Compute_CamDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CBaseObj* pTarget = dynamic_cast<CBaseObj*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));
	
	if (m_ItemDesc.m_bPrize && m_pSPHERECom->Collision(pTarget->Get_Collider()))
	{
		m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
		m_bGet = true;
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CPrizeItem::Render()
{
	__super::Render();


	return S_OK;

}

HRESULT CPrizeItem::Ready_Components(void * pArg)
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

	switch (m_ItemDesc.eType)
	{
	case SMALL_KEY:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_SmallKey"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case BOSS_KEY:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_BossKey"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case COMPASS:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_Compass"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case MAP:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_DgnMap"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case HEART:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_HeartRecovery"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case RUBY:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_Ruby"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case FEATHER:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_Feather"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case CELLO:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_FullMoonCello"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	default:
		break;
	}

	CCollider::COLLIDERDESC		ColliderDesc;

	/* For.Com_SPHERE */
	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}



CPrizeItem * CPrizeItem::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPrizeItem*	pInstance = new CPrizeItem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPrizeItem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPrizeItem::Clone(void * pArg)
{
	CPrizeItem*	pInstance = new CPrizeItem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPrizeItem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPrizeItem::Free()
{
	__super::Free();

}

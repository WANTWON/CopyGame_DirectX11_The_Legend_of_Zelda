#include "stdafx.h"
#include "..\Public\PrizeItem.h"
#include "GameInstance.h"
#include "Player.h"
#include "UI_Manager.h"
#include "InvenItem.h"
#include "MessageBox.h"
#include "UIButton.h"


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

		if (m_ItemDesc.eInteractType == CARRYABLE)
			m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
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
	

	return OBJ_NOEVENT;
}

void CPrizeItem::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

#ifdef _DEBUG
		if (m_pAABBCom != nullptr)
			m_pRendererCom->Add_Debug(m_pAABBCom);
		if (m_pOBBCom != nullptr)
			m_pRendererCom->Add_Debug(m_pOBBCom);
		if (m_pSPHERECom != nullptr)
			m_pRendererCom->Add_Debug(m_pSPHERECom);
#endif
	}
		

	Compute_CamDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (m_ItemDesc.eInteractType == PRIZE)
		LateTick_PrizeModeItem(fTimeDelta);
	else if (m_ItemDesc.eInteractType == DEFAULT)
		LateTick_DefaultModeItem(fTimeDelta);
	else if (m_ItemDesc.eInteractType == CARRYABLE)
		LateTick_CarryableModeItem(fTimeDelta);
	
}

HRESULT CPrizeItem::Render()
{
	__super::Render();


	return S_OK;

}

void CPrizeItem::LateTick_PrizeModeItem(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CBaseObj* pTarget = dynamic_cast<CBaseObj*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));

	if (m_ItemDesc.eInteractType == PRIZE && m_pSPHERECom->Collision(pTarget->Get_Collider()))
	{
		m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));

		CCollision_Manager::Get_Instance()->Out_CollisionGroup(CCollision_Manager::COLLISION_ITEM, this);

		if (m_ItemDesc.eInteractType == PRIZE && !m_bGet)
		{
			dynamic_cast<CPlayer*>(pTarget)->Set_AnimState(CPlayer::ITEM_GET_ST);
			CMessageBox::MSGDESC MessageDesc;
			MessageDesc.m_eMsgType = CMessageBox::GET_ITEM;
			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MessageBox"), LEVEL_STATIC, TEXT("Layer_UI"), &MessageDesc);

			CUI_Manager::MSGDESC MsgDesc;
			MsgDesc.eMsgType = CUI_Manager::PASSABLE;
			MsgDesc.iTextureNum = m_ItemDesc.eType;

			CUI_Manager::Get_Instance()->Add_MessageDesc(MsgDesc);
			CUI_Manager::Get_Instance()->Open_Message(true);
		}

		m_bGet = true;
	}


	if (m_bGet)
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(CGameInstance::Get_Instance()->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));
		_vector pPlayerPostion = pPlayer->Get_TransformState(CTransform::STATE_POSITION);

		pPlayerPostion = XMVectorSetY(pPlayerPostion, XMVectorGetY(pPlayerPostion) + 3.f);
		m_pTransformCom->Go_PosTarget(fTimeDelta, pPlayerPostion);

		if (pPlayer->Get_AnimState() == CPlayer::ITEM_GET_ED)
		{
			CUI_Manager::Get_Instance()->Open_Message(false);
			if (m_ItemDesc.eType == SMALL_KEY)
				CUI_Manager::Get_Instance()->Get_Key();

			if (m_ItemDesc.eType == COMPASS)
			{
				CInvenItem::ITEMDESC ItemDesc;
				ItemDesc.eItemType = CInvenItem::ITEM_PRIZE;
				ItemDesc.m_iTextureNum = CInvenItem::COMPASS;
				ItemDesc.vPosition = _float2(1400, 200);
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CInvenItem"), LEVEL_STATIC, TEXT("Layer_Compass"), &ItemDesc)))
					int a = 0;
			}
			m_bDead = true;
		}

	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPrizeItem::LateTick_DefaultModeItem(_float fTimeDelta)
{
	if (CCollision_Manager::Get_Instance()->CollisionwithGroup(CCollision_Manager::COLLISION_PLAYER, m_pSPHERECom))
		m_bGet = true;


	if (m_bGet)
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(CGameInstance::Get_Instance()->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));
		if (m_ItemDesc.eType == RUBY)
			pPlayer->Set_RubyAdd();
		else if (m_ItemDesc.eType == HEART_RECOVERY)
			pPlayer->Set_RecoverHp();
		m_bDead = true;
	}
}

void CPrizeItem::LateTick_CarryableModeItem(_float fTimeDelta)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(CGameInstance::Get_Instance()->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));
	CUIButton*		pButton = dynamic_cast<CUIButton*>(CUI_Manager::Get_Instance()->Get_Button());


	if (CCollision_Manager::Get_Instance()->CollisionwithGroup(CCollision_Manager::COLLISION_PLAYER, m_pSPHERECom))
	{
		if (!m_bGet)
		{
			pButton->Set_TexType(CUIButton::CARRY);
			pButton->Set_Visible(true);
			_float2 fPosition = pPlayer->Get_ProjPosition();
			fPosition.y = g_iWinSizeY - fPosition.y;
			fPosition.x += 50.f;
			fPosition.y -= 30.f;
			pButton->Set_Position(fPosition);
		}
		
		if (CGameInstance::Get_Instance()->Key_Up(DIK_A))
		{
			if (!m_bGet)
			{
				m_bGet = true;
				pPlayer->Set_AnimState(CPlayer::ITEM_CARRY);
				pPlayer->Ready_Parts(m_ItemDesc.eType, CPlayer::PARTS_ITEM);
				m_bDead = true;
				pButton->Set_Visible(false);
			}
		}	
	}		
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
	case HEART_RECOVERY:
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
	case ARROW:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_Arrow"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case DOGFOOD:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_DogFood"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case HEART_CONTAINER:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_HeartContainer"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case MAGIC_ROD:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_MagicRod"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case BOW:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_ItemBow"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	default:
		break;
	}
	

	CCollider::COLLIDERDESC		ColliderDesc;

	/* For.Com_SPHERE */
	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	if(m_ItemDesc.eType == CELLO)
		ColliderDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
	if(m_ItemDesc.eInteractType == DEFAULT)
		ColliderDesc.vScale = _float3(0.3f, 0.3f, 0.3f);
	if (m_ItemDesc.eInteractType == CARRYABLE)
		ColliderDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
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

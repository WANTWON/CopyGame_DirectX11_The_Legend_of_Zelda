#include "stdafx.h"
#include "..\Public\ShopNpc.h"
#include "Player.h"
#include "Level_Manager.h"
#include "CameraManager.h"
#include "PrizeItem.h"
#include "UIButton.h"
#include "MessageBox.h"
#include "Weapon.h"
#include "UIButton.h"
#include "InvenTile.h"
#include "InvenItem.h"

CShopNpc::CShopNpc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CNpc(pDevice, pContext)
{
}

CShopNpc::CShopNpc(const CShopNpc & rhs)
	: CNpc(rhs)
{
}

HRESULT CShopNpc::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShopNpc::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
		memcpy(&m_NpcDesc, pArg, sizeof(NPCDESC));

	_vector vPosition = XMVectorSetW(XMLoadFloat3(&m_NpcDesc.vInitPos), 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	return S_OK;
}

int CShopNpc::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Find_Target();
	if (m_pTarget != nullptr)
		m_pTransformCom->LookAt(m_pTarget->Get_TransformState(CTransform::STATE_POSITION));


	m_pModelCom->Set_NextAnimIndex(m_eState);
	Change_Animation(fTimeDelta);


	return OBJ_NOEVENT;
}

void CShopNpc::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (!Check_IsinFrustum())
		return;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pTarget = pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pTarget);


	CUIButton*		pButton = dynamic_cast<CUIButton*>(CUI_Manager::Get_Instance()->Get_Button());

	if (CCollision_Manager::Get_Instance()->CollisionwithGroup(CCollision_Manager::COLLISION_PLAYER, m_pSPHERECom))
	{
		if (m_eState == IDLE)
		{
			pButton->Set_TexType(CUIButton::TALK);
			pButton->Set_Visible(true);
		}

		_float2 fPosition = pPlayer->Get_ProjPosition();
		fPosition.y = g_iWinSizeY - fPosition.y;
		fPosition.x += 50.f;
		fPosition.y -= 30.f;
		pButton->Set_Position(fPosition);

		if (CGameInstance::Get_Instance()->Key_Up(DIK_A) && m_eState != TALK)
		{
			CUI_Manager::Get_Instance()->Add_TalkingNpc(this);
			CCamera* pCamera = CCameraManager::Get_Instance()->Get_CurrentCamera();
			dynamic_cast<CCamera_Dynamic*>(pCamera)->Set_CamMode(CCamera_Dynamic::CAM_TALK);
			pButton->Set_Visible(false);
			pPlayer->Set_Stop(true);
			m_eState = TALK;
			Change_Message();
		}

	}
	else
	{
		pButton->Set_Visible(false);
		m_eState = IDLE;
	}

	RELEASE_INSTANCE(CGameInstance);

}

HRESULT CShopNpc::Render()
{
	__super::Render();

	return S_OK;
}


void CShopNpc::Check_Navigation(_float fTimeDelta)
{
	if (m_pNavigationCom == nullptr)
		return;
}

void CShopNpc::Send_Answer_toNPC(_uint iTextureNum)
{
	CUI_Manager* pUI_Manager = GET_INSTANCE(CUI_Manager);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pTarget = pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pTarget);
	CWeapon::TYPE eItemType = (CWeapon::TYPE)pPlayer->Get_PartsItemType();

	pUI_Manager->Clear_ChoiceButton();

	if (iTextureNum == CUIButton::BUY)
	{
		CUI_Manager::MSGDESC eMsgDesc;
		eMsgDesc.eMsgType = CUI_Manager::PASSABLE;
		if (pPlayer->Set_RubyUse(m_iCoin))
		{

			switch (eItemType)
			{
			case Client::CWeapon::BOW:
			{
				list<CGameObject*>* pItemList = CGameInstance::Get_Instance()->Get_ObjectList(LEVEL_STATIC, TEXT("Layer_Ineventile"));
				for (auto& iter : *pItemList)
				{
					if (dynamic_cast<CInvenTile*>(iter)->Get_TextureNum() == CInvenItem::ITEM_NONE)
					{
						dynamic_cast<CInvenTile*>(iter)->Set_TextureNum(CInvenItem::ITEM_BOW);
						break;
					}
					else
						continue;
				}
				break;
			}
			case Client::CWeapon::MAGIC_ROD:
			{
				list<CGameObject*>* pItemList = CGameInstance::Get_Instance()->Get_ObjectList(LEVEL_STATIC, TEXT("Layer_Ineventile"));
				for (auto& iter : *pItemList)
				{
					if (dynamic_cast<CInvenTile*>(iter)->Get_TextureNum() == CInvenItem::ITEM_NONE)
					{
						dynamic_cast<CInvenTile*>(iter)->Set_TextureNum(CInvenItem::ITEM_WAND);
						break;
					}
					else
						continue;
				}
				break;
			}
			case Client::CWeapon::DOGFOOD:
			{
				list<CGameObject*>* pQuestItemList = CGameInstance::Get_Instance()->Get_ObjectList(LEVEL_STATIC, TEXT("Layer_QuestItem"));
				for (auto& iter : *pQuestItemList)
				{
					if (dynamic_cast<CInvenItem*>(iter)->Get_TextureNum() == CInvenItem::ITEM_NONE)
					{
						dynamic_cast<CInvenItem*>(iter)->Set_TextureNum(CInvenItem::DOG_FOOD);
						break;

					}
					else
						continue;
				}
				break;
			}
			}

			eMsgDesc.iTextureNum = THANKYOU;
		}	
		else
			eMsgDesc.iTextureNum = SORRY;
		pUI_Manager->Add_MessageDesc(eMsgDesc);
	}
	else if (iTextureNum == CUIButton::NOBUY)
	{
		CPrizeItem::ITEMDESC ItemDesc;

		switch (eItemType)
		{
		case Client::CWeapon::NONE:
			break;
		case Client::CWeapon::BOW:
			ItemDesc.eType = CPrizeItem::BOW;
			ItemDesc.eInteractType = CPrizeItem::CARRYABLE;
			ItemDesc.vPosition = _float3(4.14f, 1.5f, 1.5f);
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PrizeItem"), LEVEL_ROOM, TEXT("Layer_Object"), &ItemDesc)))
				return;
			break;
		case Client::CWeapon::ARROW:
			ItemDesc.eType = CPrizeItem::ARROW;
			ItemDesc.eInteractType = CPrizeItem::CARRYABLE;
			ItemDesc.vPosition = _float3(1.79f, 1.5f, 1.5f);
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PrizeItem"), LEVEL_ROOM, TEXT("Layer_Object"), &ItemDesc)))
				return;
			break;
		case Client::CWeapon::DOGFOOD:
			ItemDesc.eType = CPrizeItem::DOGFOOD;
			ItemDesc.eInteractType = CPrizeItem::CARRYABLE;
			ItemDesc.vPosition = _float3(-0.24f, 1.5f, 1.5f);
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PrizeItem"), LEVEL_ROOM, TEXT("Layer_Object"), &ItemDesc)))
				return;
			break;
		case Client::CWeapon::HEART_CONTAINER:
			ItemDesc.eType = CPrizeItem::HEART_CONTAINER;
			ItemDesc.eInteractType = CPrizeItem::CARRYABLE;
			ItemDesc.vPosition = _float3(-2.39f, 1.5f, 1.5f);
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PrizeItem"), LEVEL_ROOM, TEXT("Layer_Object"), &ItemDesc)))
				return;
			break;
		case Client::CWeapon::MAGIC_ROD:
			ItemDesc.eType = CPrizeItem::MAGIC_ROD;
			ItemDesc.eInteractType = CPrizeItem::CARRYABLE;
			ItemDesc.vPosition = _float3(-4.22f, 1.5f, 1.5f);
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PrizeItem"), LEVEL_ROOM, TEXT("Layer_Object"), &ItemDesc)))
				return;
			break;
		default:
			break;
		}
	}

	RELEASE_INSTANCE(CUI_Manager);
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CShopNpc::Ready_Components(void * pArg)
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

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_ROOM, TEXT("Prototype_Component_Model_ShopNpc"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;

	/* For.Com_SPHERE */
	ColliderDesc.vScale = _float3(5.f, 5.f, 5.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CShopNpc::SetUp_ShaderResources()
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

HRESULT CShopNpc::SetUp_ShaderID()
{
	return S_OK;
}

void CShopNpc::Change_Animation(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CShopNpc::TALK:
	case Client::CShopNpc::IDLE:
	case Client::CShopNpc::LASER:
		m_fAnimSpeed = 2.f;
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop);
		break;
	case Client::CShopNpc::LASER_ST:
		m_fAnimSpeed = 2.f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop))
			m_eState = LASER;
		break;
	default:
		break;
	}
}

void CShopNpc::Change_Message()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CUI_Manager* pUI_Manager = GET_INSTANCE(CUI_Manager);
	CGameObject* pTarget = pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pTarget);
	pUI_Manager->Set_Talking(true);

	CMessageBox::MSGDESC MessageDesc;
	MessageDesc.m_eMsgType = CMessageBox::SHOP_TALK;
	MessageDesc.fPosition.x -= 150.f;
	
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MessageBox"), LEVEL_STATIC, TEXT("Layer_UI"), &MessageDesc);

	CWeapon::TYPE eItemType = (CWeapon::TYPE)pPlayer->Get_PartsItemType();
	CUI_Manager::MSGDESC eMsgDesc;

	switch (eItemType)
	{
	case Client::CWeapon::BOW:
		eMsgDesc.eMsgType = CUI_Manager::MUST_CHOICE;
		eMsgDesc.iTextureNum = BOW;
		eMsgDesc.eChoiceType = CUI_Manager::BUY_NOBUY;
		pUI_Manager->Add_MessageDesc(eMsgDesc);
		m_iCoin = 100;
		break;
	case Client::CWeapon::ARROW:
		eMsgDesc.eMsgType = CUI_Manager::MUST_CHOICE;
		eMsgDesc.iTextureNum = ARROW;
		eMsgDesc.eChoiceType = CUI_Manager::BUY_NOBUY;
		pUI_Manager->Add_MessageDesc(eMsgDesc);
		m_iCoin = 3;
		break;
	case Client::CWeapon::DOGFOOD:
		eMsgDesc.eMsgType = CUI_Manager::MUST_CHOICE;
		eMsgDesc.iTextureNum = DOG_FOOD;
		eMsgDesc.eChoiceType = CUI_Manager::BUY_NOBUY;
		pUI_Manager->Add_MessageDesc(eMsgDesc);
		m_iCoin = 10;
		break;
	case Client::CWeapon::HEART_CONTAINER:
		eMsgDesc.eMsgType = CUI_Manager::MUST_CHOICE;
		eMsgDesc.iTextureNum = HEART_CONTAINER;
		eMsgDesc.eChoiceType = CUI_Manager::BUY_NOBUY;
		pUI_Manager->Add_MessageDesc(eMsgDesc);
		m_iCoin = 50;
		break;
	case Client::CWeapon::MAGIC_ROD:
		eMsgDesc.eMsgType = CUI_Manager::MUST_CHOICE;
		eMsgDesc.iTextureNum = MAGIC_ROD;
		eMsgDesc.eChoiceType = CUI_Manager::BUY_NOBUY;
		pUI_Manager->Add_MessageDesc(eMsgDesc);
		m_iCoin = 150;
		break;
	default:
		eMsgDesc.eMsgType = CUI_Manager::PASSABLE;
		eMsgDesc.iTextureNum = TALK_DEFAULT;
		pUI_Manager->Add_MessageDesc(eMsgDesc);
		break;
	}

	pUI_Manager->Open_Message(true);

	RELEASE_INSTANCE(CUI_Manager);
	RELEASE_INSTANCE(CGameInstance);
}



void CShopNpc::Free()
{
	__super::Free();


	CCollision_Manager::Get_Instance()->Out_CollisionGroup(CCollision_Manager::COLLISION_MONSTER, this);

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	Safe_Release(m_pAABBCom);
	Safe_Release(m_pOBBCom);
	Safe_Release(m_pSPHERECom);

}

CShopNpc * CShopNpc::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CShopNpc*	pInstance = new CShopNpc(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CShopNpc"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShopNpc::Clone(void * pArg)
{
	CShopNpc*	pInstance = new CShopNpc(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CShopNpc"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

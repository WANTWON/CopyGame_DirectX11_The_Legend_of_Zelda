#include "stdafx.h"
#include "..\Public\MarinNpc.h"
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

CMarinNpc::CMarinNpc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CNpc(pDevice, pContext)
{
}

CMarinNpc::CMarinNpc(const CMarinNpc & rhs)
	: CNpc(rhs)
{
}

HRESULT CMarinNpc::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMarinNpc::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
		memcpy(&m_NpcDesc, pArg, sizeof(NPCDESC));

	_vector vPosition = XMVectorSetW(XMLoadFloat3(&m_NpcDesc.vInitPos), 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	Set_Scale(_float3(1.2f, 1.2f, 1.2f));

	return S_OK;
}

int CMarinNpc::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Find_Target();
	if (m_pTarget != nullptr && m_eState != MARIN_GET && m_eState != MARIN_GETED)
		m_pTransformCom->LookAt(m_pTarget->Get_TransformState(CTransform::STATE_POSITION));

	m_pModelCom->Set_NextAnimIndex(m_eState);
	Change_Animation(fTimeDelta);


	if (m_eState == MARIN_GET)
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(CGameInstance::Get_Instance()->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));
		_vector pPlayerPostion = pPlayer->Get_TransformState(CTransform::STATE_POSITION);

		pPlayerPostion = XMVectorSetY(pPlayerPostion, XMVectorGetY(pPlayerPostion) + 3.f);
		m_pTransformCom->Go_PosTarget(fTimeDelta, pPlayerPostion);

		if (pPlayer->Get_AnimState() == CPlayer::ITEM_GET_ED)
		{
			CUI_Manager::Get_Instance()->Open_Message(false);
			m_eState = MARIN_GETED;
		}
	}

	return OBJ_NOEVENT;
}

void CMarinNpc::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (!Check_IsinFrustum())
		return;

	if (m_eState == MARIN_GET || m_eState == MARIN_GETED)
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

HRESULT CMarinNpc::Render()
{
	__super::Render();

	return S_OK;
}


void CMarinNpc::Set_GetMode()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CBaseObj* pTarget = dynamic_cast<CBaseObj*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));

	m_eState = MARIN_GET;

	dynamic_cast<CPlayer*>(pTarget)->Set_AnimState(CPlayer::ITEM_GET_ST);
	CMessageBox::MSGDESC MessageDesc;
	MessageDesc.m_eMsgType = CMessageBox::GET_ITEM;
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MessageBox"), LEVEL_STATIC, TEXT("Layer_UI"), &MessageDesc);

	CUI_Manager::MSGDESC MsgDesc;
	MsgDesc.eMsgType = CUI_Manager::PASSABLE;
	MsgDesc.iTextureNum = CUI_Manager::MARIN;

	CUI_Manager::Get_Instance()->Add_MessageDesc(MsgDesc);
	CUI_Manager::Get_Instance()->Open_Message(true);

	m_bGet = false;
	XMStoreFloat3(&m_vLastLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	XMStoreFloat4(&m_vLastPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	RELEASE_INSTANCE(CGameInstance);
}

void CMarinNpc::Check_Navigation(_float fTimeDelta)
{
	if (m_pNavigationCom == nullptr)
		return;
}

void CMarinNpc::Send_Answer_toNPC(_uint iTextureNum)
{
	CUI_Manager* pUI_Manager = GET_INSTANCE(CUI_Manager);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pTarget = pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pTarget);
	CWeapon::TYPE eItemType = (CWeapon::TYPE)pPlayer->Get_PartsItemType();

	pUI_Manager->Clear_ChoiceButton();

	if (iTextureNum == CUIButton::NPC_TALK)
	{
		CUI_Manager::MSGDESC eMsgDesc;
		switch (m_eTalkingMode)
		{
		case Client::CMarinNpc::PERFUME:
			eMsgDesc.eMsgType = CUI_Manager::MUST_CHOICE;
			eMsgDesc.iTextureNum = MARIN_Q1_PERFUME;
			eMsgDesc.eChoiceType = CUI_Manager::MARIN_PERFUME;
			pUI_Manager->Add_MessageDesc(eMsgDesc);
			break;
		case Client::CMarinNpc::CHANGE:
			eMsgDesc.eMsgType = CUI_Manager::MUST_CHOICE;
			eMsgDesc.iTextureNum = MARIN_Q2_CHANGE;
			eMsgDesc.eChoiceType = CUI_Manager::MARIN_CHANGE;
			pUI_Manager->Add_MessageDesc(eMsgDesc);
			break;
		case Client::CMarinNpc::CROSS_DOT:
			eMsgDesc.eMsgType = CUI_Manager::MUST_CHOICE;
			eMsgDesc.iTextureNum = MARIN_Q3_VECTOR;
			eMsgDesc.eChoiceType = CUI_Manager::MARIN_VECTOR;
			pUI_Manager->Add_MessageDesc(eMsgDesc);
			break;
		case Client::CMarinNpc::CHARM:
			eMsgDesc.eMsgType = CUI_Manager::MUST_CHOICE;
			eMsgDesc.iTextureNum = MARIN_Q4_CHARM;
			eMsgDesc.eChoiceType = CUI_Manager::MARIN_CHARM;
			pUI_Manager->Add_MessageDesc(eMsgDesc);
			break;
		case Client::CMarinNpc::FINAL:
			eMsgDesc.eMsgType = CUI_Manager::PASSABLE;
			if(m_iHeart > 10)
				eMsgDesc.iTextureNum = MARIN_TALKEND_COMPLETE;
			else
				eMsgDesc.iTextureNum = MARIN_TALKEND_FAIL;
			pUI_Manager->Add_MessageDesc(eMsgDesc);
			break;
		default:
			break;
		}
		
	}

	if (iTextureNum == CUIButton::PERFUME_COMPLETE)
	{
		CUI_Manager::MSGDESC eMsgDesc;
		eMsgDesc.eMsgType = CUI_Manager::PASSABLE;
		eMsgDesc.iTextureNum = MARIN_A1_COMPLETE;
		pUI_Manager->Add_MessageDesc(eMsgDesc);
		m_eState = EXCITING;
		m_eTalkingMode++;
		m_iHeart += 3;
	}
	else if (iTextureNum == CUIButton::PERFUME_FAIL1 || iTextureNum == CUIButton::PERFUME_FAIL2 || iTextureNum == CUIButton::PERFUME_FAIL3)
	{
		CUI_Manager::MSGDESC eMsgDesc;
		eMsgDesc.eMsgType = CUI_Manager::PASSABLE;
		eMsgDesc.iTextureNum = MARIN_A1_FAIL;
		pUI_Manager->Add_MessageDesc(eMsgDesc);
		m_eState = DEPRESS;
		m_eTalkingMode++;
	}

	if (iTextureNum == CUIButton::CHANGE_COMPLETE)
	{
		CUI_Manager::MSGDESC eMsgDesc;
		eMsgDesc.eMsgType = CUI_Manager::PASSABLE;
		eMsgDesc.iTextureNum = MARIN_A2_COMPLETE;
		pUI_Manager->Add_MessageDesc(eMsgDesc);
		m_eState = SURPRISE_LP;
		m_eTalkingMode++;
		m_iHeart += 3;
	}
	else if (iTextureNum == CUIButton::CHANGE_FAIL1 || iTextureNum == CUIButton::CHANGE_FAIL2 || iTextureNum == CUIButton::CHANGE_FAIL3)
	{
		CUI_Manager::MSGDESC eMsgDesc;
		eMsgDesc.eMsgType = CUI_Manager::PASSABLE;
		eMsgDesc.iTextureNum = MARIN_A2_FAIL;
		pUI_Manager->Add_MessageDesc(eMsgDesc);
		m_eState = TREMBLE;
		m_eTalkingMode++;
	}

	if (iTextureNum == CUIButton::CROSS_COMPLETE)
	{
		CUI_Manager::MSGDESC eMsgDesc;
		eMsgDesc.eMsgType = CUI_Manager::MUST_CHOICE;
		eMsgDesc.iTextureNum = MARIN_Q3_DOT;
		eMsgDesc.eChoiceType = CUI_Manager::MARIN_DOT;
		pUI_Manager->Add_MessageDesc(eMsgDesc);
		m_eState = TALK2;
	}
	else if (iTextureNum == CUIButton::CROSS_FAIL1 || iTextureNum == CUIButton::CROSS_FAIL2 )
	{
		CUI_Manager::MSGDESC eMsgDesc;
		eMsgDesc.eMsgType = CUI_Manager::PASSABLE;
		eMsgDesc.iTextureNum = MARIN_Q3_VECTOR_FAIL;
		pUI_Manager->Add_MessageDesc(eMsgDesc);

		eMsgDesc.eMsgType = CUI_Manager::PASSABLE;
		eMsgDesc.iTextureNum = MARIN_Q3_FAIL;
		pUI_Manager->Add_MessageDesc(eMsgDesc);
		m_eState = DEPRESS;
		m_eTalkingMode++;
	}

	if (iTextureNum == CUIButton::DOT_COMPLETE)
	{
		CUI_Manager::MSGDESC eMsgDesc;
		eMsgDesc.eMsgType = CUI_Manager::PASSABLE;
		eMsgDesc.iTextureNum = MARIN_Q3_COMPLETE;
		pUI_Manager->Add_MessageDesc(eMsgDesc);
		m_eTalkingMode++;
		m_iHeart += 3;
	}
	else if (iTextureNum == CUIButton::DOT_FAIL1 || iTextureNum == CUIButton::DOT_FAIL2)
	{
		CUI_Manager::MSGDESC eMsgDesc;
		eMsgDesc.eMsgType = CUI_Manager::PASSABLE;
		eMsgDesc.iTextureNum = MARIN_Q3_DOT_FAIL;
		pUI_Manager->Add_MessageDesc(eMsgDesc);

		eMsgDesc.eMsgType = CUI_Manager::PASSABLE;
		eMsgDesc.iTextureNum = MARIN_Q3_FAIL;
		pUI_Manager->Add_MessageDesc(eMsgDesc);
		m_eState = DEPRESS;
		m_eTalkingMode++;
	}

	if (iTextureNum == CUIButton::CHARM_1 || iTextureNum == CUIButton::CHARM_2 || iTextureNum == CUIButton::CHARM_3 || iTextureNum == CUIButton::CHARM_4)
	{
		CUI_Manager::MSGDESC eMsgDesc;
		eMsgDesc.eMsgType = CUI_Manager::PASSABLE;
		eMsgDesc.iTextureNum = MARIN_A4_COMPLETE;
		pUI_Manager->Add_MessageDesc(eMsgDesc);
		m_eTalkingMode++;
		m_iHeart += 3;
	}

	if (iTextureNum == CUIButton::LOVEU)
	{
		if (m_bEnd)
		{
			CUI_Manager::MSGDESC eMsgDesc;
			eMsgDesc.eMsgType = CUI_Manager::PASSABLE;
			if (m_iHeart > 10)
				eMsgDesc.iTextureNum = MARIN_TALKEND_COMPLETE;
			else
				eMsgDesc.iTextureNum = MARIN_TALKEND_FAIL;
			pUI_Manager->Add_MessageDesc(eMsgDesc);
		}
		else if (m_eTalkingMode == FINAL)
		{
			CUI_Manager::MSGDESC eMsgDesc;
			eMsgDesc.eMsgType = CUI_Manager::MUST_CHOICE;
			eMsgDesc.iTextureNum = MARIN_Q5_FINAL;
			eMsgDesc.eChoiceType = CUI_Manager::MARIN_DATEME;
			m_eState = TALK2;
			pUI_Manager->Add_MessageDesc(eMsgDesc);
			m_bEnd = true;
			m_bGet = true;
		}
		else
		{
			CUI_Manager::MSGDESC eMsgDesc;
			eMsgDesc.eMsgType = CUI_Manager::PASSABLE;
			eMsgDesc.iTextureNum = MARIN_LOVEUFAIL;
			m_eState = SCOLD_LP;
			pUI_Manager->Add_MessageDesc(eMsgDesc);
			m_bEnd = true;
		}
	}

	if (iTextureNum == CUIButton::DATE_WITH_ME)
	{
		CUI_Manager::MSGDESC eMsgDesc;
		eMsgDesc.eMsgType = CUI_Manager::PASSABLE;
		eMsgDesc.iTextureNum = MARIN_A5_COMPLETE;
		m_eState = EXCITING;
		pUI_Manager->Add_MessageDesc(eMsgDesc);
	}
	else if (iTextureNum == CUIButton::SORRY)
	{
		CUI_Manager::MSGDESC eMsgDesc;
		eMsgDesc.eMsgType = CUI_Manager::PASSABLE;
		eMsgDesc.iTextureNum = MARIN_A5_FAIL;
		pUI_Manager->Add_MessageDesc(eMsgDesc);
		m_eState = DEPRESS;
	}

	RELEASE_INSTANCE(CUI_Manager);
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CMarinNpc::Ready_Components(void * pArg)
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
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Marin"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;

	/* For.Com_SPHERE */
	ColliderDesc.vScale = _float3(3.f, 3.f, 3.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMarinNpc::SetUp_ShaderResources()
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

HRESULT CMarinNpc::SetUp_ShaderID()
{
	return S_OK;
}

void CMarinNpc::Change_Animation(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CMarinNpc::TALK:
	case Client::CMarinNpc::IDLE:
	case Client::CMarinNpc::EXCITING:
	case Client::CMarinNpc::DEPRESS:
	case Client::CMarinNpc::TREMBLE:
	case Client::CMarinNpc::SURPRISE_LP:
		m_fAnimSpeed = 2.f;
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop);
		break;
	case Client::CMarinNpc::MARIN_GET:
		m_fAnimSpeed = 1.f;
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop);
		break;
	case Client::CMarinNpc::MARIN_GETED:
		m_fAnimSpeed = 2.f;
		m_bIsLoop = false;
		m_pTransformCom->LookDir(XMLoadFloat3(&m_vLastLook));
		
		_vector vPos = XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f);
		m_pTransformCom->Go_PosTarget(fTimeDelta, vPos);

		if (m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop))
		{
			m_eState = IDLE;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, (XMLoadFloat4(&m_vLastPosition)));
		}
		break;
	}
}

void CMarinNpc::Change_Message()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CUI_Manager* pUI_Manager = GET_INSTANCE(CUI_Manager);
	CGameObject* pTarget = pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pTarget);
	pUI_Manager->Set_Talking(true);

	CMessageBox::MSGDESC MessageDesc;
	MessageDesc.m_eMsgType = CMessageBox::MARIN_TALK;
	MessageDesc.fPosition.x -= 150.f;
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MessageBox"), LEVEL_STATIC, TEXT("Layer_UI"), &MessageDesc);

	CUI_Manager::MSGDESC eMsgDesc;
	eMsgDesc.eMsgType = CUI_Manager::MUST_CHOICE;
	eMsgDesc.iTextureNum = MARIN_TALK;
	eMsgDesc.eChoiceType = CUI_Manager::TALK_NOTTALK;
	pUI_Manager->Add_MessageDesc(eMsgDesc);
	
	pUI_Manager->Open_Message(true);

	RELEASE_INSTANCE(CUI_Manager);
	RELEASE_INSTANCE(CGameInstance);
}



void CMarinNpc::Free()
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

CMarinNpc * CMarinNpc::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMarinNpc*	pInstance = new CMarinNpc(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMarinNpc"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMarinNpc::Clone(void * pArg)
{
	CMarinNpc*	pInstance = new CMarinNpc(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMarinNpc"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

#include "stdafx.h"
#include "..\Public\CuccoKeeper.h"
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

CCuccoKeeperNpc::CCuccoKeeperNpc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CNpc(pDevice, pContext)
{
}

CCuccoKeeperNpc::CCuccoKeeperNpc(const CCuccoKeeperNpc & rhs)
	: CNpc(rhs)
{
}

HRESULT CCuccoKeeperNpc::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCuccoKeeperNpc::Initialize(void* pArg)
{
	if (pArg != nullptr)
		memcpy(&m_NpcDesc, pArg, sizeof(NPCDESC));


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	_vector vPosition = XMVectorSetW(XMLoadFloat3(&m_NpcDesc.vInitPos), 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	Set_Scale(_float3(1.2f, 1.2f, 1.2f));

	switch (m_NpcDesc.eNpcType)
	{
	case CUCCO_KEEPER:
		m_eState = IDLE_CUCCO;
		m_eIdleState = IDLE_CUCCO;
		m_eTalkState = TALK_CUCCO;
		break;
	case MADAM:
		m_eState = IDLE_MADAM;
		m_eIdleState = IDLE_MADAM;
		m_eTalkState = TALK_MADAM;
		break;
	case GRANDMA:
		m_eState = IDLE_GRANDMA_BROOM;
		m_eIdleState = IDLE_GRANDMA_BROOM;
		m_eTalkState = TALK_GRANDMA;
		break;
	case CHILD:
		m_eState = APPEAL;
		m_eIdleState = APPEAL;
		m_eTalkState = TALK_CHILD;
		break;
	default:
		break;
	}
	
	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//	CData_Manager* pData_Manager = GET_INSTANCE(CData_Manager);
	//	char cName[MAX_PATH];
	//	ZeroMemory(cName, sizeof(char) * MAX_PATH);
	//	pData_Manager->TCtoC(TEXT("GrandmaUlrira"), cName);
	//	pData_Manager->Conv_Bin_Model(m_pModelCom, cName, CData_Manager::DATA_ANIM);
	//	//ERR_MSG(TEXT("Save_Bin_Model"));
	//	RELEASE_INSTANCE(CData_Manager);

	//RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

int CCuccoKeeperNpc::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	switch (m_NpcDesc.eNpcType)
	{
	case CUCCO_KEEPER:
		CuccoKeeper_Tick(fTimeDelta);
		break;
	case MADAM:
		Madam_Tick(fTimeDelta);
		break;
	case GRANDMA:
		Grandma_Tick(fTimeDelta);
		break;
	case CHILD:
		Child_Tick(fTimeDelta);
		break;
	default:
		break;
	} 

	

	m_pModelCom->Set_NextAnimIndex(m_eState);
	Change_Animation(fTimeDelta);


	return OBJ_NOEVENT;
}

void CCuccoKeeperNpc::Late_Tick(_float fTimeDelta)
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
		if (m_eState == m_eIdleState)
		{
			pButton->Set_TexType(CUIButton::TALK);
			pButton->Set_Visible(true);
		}

		_float2 fPosition = pPlayer->Get_ProjPosition();
		fPosition.y = g_iWinSizeY - fPosition.y;
		fPosition.x += 50.f;
		fPosition.y -= 30.f;
		pButton->Set_Position(fPosition);

		if (CGameInstance::Get_Instance()->Key_Up(DIK_A) && m_eState != m_eTalkState)
		{
			CUI_Manager::Get_Instance()->Add_TalkingNpc(this);
			CCamera* pCamera = CCameraManager::Get_Instance()->Get_CurrentCamera();
			dynamic_cast<CCamera_Dynamic*>(pCamera)->Set_CamMode(CCamera_Dynamic::CAM_TALK);
			pButton->Set_Visible(false);
			pPlayer->Set_Stop(true);
			m_pTransformCom->LookAt(pPlayer->Get_TransformState(CTransform::STATE_POSITION));
			m_eState = m_eTalkState;
			Change_Message();
		}
		m_bFirst = false;
	}
	else if (!m_bFirst)
	{
		pButton->Set_Visible(false);
		m_eState = m_eIdleState;
		m_bFirst = true;
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CCuccoKeeperNpc::Render()
{
	__super::Render();

	return S_OK;
}


void CCuccoKeeperNpc::Check_Navigation(_float fTimeDelta)
{
	if (m_pNavigationCom == nullptr)
		return;
}


HRESULT CCuccoKeeperNpc::Ready_Components(void * pArg)
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

	switch (m_NpcDesc.eNpcType)
	{
	case CUCCO_KEEPER:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("CuccoKeeper"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case MADAM:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("MadamMeowMeow"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case GRANDMA:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("GrandmaUlrira"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case CHILD:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("QuadrupletGreen"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	default:
		break;
	}

	

	CCollider::COLLIDERDESC		ColliderDesc;

	/* For.Com_SPHERE */
	ColliderDesc.vScale = _float3(3.f, 3.f, 3.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCuccoKeeperNpc::SetUp_ShaderResources()
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

HRESULT CCuccoKeeperNpc::SetUp_ShaderID()
{
	return S_OK;
}

void CCuccoKeeperNpc::Change_Animation(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CCuccoKeeperNpc::TALK_CUCCO:
	case Client::CCuccoKeeperNpc::IDLE_CUCCO:
		m_fAnimSpeed = 2.f;
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop);
		break;
	default:
		m_fAnimSpeed = 2.f;
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop);
		break;
	}
}

void CCuccoKeeperNpc::Change_Message()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CUI_Manager* pUI_Manager = GET_INSTANCE(CUI_Manager);
	CGameObject* pTarget = pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pTarget);
	pUI_Manager->Set_Talking(true);

	CMessageBox::MSGDESC MessageDesc;
	CUI_Manager::MSGDESC eMsgDesc;
	MessageDesc.m_eMsgType = CMessageBox::FIELDNPC_TALK;
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MessageBox"), LEVEL_STATIC, TEXT("Layer_UI"), &MessageDesc);


	switch (m_NpcDesc.eNpcType)
	{
	case CUCCO_KEEPER:
		eMsgDesc.eMsgType = CUI_Manager::PASSABLE;
		eMsgDesc.iTextureNum = TALK_DEFAULT;
		pUI_Manager->Add_MessageDesc(eMsgDesc);
		break;
	case MADAM:
		eMsgDesc.eMsgType = CUI_Manager::PASSABLE;
		eMsgDesc.iTextureNum = TALK_DEFAULT;
		pUI_Manager->Add_MessageDesc(eMsgDesc);
		break;
	case GRANDMA:
		eMsgDesc.eMsgType = CUI_Manager::PASSABLE;
		eMsgDesc.iTextureNum = TALK_DEFAULT;
		pUI_Manager->Add_MessageDesc(eMsgDesc);
		break;
	case CHILD:
		eMsgDesc.eMsgType = CUI_Manager::PASSABLE;
		eMsgDesc.iTextureNum = TALK_DEFAULT;
		pUI_Manager->Add_MessageDesc(eMsgDesc);
		break;
	default:
		break;
	}

	pUI_Manager->Open_Message(true);

	RELEASE_INSTANCE(CUI_Manager);
	RELEASE_INSTANCE(CGameInstance);
}

int CCuccoKeeperNpc::CuccoKeeper_Tick(_float fTimeDelta)
{
	if (m_eState != TALK_CUCCO)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		list<CGameObject*>* pCuccoList = pGameInstance->Get_ObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Cucco"));
		if (pCuccoList != nullptr)
		{
			_float fMinDistance = 999;

			for (auto& iter : *pCuccoList)
			{
				if (iter == nullptr)
					continue;

				_float fDistance = XMVectorGetX(XMVector3Length(dynamic_cast<CBaseObj*>(iter)->Get_TransformState(CTransform::STATE_POSITION) - Get_TransformState(CTransform::STATE_POSITION)));
				if (fMinDistance > fDistance)
				{
					m_pTarget = dynamic_cast<CBaseObj*>(iter);
					fMinDistance = fDistance;

				}
			}

			if (m_pTarget != nullptr)
				m_pTransformCom->LookAt(m_pTarget->Get_TransformState(CTransform::STATE_POSITION));

		}

		RELEASE_INSTANCE(CGameInstance);
	}

	return OBJ_NOEVENT;
}


int CCuccoKeeperNpc::Madam_Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CBaseObj* pTarget = dynamic_cast<CBaseObj*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));

	m_pTransformCom->LookAt(pTarget->Get_TransformState(CTransform::STATE_POSITION));

	RELEASE_INSTANCE(CGameInstance);
	return OBJ_NOEVENT;
}

int CCuccoKeeperNpc::Grandma_Tick(_float fTimeDelta)
{
	if (m_eState == m_eTalkState)
		return OBJ_NOEVENT;

	if (m_dwIdleTime + 5000 < GetTickCount())
	{
		if (m_eState == IDLE_GRANDMA)
		{
			m_eState = IDLE_GRANDMA_BROOM;
			m_eIdleState = IDLE_GRANDMA_BROOM;
			m_eTalkState = TALK_BROOM;
		}
		else
		{
			m_eState = IDLE_GRANDMA;
			m_eIdleState = IDLE_GRANDMA;
			m_eTalkState = TALK_GRANDMA;
		}

		m_dwIdleTime = GetTickCount();
	}

	if (m_eState == IDLE_GRANDMA)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CBaseObj* pTarget = dynamic_cast<CBaseObj*>(pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));

		_vector		vLook = pTarget->Get_TransformState(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector		vMyOriginalLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

		vMyOriginalLook = XMVectorSetY(vMyOriginalLook, XMVectorGetY(vLook));
		_vector vLookDot = XMVector3AngleBetweenVectors(XMVector3Normalize(vLook), XMVector3Normalize(vMyOriginalLook));
		_float fAngle = XMConvertToDegrees(XMVectorGetX(vLookDot));

		_vector vCross = XMVector3Cross(XMVector3Normalize(vLook), XMVector3Normalize(vMyOriginalLook));
		_vector vDot = XMVector3Dot(XMVector3Normalize(vCross), XMVector3Normalize(XMVectorSet(0.f, 1.f, 0.f, 0.f)));

		_float fScala = XMVectorGetX(vDot);

		
		if (fScala == -1)
			fAngle = 360 - fAngle;
	
		
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), 0.1f*fAngle);

		RELEASE_INSTANCE(CGameInstance);
	}


	return OBJ_NOEVENT;
}

int CCuccoKeeperNpc::Child_Tick(_float fTimeDelta)
{

	// Switch between Idle and Walk (based on time)
	if (m_eState == APPEAL)
	{
		if (GetTickCount() > m_dwIdleTime + (rand() % 1500) * (rand() % 3 + 1) + 3000)
		{
			m_eState = RUN;
			m_dwWalkTime = GetTickCount();

			m_eDir[DIR_X] = (rand() % 300)*0.01f - 1.f;
			m_eDir[DIR_Z] = (rand() % 300)*0.01f - 1.f;

		}
	}
	else if (m_eState == RUN)
	{
		if (GetTickCount() > m_dwWalkTime + (rand() % 3000) * (rand() % 3 + 1) + 1500)
		{
			m_eState = APPEAL;
			m_dwIdleTime = GetTickCount();
		}
	}

	// Movement
	if (m_eState == RUN)
	{
		Change_Direction();
		m_pTransformCom->Go_StraightSliding(fTimeDelta, m_pNavigationCom);
	}
	return OBJ_NOEVENT;
}


void CCuccoKeeperNpc::Free()
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

CCuccoKeeperNpc * CCuccoKeeperNpc::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCuccoKeeperNpc*	pInstance = new CCuccoKeeperNpc(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCraneGameNpc"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCuccoKeeperNpc::Clone(void * pArg)
{
	CCuccoKeeperNpc*	pInstance = new CCuccoKeeperNpc(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCraneGameNpc"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

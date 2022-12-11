#include "stdafx.h"
#include "..\Public\Albatoss.h"
#include "Player.h"
#include "CameraManager.h"
#include "MessageBox.h"
#include "MonsterBullet.h"


CAlbatoss::CAlbatoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonster(pDevice, pContext)
{
}

HRESULT CAlbatoss::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAlbatoss::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_tInfo.iMaxHp = 20;
	m_tInfo.iDamage = 4;
	m_tInfo.iCurrentHp = m_tInfo.iMaxHp;

	m_fAttackRadius = 1.5f;
	m_eMonsterID = MONSTER_PAWN;

	_vector vecPostion = XMLoadFloat3((_float3*)pArg);
	vecPostion = XMVectorSetW(vecPostion, 1.f);

	Set_Scale(_float3(1.2f, 1.2f, 1.2f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vecPostion);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	CCollision_Manager::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_MONSTER, this);
	

	return S_OK;
}

int CAlbatoss::Tick(_float fTimeDelta)
{
	if (__super::Tick(fTimeDelta))
	{
		Drop_Items();
		//CCamera_Dynamic* pCamera = dynamic_cast<CCamera_Dynamic*>(CCameraManager::Get_Instance()->Get_CurrentCamera());
	//	pCamera->Set_CamMode(CCamera_Dynamic::CAM_PLAYER);
		return OBJ_DEAD;
	}

	if (!m_bOpening)
		Opening_Motion(fTimeDelta);
	else
	{
		AI_Behaviour(fTimeDelta);
	}

	m_pModelCom->Set_NextAnimIndex(m_eState);
	Change_Animation(fTimeDelta);

	return OBJ_NOEVENT;
}

void CAlbatoss::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CAlbatoss::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CAlbatoss::Check_Navigation(_float fTimeDelta)
{
	
}

void CAlbatoss::Change_Animation(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CAlbatoss::PICCOLO_WAIT:
	case Client::CAlbatoss::HOVERING_LP:
	case Client::CAlbatoss::HOVERING_LP_FAST:
	case Client::CAlbatoss::WEAK_HOVERING:
	case Client::CAlbatoss::RUSH:
	case Client::CAlbatoss::ATTACK_FLAPPING:
		m_fAnimSpeed = 2.f;
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop);
		break;
	case Client::CAlbatoss::DEMO_POP:
		m_fAnimSpeed = 2.f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop))
		{
			m_bFirst = false;
			m_bOpening = true;
			m_eState = HOVERING_LP;
			CCamera_Dynamic* pCamera = dynamic_cast<CCamera_Dynamic*>(CCameraManager::Get_Instance()->Get_CurrentCamera());
			pCamera->Set_TargetPosition(XMVectorSet(0.f, 19.f, 0.f, 1.f));
			dynamic_cast<CPlayer*>(m_pTarget)->Set_Stop(false);

			_vector vPosition = XMLoadFloat4(&m_RushLeftPos);
			vPosition = XMVectorSetZ(vPosition, 3.f);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
			m_bIsAttacking = false;
		}
		break;
	case Client::CAlbatoss::RUSH_ST:
		m_fAnimSpeed = 2.f;
		m_bIsLoop = false;
		_vector vPlayerPos = m_pTarget->Get_TransformState(CTransform::STATE_POSITION);
		vPlayerPos = XMVectorSetZ(vPlayerPos, 3.f);
		m_pTransformCom->LookAt(vPlayerPos);
		if (m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop))
		{
			m_eState = RUSH;
		}
		break;
	case Client::CAlbatoss::RUSH_RETURN:
		m_fAnimSpeed = 2.f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop))
		{
			m_vLastDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			if (XMVectorGetY(Get_TransformState(CTransform::STATE_POSITION)) < 16.f && XMVectorGetY(m_vLastDir) < -0.5f)
			{
				m_vLastDir = XMVectorSetY(m_vLastDir, XMVectorGetY(m_vLastDir) + 0.3f);
			}
			else if (XMVectorGetY(m_vLastDir) < 0 )
				m_vLastDir = XMVectorSetY(m_vLastDir, XMVectorGetY(m_vLastDir) + 0.1f);
			
			m_pTransformCom->LookDir(m_vLastDir);
			m_eState = RUSH;
		}
		break;
	case Client::CAlbatoss::DAMAGE_HOVERING:
	case Client::CAlbatoss::DAMAGE_RUSH:
	case Client::CAlbatoss::ATTACK_CLAW_ED:
		m_fAnimSpeed = 2.f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop))
		{
			m_eState = HOVERING_LP;

		}
		break;
	case Client::CAlbatoss::ATTACK_CLAW_ST:
		m_fAnimSpeed = 2.f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop))
		{
			m_bIsAttacking = true;
			m_bDownHovering = false;
		}
		break;
	case Client::CAlbatoss::ATTACK_CLAW:
		m_fAnimSpeed = 2.f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop))
		{
			m_iClawCount++;
		}
		break;
	case Client::CAlbatoss::ATTACK_CLAW_LP:
		m_fAnimSpeed = 2.f;
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop);
		break;
	case Client::CAlbatoss::HOVERING_ST:
	case Client::CAlbatoss::WEAK_HOVERING_ST:
		m_fAnimSpeed = 2.f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop))
			m_eState = m_eHoverState;
		break;
	case Client::CAlbatoss::ATTACK_FLAPPING_ST:
		m_fAnimSpeed = 2.f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop))
		{
			m_dwFlappingTime = GetTickCount();
			m_eState = ATTACK_FLAPPING;
			m_bIsAttacking = true;
		}
		break;
	case Client::CAlbatoss::ATTACK_FLAPPING_ED:
		m_fAnimSpeed = 2.f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop))
		{
			m_eState = HOVERING_LP;
			m_bIsAttacking = false;
			m_bDownHovering = false;
			m_eAttackMode = RUSH_STATE;

		}
		break;
	case Client::CAlbatoss::DEAD:
		m_fAnimSpeed = 2.f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop))
		{
			m_bDead = true;

		}
		break;
	default:
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop);
		break;
	}
	
	
}

HRESULT CAlbatoss::Ready_Components(void * pArg)
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 6.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(1.0f);
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_TOWER, TEXT("Albatoss"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_SHPERE */
	CCollider::COLLIDERDESC		ColliderDesc;
	ColliderDesc.vScale = _float3(4.f, 4.f, 4.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vPosition = _float3(0.f, 2.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_TOWER, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CAlbatoss::SetUp_ShaderResources()
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

_bool CAlbatoss::IsDead()
{
	if (m_bDead && m_eState == STATE::DEAD)//&& m_dwDeathTime + 1000 < GetTickCount())
		return true;
	else if (m_bDead && m_eState != STATE::DEAD)
	{
		m_dwDeathTime = GetTickCount();
		m_eState = STATE::DEAD;
	}

	return false;
}

void CAlbatoss::Find_Target()
{
	if (!m_bIsAttacking && !m_bHit && !m_bDead)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		CGameObject* pTarget = pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pTarget);

		if (pPlayer)
		{
			if (pPlayer->Get_Dead())
			{
				if (m_bAggro)
				{
					m_pTarget = nullptr;
					m_eState = STATE::HOVERING_LP;
					m_bAggro = false;

				}
				return;
			}

			if (pTarget)
			{
				CTransform* PlayerTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));
				_vector vTargetPos = PlayerTransform->Get_State(CTransform::STATE_POSITION);
				m_fDistanceToTarget = XMVectorGetX(XMVector3Length(Get_TransformState(CTransform::STATE_POSITION) - vTargetPos));
				m_pTarget = dynamic_cast<CBaseObj*>(pTarget);
			}
			else
				m_pTarget = nullptr;
		}
	}
}

void CAlbatoss::Follow_Target(_float fTimeDelta)
{
	if (m_pTarget == nullptr)
		return;

	_vector vTargetPos = (m_pTarget)->Get_TransformState(CTransform::STATE_POSITION);

	m_pTransformCom->LookAt(vTargetPos);
	m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);

	m_bIsAttacking = false;
}

void CAlbatoss::AI_Behaviour(_float fTimeDelta)
{
	if ( m_eState == DAMAGE_HOVERING || m_eState == DAMAGE_RUSH || m_eState == DEAD)
		return;

	// Check for Target, AggroRadius
	Find_Target();

	if (XMVectorGetY(m_pTarget->Get_TransformState(CTransform::STATE_POSITION)) < 15.f)
	{
		CCamera_Dynamic* pCamera = dynamic_cast<CCamera_Dynamic*>(CCameraManager::Get_Instance()->Get_CurrentCamera());
		pCamera->Set_CamMode(CCamera_Dynamic::CAM_PLAYER);
	}
	else
	{
		CCamera_Dynamic* pCamera = dynamic_cast<CCamera_Dynamic*>(CCameraManager::Get_Instance()->Get_CurrentCamera());
		pCamera->Set_CamMode(CCamera_Dynamic::CAM_TARGET);
		pCamera->Set_TargetPosition(XMVectorSet(0.f, 19.f, 2.f, 1.f));
	}



	switch (m_eAttackMode)
	{
	case RUSH_STATE:
		Rush_Attack(fTimeDelta);
		break;
	case CLAW_STATE:
		Claw_Attack(fTimeDelta);
		break;
	case FLAPPING:
		Flapping_Attack(fTimeDelta);
		break;
	default:
		break;
	}


	
	
	
}

HRESULT CAlbatoss::Drop_Items()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	LEVEL iLevel = (LEVEL)pGameInstance->Get_CurrentLevelIndex();

	CPrizeItem::ITEMDESC ItemDesc;
	ItemDesc.vPosition = _float3(0.f, 16.5, 3.f);
	ItemDesc.eInteractType = CPrizeItem::PRIZE;
	ItemDesc.eType = CPrizeItem::TAIL_KEY;
	

	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PrizeItem"), iLevel, TEXT("PrizeItem"), &ItemDesc);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CAlbatoss::Opening_Motion(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CUI_Manager* pUI_Manager = GET_INSTANCE(CUI_Manager);

	if (CGameInstance::Get_Instance()->Key_Up(DIK_SPACE))
	{
		m_bFirst = false;
		m_bOpening = true;
		m_eState = HOVERING_LP;
		CCamera_Dynamic* pCamera = dynamic_cast<CCamera_Dynamic*>(CCameraManager::Get_Instance()->Get_CurrentCamera());
		pCamera->Set_TargetPosition(XMVectorSet(0.f, 19.f, 0.f, 1.f));
		dynamic_cast<CPlayer*>(m_pTarget)->Set_Stop(false);

		_vector vPosition = XMLoadFloat4(&m_RushLeftPos);
		vPosition = XMVectorSetZ(vPosition, 3.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		m_bIsAttacking = false;
	}

	Find_Target();

	_vector vPlayerPos = m_pTarget->Get_TransformState(CTransform::STATE_POSITION);
	if (!m_bOpening && XMVectorGetY(vPlayerPos) > 15)
	{
		if (!m_bFirst)
		{
			
			CGameObject* pTarget = pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
			CPlayer* pPlayer = dynamic_cast<CPlayer*>(pTarget);

			CMessageBox::MSGDESC MessageDesc;
			MessageDesc.m_eMsgType = CMessageBox::ALBATOSS_TALK;
			//MessageDesc.fPosition.y = 100.f;
			pUI_Manager->Set_Talking(true);

			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MessageBox"), LEVEL_STATIC, TEXT("Layer_UI"), &MessageDesc);

			CUI_Manager::MSGDESC eMsgDesc;
			eMsgDesc.eMsgType = CUI_Manager::PASSABLE;
			eMsgDesc.iTextureNum = MSG1;
			pUI_Manager->Add_MessageDesc(eMsgDesc);

			eMsgDesc.eMsgType = CUI_Manager::PASSABLE;
			eMsgDesc.iTextureNum = MSG2;
			pUI_Manager->Add_MessageDesc(eMsgDesc);

			pUI_Manager->Open_Message(true);
			m_dwOpeningTime = GetTickCount();
			CCamera_Dynamic* pCamera = dynamic_cast<CCamera_Dynamic*>(CCameraManager::Get_Instance()->Get_CurrentCamera());
			pCamera->Set_CamMode(CCamera_Dynamic::CAM_TARGET);
			dynamic_cast<CPlayer*>(m_pTarget)->Set_Stop(true);
			m_vTargetDistance.y = 2.f;
			m_vTargetDistance.z = 7.f;
			m_bFirst = true;
		}

		m_eState = DEMO_POP;
		
		CCamera_Dynamic* pCamera = dynamic_cast<CCamera_Dynamic*>(CCameraManager::Get_Instance()->Get_CurrentCamera());
		_vector vPosition = vPlayerPos;
		vPosition += XMLoadFloat3(&m_vTargetDistance);
		pCamera->Set_TargetPosition(vPosition);

		if (m_bFirst && m_dwOpeningTime + 18000 < GetTickCount())
		{
			m_vTargetDistance.x = 3.f;
			m_vTargetDistance.y = 3.f;
			m_vTargetDistance.z = 6.f;
		}
		else if (m_bFirst && m_dwOpeningTime + 15500 < GetTickCount())
		{
			m_vTargetDistance.x = 3.f;
			m_vTargetDistance.y = 3.f;
			m_vTargetDistance.z += 0.01f;
		}
		else if(m_bFirst && m_dwOpeningTime + 13500 < GetTickCount())
		{
			m_vTargetDistance.x = 3.f;
			m_vTargetDistance.y = 3.f;
			m_vTargetDistance.z = 6.f;
		}
		else if (m_bFirst && m_dwOpeningTime + 8000 < GetTickCount())
		{
			m_vTargetDistance.x = 1.f;
			m_vTargetDistance.y = 7.f;
			m_vTargetDistance.z = 8.f;
		}
		else if (m_bFirst && m_bMessageAutoPass && m_dwOpeningTime + 7000 < GetTickCount())
		{
			pUI_Manager->Set_NextMessage();
			m_bMessageAutoPass = false;
		}
		else if (m_bFirst&& !m_bMessageAutoPass && m_dwOpeningTime + 4000 < GetTickCount())
		{
			pUI_Manager->Set_NextMessage();
			m_bMessageAutoPass = true;
		}
	}

	RELEASE_INSTANCE(CUI_Manager);
	RELEASE_INSTANCE(CGameInstance);
}

void CAlbatoss::Rush_Attack(_float fTimeDelta)
{
	_vector vTargetPos = (m_pTarget)->Get_TransformState(CTransform::STATE_POSITION);
	_vector vPosition = Get_TransformState(CTransform::STATE_POSITION);

	if (!m_bIsAttacking)
	{
		if (m_eState != m_eHoverState && m_eState != m_eHoverStState)
		{
			m_eRushDir =  rand() % 2 == 0 ? RUSH_RIGHT : RUSH_LEFT;
			m_eState = m_eHoverStState;
		}
			
		if (m_eRushDir == RUSH_LEFT)
		{
			m_pTransformCom->Go_PosDir(fTimeDelta, XMVectorSet(-1.f, 0.5f, 0.f, 0.f));
			if (XMVectorGetX(vPosition) < m_RushLeftPos.x)
			{
				vPosition = XMVectorSetX(vPosition, m_RushLeftPos.x);
				//vPosition = XMVectorSetY(vPosition, m_RushLeftPos.y);

				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
				m_bIsAttacking = true;
			}
		}
		else
		{
			m_pTransformCom->Go_PosDir(fTimeDelta, XMVectorSet(1.f, 0.5f, 0.f, 0.f));
			if (XMVectorGetX(vPosition) > m_RushRightPos.x)
			{
				vPosition = XMVectorSetX(vPosition, m_RushRightPos.x);
				//vPosition = XMVectorSetY(vPosition, m_RushRightPos.y);
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
				m_bIsAttacking = true;
			}
		}

		
	}


	if (m_bIsAttacking)
	{
		if (m_eAttackMode == RUSH_STATE)
		{
			if (m_eState != RUSH_ST &&m_eState != RUSH_RETURN && m_eState != RUSH)
			{
				m_eState = RUSH_ST;
			}

			if (m_eState == RUSH || m_eState == RUSH_RETURN)
			{
				if (XMVectorGetY(Get_TransformState(CTransform::STATE_POSITION)) <= XMVectorGetY(vTargetPos) + 1 &&
					XMVectorGetY(Get_TransformState(CTransform::STATE_LOOK)) < 0.f)
				{
					m_eState = RUSH_RETURN;
				}

				

				if (XMVectorGetX(vPosition) < m_RushLeftPos.x)
				{
					m_eState = RUSH_ST;
					vPosition = XMVectorSetX(vPosition, m_RushLeftPos.x);
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
					m_iRushCount++;
				}
				else if (XMVectorGetX(vPosition) > m_RushRightPos.x)
				{
					m_eState = RUSH_ST;
					vPosition = XMVectorSetX(vPosition, m_RushRightPos.x);
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
					m_iRushCount++;
				}
				else
				{
					if (XMVectorGetY(Get_TransformState(CTransform::STATE_POSITION)) < 16.f)
					{
						m_vLastDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
						if (XMVectorGetY(m_vLastDir) < -0.5)
							m_vLastDir = XMVectorSetY(m_vLastDir, XMVectorGetY(m_vLastDir) + 0.1f);
						m_pTransformCom->LookDir(m_vLastDir);
					}
					
					m_pTransformCom->Go_Straight(fTimeDelta);
				}


				if (m_iRushCount % 5 == 4)
				{
					m_bIsAttacking = false;
					m_eAttackMode = CLAW_STATE;
					m_iRushCount = 0;
				}
			}
		}
	}
}

void CAlbatoss::Claw_Attack(_float fTimeDelta)
{
	_vector vTargetPos = (m_pTarget)->Get_TransformState(CTransform::STATE_POSITION);
	_vector vPosition = Get_TransformState(CTransform::STATE_POSITION);

	if (!m_bIsAttacking)
	{
		if (!m_bDownHovering)
		{
			if (m_eState != m_eHoverState && m_eState != m_eHoverStState)
				m_eState = m_eHoverStState;

			if (m_eState == m_eHoverState)
			{
				m_pTransformCom->Go_PosDir(fTimeDelta*0.5f, XMVectorSet(0.f, 1.f, 0.f, 0.f));
				if (XMVectorGetY(vPosition) > m_ClawingPos.y)
				{
					_vector vDir = vTargetPos - vPosition;
					vDir = XMVectorSetY(vDir, 0.f);
					m_pTransformCom->LookDir(vDir);
					vPosition = XMVectorSetX(vPosition, rand() % 2 == 0 ? 5.f : -5.f);
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
					m_bDownHovering = true;
				}
			}
			
		}
		else if (m_bDownHovering)
		{
			if (m_eState == m_eHoverState)
			{
				_vector vDir = vTargetPos - vPosition;
				vDir = XMVectorSetY(vDir, 0.f);
				m_pTransformCom->LookDir(vDir);
				m_pTransformCom->Go_PosDir(fTimeDelta*0.5f, XMVectorSet(0.f, -1.f, 0.f, 0.f));
				if (XMVectorGetY(vPosition) < XMVectorGetY(vTargetPos) + 5)
				{
					m_eState = ATTACK_CLAW_ST;
				}
					
			}
		}
	}


	if (m_bIsAttacking)
	{
		if (CCollision_Manager::Get_Instance()->CollisionwithGroup(CCollision_Manager::COLLISION_PLAYER, m_pSPHERECom))
		{
			m_eState = ATTACK_CLAW;
		}
		else
		{
			if (m_eState == ATTACK_CLAW)
			{
				m_bIsAttacking = false;
				m_bDownHovering = false;
			}
			else
				m_eState = ATTACK_CLAW_LP;
		}

		if (m_eState == ATTACK_CLAW_LP)
		{
			m_pTransformCom->Go_PosTarget(fTimeDelta, vTargetPos);

			if (XMVectorGetY(Get_TransformState(CTransform::STATE_POSITION)) < XMVectorGetY(vTargetPos))
			{
				m_bIsAttacking = false;
				m_bDownHovering = false;
			}
		}
		else if (m_eState == ATTACK_CLAW)
		{
			_vector vDir = XMVector3Normalize(vTargetPos - vPosition); 
			vDir = XMVectorSetY(vDir, 0.f);
			m_pTransformCom->Go_PosDir(fTimeDelta*0.2f, vDir);
		}

		if (m_iClawCount % 4 == 3)
		{
			m_bIsAttacking = false;
			m_eAttackMode = FLAPPING;
			m_iClawCount = 0;
		}
		
	}

	
}

void CAlbatoss::Flapping_Attack(_float fTimeDelta)
{
	_vector vTargetPos = (m_pTarget)->Get_TransformState(CTransform::STATE_POSITION);
	_vector vPosition = Get_TransformState(CTransform::STATE_POSITION);

	if (!m_bIsAttacking)
	{
		if (!m_bDownHovering)
		{
			if (m_eState != m_eHoverState && m_eState != m_eHoverStState)
				m_eState = m_eHoverStState;

			if (m_eState == m_eHoverState)
			{
				m_pTransformCom->Go_PosDir(fTimeDelta*0.5f, XMVectorSet(0.f, 1.f, 0.f, 0.f));
				if (XMVectorGetY(vPosition) > m_ClawingPos.y)
				{
					_vector vDir = vTargetPos - vPosition;
					vDir = XMVectorSetY(vDir, 0.f);
					m_pTransformCom->LookDir(vDir);
					vPosition = XMVectorSetX(vPosition, rand() % 2 == 0 ? 7.f : -7.f);
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
					m_bDownHovering = true;
				}
			}

		}
		else if (m_bDownHovering)
		{
			if (m_eState == m_eHoverState)
			{
				_vector vDir = vTargetPos - vPosition;
				vDir = XMVectorSetY(vDir, 0.f);
				m_pTransformCom->LookDir(vDir);
				m_pTransformCom->Go_PosDir(fTimeDelta*0.5f, XMVectorSet(0.f, -1.f, 0.f, 0.f));
				if (XMVectorGetY(vPosition) < XMVectorGetY(vTargetPos) + 1)
				{
					m_eState = ATTACK_FLAPPING_ST;
					m_dwBulletTime = GetTickCount();
				}

			}
		}
	}


	if (m_bIsAttacking)
	{
		
		if (m_eState == ATTACK_FLAPPING)
		{
			_vector vDir = XMVector3Normalize(vTargetPos - vPosition);
			vDir = XMVectorSetY(vDir, 0.f);
			m_pTransformCom->LookDir(vDir);

			if (m_dwBulletTime + 200 < GetTickCount())
			{
				m_bMakeBullet = true;
				m_dwBulletTime = GetTickCount();
			}

			if (m_bMakeBullet)
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				CMonsterBullet::BULLETDESC BulletDesc;
				BulletDesc.eBulletType = CMonsterBullet::ALBATOSS;
				BulletDesc.vInitPositon = Get_TransformState(CTransform::STATE_POSITION);
				BulletDesc.vLook = Get_TransformState(CTransform::STATE_LOOK);
				if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MonsterBullet"), LEVEL_TOWER, TEXT("Layer_Bullet"), &BulletDesc)))
					return;
				RELEASE_INSTANCE(CGameInstance);
				m_bMakeBullet = false;
			}

		}
		
		

	

		if (m_dwFlappingTime + 5000 < GetTickCount())
		{
			m_eState = ATTACK_FLAPPING_ED;
			m_bMakeBullet = false;
		}

	}

}



_uint CAlbatoss::Take_Damage(float fDamage, void * DamageType, CBaseObj * DamageCauser)
{
	if (m_eState == DAMAGE_HOVERING || m_eState == STATE::DAMAGE_RUSH)
		return 0;

	_uint fHp = __super::Take_Damage(fDamage, DamageType, DamageCauser);

	if (fHp > 0)
	{
		if (!m_bDead)
		{
			m_bHit = true;
			if(m_eAttackMode == RUSH_STATE)
				m_eState = STATE::DAMAGE_RUSH;
			else
				m_eState = STATE::DAMAGE_HOVERING;

			if (fHp < 10 && m_eHoverState != WEAK_HOVERING)
			{
				m_eHoverStState = WEAK_HOVERING_ST;
				m_eHoverState = WEAK_HOVERING;
			}

		}

		m_bAggro = true;
		m_bIsAttacking = false;
		m_dwAttackTime = GetTickCount();

		return fHp;
	}
	else
		m_eState = STATE::DEAD;

	return 0;
}

CAlbatoss * CAlbatoss::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CAlbatoss*	pInstance = new CAlbatoss(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CAlbatoss"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAlbatoss::Clone(void * pArg)
{
	CAlbatoss*	pInstance = new CAlbatoss(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CAlbatoss"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAlbatoss::Free()
{
	__super::Free();

	CCollision_Manager::Get_Instance()->Out_CollisionGroup(CCollision_Manager::COLLISION_MONSTER, this);
	Safe_Release(m_pSPHERECom);
}


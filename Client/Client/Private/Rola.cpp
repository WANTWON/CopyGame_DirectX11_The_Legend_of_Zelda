#include "stdafx.h"
#include "..\Public\Rola.h"
#include "Player.h"

CRola::CRola(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonster(pDevice, pContext)
{
}

HRESULT CRola::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRola::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_tInfo.iMaxHp = 3;
	m_tInfo.iDamage = 20;
	m_tInfo.iCurrentHp = m_tInfo.iMaxHp;

	m_fAttackRadius = 2.f;
	m_fPatrolRadius = 7.f;
	m_eMonsterID = MONSTER_MOBLINSWORD;

	_vector vecPostion = XMLoadFloat3((_float3*)pArg);
	vecPostion = XMVectorSetW(vecPostion, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vecPostion);
	return S_OK;
}

int CRola::Tick(_float fTimeDelta)
{
	if (__super::Tick(fTimeDelta))
		return OBJ_DEAD;

	if (m_fDistanceToTarget < 2.f)
	{
		if (CGameInstance::Get_Instance()->Key_Down(DIK_X))
		{
			_int iNumRand = rand() % 2;
			if (iNumRand == 0)
				Take_Damage(1, nullptr, nullptr);
			else
			{
				m_eState = GUARD;
				m_pTransformCom->LookAt(dynamic_cast<CPlayer*>(m_pTarget)->Get_TransformState(CTransform::STATE_POSITION));
				m_bAggro = true;
			}
				
		}
		
		if (m_bIsAttacking)
		{
			CPlayer::ANIM ePlayerState = dynamic_cast<CPlayer*>(m_pTarget)->Get_AnimState();
			if (ePlayerState == CPlayer::SHIELD_LP || ePlayerState == CPlayer::SHIELD_ST)
			{
					m_eState = STAGGER;
					dynamic_cast<CPlayer*>(m_pTarget)->Set_AnimState(CPlayer::SHIELD_HIT);
					m_bIsAttacking = false;
			}	
		}	
	}




	AI_Behaviour(fTimeDelta);
	m_pModelCom->Set_CurrentAnimIndex(m_eState);
	Change_Animation(fTimeDelta);

	//m_pAABBCom->Update(m_pTransformCom->Get_WorldMatrix());
	m_pOBBCom->Update(m_pTransformCom->Get_WorldMatrix());
	return OBJ_NOEVENT;
}

void CRola::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CRola::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	//m_pAABBCom->Render();
	m_pOBBCom->Render();
	/*m_pSPHERECom->Render();*/

#endif
	return S_OK;
}

void CRola::Change_Animation(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CRola::KYOROKYORO:
	case Client::CRola::IDLE:
	case Client::CRola::STANCE_WAIT:
	case Client::CRola::WALK:
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta*2, m_bIsLoop);
		break;
	case Client::CRola::DAMAGE_F:
	case Client::CRola::DAMAGE_B:
	case Client::CRola::GUARD:
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop))
		{
			m_eState = IDLE;
			m_bHit = false;
		}
		break;
	case Client::CRola::FIND:
	case Client::CRola::STANCE_WALK:
	case Client::CRola::STAGGER:
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop))
		{
			m_bIsAttacking = false;
			m_eState = IDLE;
			m_bHit = false;
		}
		break;
	case Client::CRola::DEAD_F:
		m_pTransformCom->Go_Backward(fTimeDelta * 4);
		m_bIsLoop = false;
		m_pTransformCom->Go_PosDir(fTimeDelta, XMVectorSet(0.f, 0.1f, 0.f, 0.f));
		if (m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop))
			m_bDead = true;
		break;
	case Client::CRola::DEAD_B:
		m_pTransformCom->Go_Straight(fTimeDelta * 4);
		m_bIsLoop = false;
		m_pTransformCom->Go_PosDir(fTimeDelta, XMVectorSet(0.f, 0.1f, 0.f, 0.f));
		if (m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop))
			m_bDead = true;
		break;
	case Client::CRola::DEAD_FIRE:
		m_bIsLoop = false;
		m_pTransformCom->Go_PosDir(fTimeDelta, XMVectorSet(0.f, 0.1f, 0.f, 0.f));
		if (m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop))
			m_bDead = true;
		break;
	default:
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop);
		break;
	}
}

HRESULT CRola::Ready_Components(void * pArg)
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 2.0f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(1.0f);
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_Rola"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vScale = _float3(0.7f, 0.7f, 0.7f);
	ColliderDesc.vPosition = _float3(0.f, 0.7f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_AABB"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), (CComponent**)&m_pAABBCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_OBB*/
	ColliderDesc.vScale = _float3(1.f, 2.f, 1.f);
	ColliderDesc.vRotation = _float3(0.f, XMConvertToRadians(0.0f), 0.f);
	ColliderDesc.vPosition = _float3(0.f, 0.7f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_OBB"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_SPHERE */
	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_SPHERE"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSPHERECom, &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CRola::SetUp_ShaderResources()
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

_bool CRola::IsDead()
{
	if (m_bDead && m_eState == STATE::DEAD_F)//&& m_dwDeathTime + 1000 < GetTickCount())
		return true;
	else if (m_bDead && m_eState != STATE::DEAD_F)
	{
		m_dwDeathTime = GetTickCount();
		m_eState = STATE::DEAD_F;
	}

	return false;
}

void CRola::Find_Target()
{
	if (!m_bHit && !m_bDead)
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
					m_eState = STATE::IDLE;
					m_bAggro = false;
				}
				return;
			}

			if (pTarget)
			{
				CTransform* PlayerTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));
				_vector vTargetPos = PlayerTransform->Get_State(CTransform::STATE_POSITION);
				m_fDistanceToTarget = XMVectorGetX(XMVector3Length(Get_TransformState(CTransform::STATE_POSITION) - vTargetPos));
				m_pTarget = pTarget;
			}
			else
				m_pTarget = nullptr;
		}
	}
	//else
		//m_pTarget = nullptr;
}

void CRola::Follow_Target(_float fTimeDelta)
{
	if (m_pTarget == nullptr)
		return;

	m_eState = STATE::STANCE_WALK;
	_vector vTargetPos = dynamic_cast<CBaseObj*>(m_pTarget)->Get_TransformState(CTransform::STATE_POSITION);
	m_pTransformCom->LookAt(vTargetPos);
	m_pTransformCom->Go_Straight(fTimeDelta*1.5f);
	m_bIsAttacking = true;
}

void CRola::AI_Behaviour(_float fTimeDelta)
{
	if (!m_bMove || m_eState == DEAD_F || m_eState == STAGGER || m_eState == DEAD_B || m_bHit )
		return;

	// Check for Target, AggroRadius
	Find_Target();
	if (m_bAggro && m_fDistanceToTarget < m_fPatrolRadius)
	{

		if (m_pTarget)
		{
			// If in AttackRadius > Attack
			if (m_fDistanceToTarget < m_fAttackRadius)
			{
				m_pTransformCom->LookAt(dynamic_cast<CBaseObj*>(m_pTarget)->Get_TransformState(CTransform::STATE_POSITION));
				if (!m_bIsAttacking && GetTickCount() > m_dwAttackTime + 1500)
				{
					m_eState = STATE::STANCE_WALK;
					m_dwAttackTime = GetTickCount();
					m_bIsAttacking = true;
				}
				else if (!m_bIsAttacking)
					m_eState = STATE::IDLE;
			}
			else
				Follow_Target(fTimeDelta);
				
		}
	}
	else
		Patrol(fTimeDelta);
}

void CRola::Patrol(_float fTimeDelta)
{
	// Switch between Idle and Walk (based on time)
	m_bAggro = false;

	if (m_eState == STATE::IDLE || m_eState == STATE::KYOROKYORO)
	{
		if (GetTickCount() > m_dwIdleTime + (rand() % 1500) * (rand() % 2 + 1) + 3000)
		{
			m_eState = STATE::WALK;
			m_dwWalkTime = GetTickCount();

			m_eDir[DIR_X] = rand() % 3 - 1;
			m_eDir[DIR_Z] = m_eDir[DIR_X] == 0 ? rand() % 3 - 1 : 0;

		}
	}
	else if (m_eState == STATE::WALK)
	{
		if (GetTickCount() > m_dwWalkTime + (rand() % 3000) * (rand() % 2 + 1) + 1500)
		{
			m_eState = rand()%2 == 0 ?  STATE::IDLE : STATE::KYOROKYORO;
			m_dwIdleTime = GetTickCount();
		}
	}

	// Movement
	if (m_eState == STATE::WALK)
	{
		Change_Direction();
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
}

_uint CRola::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	_uint iHp = __super::Take_Damage(fDamage, DamageType, DamageCauser);

	if (iHp > 0)
	{
		if (!m_bDead)
		{
			m_bHit = true;
			if (Calculate_Direction() == FRONT)
				m_eState = STATE::DAMAGE_F;
			else
				m_eState = STATE::DAMAGE_B;
			m_bMove = true;
		}


		m_bAggro = true;
		m_bIsAttacking = false;
		m_dwAttackTime = GetTickCount();

		return iHp;
	}
	else
	{
		if (Calculate_Direction() == FRONT)
			m_eState = STATE::DEAD_F;
		else
			m_eState = STATE::DEAD_B;
	}
		

	return 0;
}

CRola * CRola::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRola*	pInstance = new CRola(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CRola"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRola::Clone(void * pArg)
{
	CRola*	pInstance = new CRola(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CRola"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRola::Free()
{
	__super::Free();
}


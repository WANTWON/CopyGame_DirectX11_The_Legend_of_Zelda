#include "stdafx.h"
#include "..\Public\Octorock.h"
#include "Player.h"

COctorock::COctorock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonster(pDevice, pContext)
{
}

HRESULT COctorock::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT COctorock::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_tInfo.iMaxHp = 3;
	m_tInfo.iDamage = 20.f;
	m_tInfo.iCurrentHp = m_tInfo.iMaxHp;

	m_fAttackRadius = 5.f;
	m_eMonsterID = MONSTER_OCTOROCK;

	_vector vecPostion = XMLoadFloat3((_float3*)pArg);
	vecPostion = XMVectorSetW(vecPostion, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vecPostion);
	return S_OK;
}

int COctorock::Tick(_float fTimeDelta)
{
	if (__super::Tick(fTimeDelta))
		return OBJ_DEAD;

	if (m_fDistanceToTarget < 2.f)
	{
		if (CGameInstance::Get_Instance()->Key_Down(DIK_X))
			Take_Damage(1, nullptr, nullptr);
	}

	


	AI_Behaviour(fTimeDelta);
	m_pModelCom->Set_CurrentAnimIndex(m_eState);
	Change_Animation(fTimeDelta);
	return OBJ_NOEVENT;
}

void COctorock::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT COctorock::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void COctorock::Change_Animation(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::COctorock::IDLE:
	case Client::COctorock::WALK:
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop);
		break;
	case Client::COctorock::ATTACK_ST:
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop))
		{
			m_eState = ATTACK_ED;
			//make bullet
		}
		break;
	case Client::COctorock::ATTACK_ED:
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop))
		{
			m_bIsAttacking = false;
			m_eState = IDLE;
			m_bHit = false;
		}
		break;
	case Client::COctorock::DAMAGE:
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop))
		{
			m_eState = IDLE;
			m_bHit = false;
		}
		break;
	case Client::COctorock::DEAD:
	case Client::COctorock::DEAD_FIRE:
		m_bIsLoop = false;
		m_pTransformCom->Go_Backward(fTimeDelta*4);
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

HRESULT COctorock::Ready_Components(void * pArg)
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(1.0f);
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Octorock"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT COctorock::SetUp_ShaderResources()
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

_bool COctorock::IsDead()
{
	if (m_bDead && m_eState == STATE::DEAD )//&& m_dwDeathTime + 1000 < GetTickCount())
		return true;
	else if (m_bDead && m_eState != STATE::DEAD)
	{
		m_dwDeathTime = GetTickCount();
		m_eState = STATE::DEAD;
	}

	return false;
}

void COctorock::Find_Target()
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
					m_eState = STATE::IDLE;
					m_bAggro = false;
				}
				return;
			}

			if (pTarget)
			{
				CTransform* PlayerTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_STATIC,TEXT("Layer_Player"), TEXT("Com_Transform"));
				_vector vTargetPos = PlayerTransform->Get_State(CTransform::STATE_POSITION);
				m_fDistanceToTarget =  XMVectorGetX(XMVector3Length(Get_TransformState(CTransform::STATE_POSITION) - vTargetPos));
				m_pTarget = pTarget;
			}
			else
				m_pTarget = nullptr;
		}
	}
	else
		m_pTarget = nullptr;
}

void COctorock::Follow_Target(_float fTimeDelta)
{
	if (m_pTarget == nullptr)
		return;

	m_eState = STATE::WALK;

	_vector vTargetPos = dynamic_cast<CBaseObj*>(m_pTarget)->Get_TransformState(CTransform::STATE_POSITION);

	m_pTransformCom->LookAt(vTargetPos);
	m_pTransformCom->Go_Straight(fTimeDelta);

	m_bIsAttacking = false;
}

void COctorock::AI_Behaviour(_float fTimeDelta)
{
	if (!m_bMove && m_eState == DEAD)
		return;

	// Check for Target, AggroRadius
	Find_Target();
	if (m_bAggro)
	{
		
		if (m_pTarget)
		{
			// If in AttackRadius > Attack
			if (m_fDistanceToTarget < m_fAttackRadius)
			{
				m_pTransformCom->LookAt(dynamic_cast<CBaseObj*>(m_pTarget)->Get_TransformState(CTransform::STATE_POSITION));
				if (!m_bIsAttacking && GetTickCount() > m_dwAttackTime + 1500)
				{
					m_eState = STATE::ATTACK_ST;
					m_bIsAttacking = true;
				}
				else if (!m_bIsAttacking)
					m_eState = STATE::IDLE;
			}
			// If NOT in AttackRadius > Follow Target
			else
				Follow_Target(fTimeDelta);
		}
	}
	else
		Patrol(fTimeDelta);
}

void COctorock::Patrol(_float fTimeDelta)
{
	// Switch between Idle and Walk (based on time)
	if (m_eState == STATE::IDLE)
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
			m_eState = STATE::IDLE;
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

_float COctorock::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	_float fHp = __super::Take_Damage(fDamage, DamageType, DamageCauser);

	if (fHp > 0)
	{
		if (!m_bDead)
		{
			m_bHit = true;
			m_eState = STATE::DAMAGE;
			m_bMove = true;
		}


		m_bAggro = true;
		m_bIsAttacking = false;
		m_dwAttackTime = GetTickCount();

		return fHp;
	}
	else
		m_eState = STATE::DEAD;

	return 0.f;
}

COctorock * COctorock::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	COctorock*	pInstance = new COctorock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : COctorock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * COctorock::Clone(void * pArg)
{
	COctorock*	pInstance = new COctorock(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : COctorock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COctorock::Free()
{
	__super::Free();
}


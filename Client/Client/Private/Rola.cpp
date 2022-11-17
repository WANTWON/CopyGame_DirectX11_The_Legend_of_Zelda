#include "stdafx.h"
#include "..\Public\Rola.h"
#include "Player.h"
#include "CameraManager.h"
#include "MonsterBullet.h"


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

	m_tInfo.iMaxHp = 10;
	m_tInfo.iDamage = 20;
	m_tInfo.iCurrentHp = m_tInfo.iMaxHp;

	m_fAttackRadius = 1.f;
	m_fPatrolRadius = 7.f;
	m_eMonsterID = MONSTER_ROLA;

	_vector vecPostion = XMLoadFloat3((_float3*)pArg);
	vecPostion = XMVectorSetW(vecPostion, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vecPostion);
	Set_Scale(_float3(1.2f, 1.2f, 1.2f));
	m_pNavigationCom->Compute_CurrentIndex_byDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION));


	return S_OK;
}

int CRola::Tick(_float fTimeDelta)
{
	if (__super::Tick(fTimeDelta))
		return OBJ_DEAD;

	

	AI_Behaviour(fTimeDelta);
	Check_Navigation(fTimeDelta);
	if (m_eState != m_ePreState)
	{
		m_pModelCom->Set_NextAnimIndex(m_eState);
		m_ePreState = m_eState;
	}
	Change_Animation(fTimeDelta);

	m_pOBBCom->Update(m_pTransformCom->Get_WorldMatrix());
	return OBJ_NOEVENT;
}

void CRola::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	if (m_pTarget != nullptr && true == m_pOBBCom->Collision(m_pTarget->Get_Collider()) && m_eState == JUMP_ED)
	{
		CPlayer::ANIM ePlayerState = dynamic_cast<CPlayer*>(m_pTarget)->Get_AnimState();
		if (ePlayerState == CPlayer::SHIELD_LP || ePlayerState == CPlayer::SHIELD_ST)
		{
			Take_Damage(1.f, nullptr, nullptr);
			dynamic_cast<CPlayer*>(m_pTarget)->Set_AnimState(CPlayer::SHIELD_HIT);
			m_bIsAttacking = false;
		}
		else
		{
			m_dwAttackTime = GetTickCount();
			m_dwIdleTime = GetTickCount();
			m_bIsAttacking = false;

			CMonsterBullet::BULLETDESC BulletDesc;
			BulletDesc.eOwner = MONSTER_ROLA;
			BulletDesc.eBulletType = CMonsterBullet::DEFAULT;
			BulletDesc.vInitPositon = Get_TransformState(CTransform::STATE_POSITION);
			BulletDesc.vLook = Get_TransformState(CTransform::STATE_LOOK);
			dynamic_cast<CPlayer*>(m_pTarget)->Take_Damage(1.f, &BulletDesc, nullptr);
			m_bBackStep = true;

		}
	}
}

HRESULT CRola::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CRola::Change_Animation(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CRola::IDLE:
		m_fAnimSpeed = 2.f;
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta * m_fAnimSpeed, m_bIsLoop);
		break;
	case Client::CRola::PUSH:
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop))
		{
			m_eAttackDir = m_eAttackDir == RIGHT ? LEFT : RIGHT;
			m_eState = IDLE;
			m_bIsAttacking = false;
			m_bHit = false;


			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			CMonsterBullet::BULLETDESC BulletDesc;
			BulletDesc.eOwner = MONSTER_ROLA;
			BulletDesc.eBulletType = CMonsterBullet::ROLA;
			BulletDesc.vInitPositon = Get_TransformState(CTransform::STATE_POSITION);
			BulletDesc.vLook = Get_TransformState(CTransform::STATE_LOOK);
			BulletDesc.vLook = XMVectorSetY(BulletDesc.vLook, 0.f);

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MonsterBullet"), LEVEL_TAILCAVE, TEXT("Layer_Bullet"), &BulletDesc)))
				return;
			RELEASE_INSTANCE(CGameInstance);
		}
		break;		break;
	case Client::CRola::DAMAGE:
		m_bIsLoop = false;
		_vector vDir = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTarget->Get_TransformState(CTransform::STATE_POSITION);
		m_pTransformCom->Go_PosDir(fTimeDelta, vDir);
		if (m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop))
		{
			m_eState = JUMP_ST;
			m_bIsAttacking = false;
			m_bHit = false;
		}
		break;		break;
	case Client::CRola::DEAD:
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop))
			m_bDead = true;
		break;
	case Client::CRola::DEAD_ST:
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop))
			m_eState = DEAD;
		break;
	case Client::CRola::JUMP_ST:
		m_fAnimSpeed = 3.f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop))
			m_eState = JUMP;
		break;
	case Client::CRola::JUMP:
		m_fAnimSpeed = 2.0f;
		m_bIsLoop = false;
		// Movement
		if (m_iDmgCount % 4 != 3 && m_fDistanceToTarget > m_fAttackRadius)
			Follow_Target(fTimeDelta);

		if (m_pModelCom->Play_Animation(fTimeDelta*m_fAnimSpeed, m_bIsLoop))
		{
			m_eState = JUMP_ED;
		}
		break;
	case Client::CRola::JUMP_ED:
	{
		CCamera_Dynamic* pCamera = dynamic_cast<CCamera_Dynamic*>(CCameraManager::Get_Instance()->Get_CurrentCamera());
		pCamera->Set_CamMode(CCamera_Dynamic::CAM_SHAKING, 0.1f, 0.1f, 0.01f);
		m_fAnimSpeed = 3.f;
		m_bIsLoop = false;
		if (m_pModelCom->Play_Animation(fTimeDelta* m_fAnimSpeed, m_bIsLoop))
		{
			//if (m_bJump)
				m_eState = JUMP_ST;
			//else
			//	m_eState = IDLE;
		}
		break;
	}
	default:
		m_bIsLoop = true;
		m_pModelCom->Play_Animation(fTimeDelta, m_bIsLoop);
		break;
	}

	m_fAnimSpeed = 1.f;
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
	TransformDesc.fRotationPerSec = XMConvertToRadians(3.f);
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model*/
	if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_Rola"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC		ColliderDesc;


	/* For.Com_OBB*/
	ColliderDesc.vScale = _float3(1.f, 1.0f, 1.0f);
	ColliderDesc.vRotation = _float3(0.f, XMConvertToRadians(0.0f), 0.f);
	ColliderDesc.vPosition = _float3(0.f, 0.7f, 0.f);
	if (FAILED(__super::Add_Components(TEXT("Com_OBB"), LEVEL_TAILCAVE, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof NaviDesc);
	NaviDesc.iCurrentCellIndex = 0;
	if (FAILED(__super::Add_Components(TEXT("Com_Navigation_TailCave"), LEVEL_STATIC, TEXT("Prototype_Component_Navigation_TailCave"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
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
	if (m_bDead && m_eState == STATE::DEAD)//&& m_dwDeathTime + 1000 < GetTickCount())
		return true;
	else if (m_bDead && m_eState != STATE::DEAD && m_eState != STATE::DEAD_ST)
	{
		m_dwDeathTime = GetTickCount();
		m_eState = STATE::DEAD_ST;
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
				m_pTarget = dynamic_cast<CBaseObj*>(pTarget);
			}
			else
				m_pTarget = nullptr;
		}
	}
}

void CRola::Follow_Target(_float fTimeDelta)
{
	if (m_pTarget == nullptr)
		return;

	_vector vTargetPos =m_pTarget->Get_TransformState(CTransform::STATE_POSITION);
	m_pTransformCom->LookAt(vTargetPos);
	m_pTransformCom->Go_Straight(fTimeDelta*1.5f, m_pNavigationCom);
}

void CRola::AI_Behaviour(_float fTimeDelta)
{
	if (!m_bMove || m_eState == DEAD || m_eState == DEAD_ST || m_bHit || m_bIsAttacking)
		return;

	// Check for Target, AggroRadius
	Find_Target();
	if (m_fDistanceToTarget > 10.f)
	{
		m_eState = IDLE;
		return;
	}
		



	if (m_bBackStep)
	{
		if (m_fDistanceToTarget > 4.f)
			m_bBackStep = false;

		m_pTransformCom->LookAt(m_pTarget->Get_TransformState(CTransform::STATE_POSITION));
		m_pTransformCom->Go_Backward(fTimeDelta*3, m_pNavigationCom);
	}

	if (m_iDmgCount % 4 == 3 && m_fDistanceToTarget < m_fPatrolRadius)
	{
		Moving_AttackPosition(fTimeDelta);

		if (m_bSpecialAttack == true)
		{
			if (!m_bIsAttacking && GetTickCount() > m_dwAttackTime + 2000)
			{
				m_eState = STATE::PUSH;
				m_dwAttackTime = GetTickCount();
				m_bIsAttacking = true;
				m_iDmgCount++;
				m_bSpecialAttack = false;
			}
			else if (!m_bIsAttacking)
				m_eState = STATE::JUMP_ST;
		}	
	}
	else
		Patrol(fTimeDelta);
}

void CRola::Patrol(_float fTimeDelta)
{
	m_bAggro = false;
	if (m_fDistanceToTarget > m_fPatrolRadius)
		return;

	// Switch between Idle and Walk (based on time)

	if (m_eState == STATE::IDLE)
	{
		m_bJump = true;
		m_eState = STATE::JUMP_ST;	
	}

}

_bool CRola::Moving_AttackPosition(_float fTimeDelta)
{
	if (m_bSpecialAttack == true)
		return true;

	if (m_eAttackDir == RIGHT)
	{
		_float Distance = XMVectorGetX(XMVector3Length(Get_TransformState(CTransform::STATE_POSITION) - XMLoadFloat4(&m_fRAttackPos)));
		if (Distance < 0.5f)
		{
			m_pTransformCom->LookAt(XMLoadFloat4(&m_fLAttackPos));
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_fRAttackPos));
			m_bSpecialAttack = true;
			m_bJump = false;
			
		}
		else
		{
			_vector		vLook = XMLoadFloat4(&m_fLAttackPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector		vMyOriginalLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

			vMyOriginalLook = XMVectorSetY(vMyOriginalLook, XMVectorGetY(vLook));
			_vector vLookDot = XMVector3AngleBetweenVectors(XMVector3Normalize(vLook), XMVector3Normalize(vMyOriginalLook));
			_float fAngle = XMConvertToDegrees(XMVectorGetX(vLookDot));

			_vector vCross = XMVector3Cross(XMVector3Normalize(vLook), XMVector3Normalize(vMyOriginalLook));
			_vector vDot = XMVector3Dot(XMVector3Normalize(vCross), XMVector3Normalize(XMVectorSet(0.f, 1.f, 0.f, 0.f)));

			_float fScala = XMVectorGetX(vDot);

			if (fScala < 0.f)
				fAngle = 360.f - fAngle;

			if (fAngle > 5)
				m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), -1.f);
			m_pTransformCom->Go_PosTarget(fTimeDelta, XMLoadFloat4(&m_fRAttackPos));

		}
	}
	else
	{
		_float Distance = XMVectorGetX(XMVector3Length(Get_TransformState(CTransform::STATE_POSITION) - XMLoadFloat4(&m_fLAttackPos)));
		if (Distance < 0.5f)
		{
			
			m_pTransformCom->LookAt(XMLoadFloat4(&m_fRAttackPos));
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_fLAttackPos));
			m_bSpecialAttack = true;
			m_bJump = false;
		}
		else
		{
			_vector		vLook = XMLoadFloat4(&m_fRAttackPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector		vMyOriginalLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

			vMyOriginalLook = XMVectorSetY(vMyOriginalLook, XMVectorGetY(vLook));
			_vector vLookDot = XMVector3AngleBetweenVectors(XMVector3Normalize(vLook), XMVector3Normalize(vMyOriginalLook));
			_float fAngle = XMConvertToDegrees(XMVectorGetX(vLookDot));

			_vector vCross = XMVector3Cross(XMVector3Normalize(vLook), XMVector3Normalize(vMyOriginalLook));
			_vector vDot = XMVector3Dot(XMVector3Normalize(vCross), XMVector3Normalize(XMVectorSet(0.f, 1.f, 0.f, 0.f)));

			_float fScala = XMVectorGetX(vDot);

			if (fScala < 0.f)
				fAngle = 360.f - fAngle;

			if ( fAngle > 5)
				m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), 1.f);
			m_pTransformCom->Go_PosTarget(fTimeDelta, XMLoadFloat4(&m_fLAttackPos));
		}
	}
	return false;
}

void CRola::Check_Navigation(_float fTimeDelta)
{
		if (m_pNavigationCom->Get_CurrentCelltype() == CCell::DROP)
			m_eState = IDLE;
		else if (m_pNavigationCom->Get_CurrentCelltype() == CCell::ACCESSIBLE)
		{
			_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float fHeight = m_pNavigationCom->Compute_Height(vPosition, 0.f);
			if (fHeight > XMVectorGetY(vPosition))
			{
				vPosition = XMVectorSetY(vPosition, fHeight);
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
			}
		}
}

_uint CRola::Take_Damage(float fDamage, void * DamageType, CBaseObj * DamageCauser)
{
	if (m_eState == DAMAGE)
		return 0;

	_uint iHp = __super::Take_Damage(fDamage, DamageType, DamageCauser);

	if (iHp > 0)
	{
		if (!m_bDead)
		{
			m_bHit = true;
			m_iDmgCount++;
			m_eState = STATE::DAMAGE;
			m_bMove = true;
		}


		m_bAggro = true;
		m_bIsAttacking = false;
		m_dwAttackTime = GetTickCount();

		return iHp;
	}
	else
		m_eState = STATE::DEAD_ST;

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


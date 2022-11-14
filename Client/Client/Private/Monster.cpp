#include "stdafx.h"
#include "..\Public\Monster.h"
#include "Player.h"
#include "Level_Manager.h"
#include "CameraManager.h"


CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBaseObj(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster & rhs)
	: CBaseObj(rhs)
{
}

HRESULT CMonster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_eObjectID = OBJID::OBJ_MONSTER;
	//Set_Scale(_float3(0.5, 0.5, 0.5));

	CCollision_Manager::Get_Instance()->Add_CollisionGroup(CCollision_Manager::COLLISION_MONSTER, this);
	return S_OK;
}

int CMonster::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (IsDead())
	{
		CCollision_Manager::Get_Instance()->Out_CollisionGroup(CCollision_Manager::COLLISION_MONSTER, this);
		return OBJ_DEAD;
	}
		

	return OBJ_NOEVENT;
}

void CMonster::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	SetUp_ShaderID();
}

HRESULT CMonster::Render()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pModelCom)
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

	return S_OK;
}


CMonster::DMG_DIRECTION CMonster::Calculate_Direction()
{
	if (m_pTarget == nullptr)
		return FRONT;

	// New Logic
	_vector vLook = XMVector3Normalize( m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_vector vTargetDir = XMVector3Normalize(dynamic_cast<CPlayer*>(m_pTarget)->Get_TransformState(CTransform::STATE_LOOK));

	_vector fDot = XMVector3Dot(vTargetDir, vLook);
	_float fAngleRadian = acos(XMVectorGetX(fDot));
	_float fAngleDegree = XMConvertToDegrees(fAngleRadian);
	_vector vCross = XMVector3Cross(vTargetDir, vLook);


	if (XMVectorGetY(vCross) > 0.f)
	{
		if (fAngleDegree > 0.f && fAngleDegree <= 90.f)
			m_eDmg_Direction = BACK;
		else if (fAngleDegree > 90.f && fAngleDegree <= 180.f)
			m_eDmg_Direction = FRONT;
	}
	else
	{
		if (fAngleDegree > 0.f && fAngleDegree <= 90.f)
			m_eDmg_Direction = BACK;
		else if (fAngleDegree > 90.f && fAngleDegree <= 180.f)
			m_eDmg_Direction = FRONT;
	}

	return m_eDmg_Direction;
}

_vector CMonster::Calculate_PosDirction()
{
	if (m_pTarget == nullptr)
		return _vector();

	// New Logic
	_vector vMyPos = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	_vector vTargetPos = XMVector3Normalize(dynamic_cast<CPlayer*>(m_pTarget)->Get_TransformState(CTransform::STATE_POSITION));

	return vTargetPos - vMyPos;
}

void CMonster::Find_Target()
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
				
				m_pTarget = nullptr;
				m_bAggro = false;
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

_uint CMonster::Take_Damage(float fDamage, void * DamageType, CBaseObj * DamageCauser)
{
	if (fDamage <= 0 || m_bDead)
		return 0;

	m_tInfo.iCurrentHp -= (int)fDamage;

	if (m_tInfo.iCurrentHp <= 0)
	{
		m_tInfo.iCurrentHp = 0;
		return m_tInfo.iCurrentHp;
	}

	return m_tInfo.iCurrentHp;
}

void CMonster::Check_Navigation(_float fTimeDelta)
{
	if (m_pNavigationCom == nullptr)
		return;
}


void CMonster::Free()
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
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


	return S_OK;
}

int CMonster::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (IsDead())
		return OBJ_DEAD;

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


void CMonster::Calculate_Direction(_vector vTargetPos)
{
	// New Logic
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vLook = XMVectorSetY(vLook, 0.f);
	_vector vTargetDir = vTargetPos - Get_Position();
	vLook = XMVector3Normalize(vLook);
	vTargetDir = XMVector3Normalize(vTargetDir);

	_vector fDot = XMVector3Dot(vTargetDir, vLook);
	_float fAngleRadian = acos(XMVectorGetX(fDot));
	_float fAngleDegree = XMConvertToDegrees(fAngleRadian);

	if (fAngleDegree < 0 || fAngleDegree > 360 )
		return;

	if (fAngleDegree > 180 )
	{
		fAngleDegree = 360 - fAngleDegree;
	}

	_vector vCross = XMVector3Cross(vTargetDir, vLook);

	m_pTransformCom->LookAt(vTargetPos);
}

_float CMonster::Take_Damage(float fDamage, void * DamageType, CGameObject * DamageCauser)
{
	if (fDamage <= 0 || m_bDead)
		return 0.f;

	m_tInfo.iCurrentHp -= fDamage;

	if (m_tInfo.iCurrentHp <= 0)
	{
		m_tInfo.iCurrentHp = 0.f;
		return m_tInfo.iCurrentHp;
	}

	return m_tInfo.iCurrentHp;
}


void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
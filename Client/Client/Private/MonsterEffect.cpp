#include "stdafx.h"
#include "..\Public\MonsterEffect.h"
#include "Weapon.h"
#include "Player.h"
#include "GameInstance.h"


CMonsterEffect::CMonsterEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CMonsterEffect::CMonsterEffect(const CMonsterEffect & rhs)
	: CEffect(rhs)
{
}

HRESULT CMonsterEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterEffect::Initialize(void * pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Set_Scale(m_EffectDesc.vInitScale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vInitPositon);
	m_pTransformCom->LookDir(m_EffectDesc.vLook);
	m_vScale = m_EffectDesc.vInitScale;


	switch (m_EffectDesc.eEffectID)
	{
	case BULLET_SMOKE:
		break;
	default:
		break;
	}


	return S_OK;
}

int CMonsterEffect::Tick(_float fTimeDelta)
{
	if (m_bDead)
	{
		return OBJ_DEAD;
	}

	__super::Tick(fTimeDelta);


	switch (m_EffectDesc.eEffectID)
	{
	case BULLET_SMOKE:
		break;
	default:
		break;
	}

	return OBJ_NOEVENT;
}

void CMonsterEffect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

HRESULT CMonsterEffect::Render()
{

	__super::Render();


	return S_OK;
}

HRESULT CMonsterEffect::Ready_Components(void * pArg)
{
	LEVEL iLevel = (LEVEL)CGameInstance::Get_Instance()->Get_DestinationLevelIndex();

	__super::Ready_Components();

	switch (m_EffectDesc.eEffectID)
	{
	default:
		break;
	}


	return S_OK;
}



HRESULT CMonsterEffect::SetUp_ShaderID()
{
	return S_OK;
}

void CMonsterEffect::Change_Animation(_float fTimeDelta)
{
}

void CMonsterEffect::Change_Texture(_float fTimeDelta)
{
	
}


void CMonsterEffect::Tick_Grass(_float fTimeDelta)
{
	m_fDeadtime += fTimeDelta;


	if (m_fAlpha <= 0)
		m_bDead = true;

}

void CMonsterEffect::Tick_Smoke(_float fTimeDelta)
{

	SetUp_BillBoard();

	m_fAlpha -= 0.01f;
	m_vScale.x -= 0.02f;
	m_vScale.y -= 0.02f;
	m_vScale.z -= 0.02f;

	Set_Scale(m_vScale);

	if (m_vScale.x <= 0 || m_fAlpha <= 0)
		m_bDead = true;
}



CMonsterEffect * CMonsterEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonsterEffect*	pInstance = new CMonsterEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMonsterEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMonsterEffect::Clone(void * pArg)
{
	CMonsterEffect*	pInstance = new CMonsterEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMonsterEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonsterEffect::Free()
{
	__super::Free();
}

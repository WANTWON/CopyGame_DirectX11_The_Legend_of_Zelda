#include "stdafx.h"
#include "..\Public\PlayerEffect.h"
#include "Weapon.h"
#include "Player.h"
#include "GameInstance.h"

CPlayerEffect::CPlayerEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CPlayerEffect::CPlayerEffect(const CPlayerEffect & rhs)
	: CEffect(rhs)
{
}

HRESULT CPlayerEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerEffect::Initialize(void * pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Set_Scale(m_EffectDesc.vInitScale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vInitPositon);
	m_pTransformCom->LookDir(m_EffectDesc.vLook);
	m_vScale = m_EffectDesc.vInitScale;

	switch (m_EffectDesc.eEffectID)
	{
	case ROLLCUT:
		m_eShaderID = SHADERM_ROLLCUT;
		m_fTexUV = 1.f;
		break;
	default:
		break;
	}

	
	return S_OK;
}

int CPlayerEffect::Tick(_float fTimeDelta)
{
	if (m_bDead)
	{
		return OBJ_DEAD;
	}
		
	__super::Tick(fTimeDelta);

	switch (m_EffectDesc.eEffectID)
	{
	case ROLLCUT:
		Tick_RollCut(fTimeDelta);
		break;
	default:
		break;
	}

	return OBJ_NOEVENT;
}

void CPlayerEffect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CPlayerEffect::Render()
{

	__super::Render();
		

	return S_OK;
}

HRESULT CPlayerEffect::Ready_Components(void * pArg)
{
	LEVEL iLevel = (LEVEL)CGameInstance::Get_Instance()->Get_DestinationLevelIndex();

	__super::Ready_Components();

	switch (m_EffectDesc.eEffectID)
	{
	case ROLLCUT:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_RollCut_Blast"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	default:
		break;
	}
	

	return S_OK;
}


HRESULT CPlayerEffect::SetUp_ShaderID()
{
	return S_OK;
}

void CPlayerEffect::Change_Animation(_float fTimeDelta)
{
}

void CPlayerEffect::Change_Texture(_float fTimeDelta)
{
	
}



void CPlayerEffect::Tick_RollCut(_float fTimeDelta)
{
	m_fDeadtime += fTimeDelta;
	m_fTexUV -= 0.07f;
	if (m_fTexUV <= 0.f)
		m_fTexUV = 0.f;

	if (m_EffectDesc.fDeadTime < m_fDeadtime)
		m_bDead = true;
}

CPlayerEffect * CPlayerEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayerEffect*	pInstance = new CPlayerEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPlayerEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayerEffect::Clone(void * pArg)
{
	CPlayerEffect*	pInstance = new CPlayerEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPlayerEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerEffect::Free()
{
	__super::Free();
}

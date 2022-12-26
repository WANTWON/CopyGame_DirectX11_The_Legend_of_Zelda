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
		m_fAlpha = 1.f;
		break;
	case SLASH:
		Set_Scale(_float3(3.f, 3.f, 3.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vInitPositon);
		m_pTransformCom->LookDir(m_EffectDesc.vLook);
		m_eShaderID = SHADERM_SLASH;
		m_fTexUV = 1.f;
		m_fAlpha = 1.f;
		break;
	default:
		break;
	}

	if (dynamic_cast<CPlayer*>(m_EffectDesc.pTarget)->Get_2DMode())
	{
		
		if (XMVectorGetX(m_EffectDesc.vLook) == 1.f)
		{
			m_pTransformCom->Rotation(XMVectorSet(-1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);
			m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), -20.f);
		}	
		else
		{
			m_pTransformCom->Rotation(XMVectorSet(-1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
			m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), 20.f);
		}
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
	case SLASH:
		Tick_Slash(fTimeDelta);
		break;
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
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_SwordSwish0"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case SLASH:
	{
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_RollCut"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}
	default:
		break;
	}

	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Glow"), (CComponent**)&m_pGlowTexture)))
		return E_FAIL;

	

	return S_OK;
}


HRESULT CPlayerEffect::SetUp_ShaderID()
{
	return S_OK;
}

HRESULT CPlayerEffect::SetUp_ShaderResources()
{
	__super::SetUp_ShaderResources();


	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_GlowTexture", m_pGlowTexture->Get_SRV(0))))
		return E_FAIL;

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

	if (m_EffectDesc.fDeadTime*0.2f > m_fDeadtime)
	{
		m_fAlpha = 0.f;
	}
	else if (m_EffectDesc.fDeadTime*0.2f < m_fDeadtime && m_EffectDesc.fDeadTime > m_fDeadtime)
	{
		m_fAlpha += 0.1f;
		if (m_fAlpha >= 1)
			m_fAlpha = 1.f;
	}
	else if (m_EffectDesc.fDeadTime < m_fDeadtime)
	{
		m_fAlpha -= 0.1f;
		if(m_fAlpha <= 0)
			m_bDead = true;
	}
		

}

void CPlayerEffect::Tick_Slash(_float fTimeDelta)
{
	m_fDeadtime += fTimeDelta;
	m_fTexUV -= 0.07f;
	if (m_fTexUV <= 0.f)
		m_fTexUV = 0.f;

	if (m_EffectDesc.fDeadTime < m_fDeadtime)
	{
		m_fAlpha -= 0.1f;
		if (m_fAlpha <= 0)
			m_bDead = true;
	}

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

	Safe_Release(m_pGlowTexture);
}

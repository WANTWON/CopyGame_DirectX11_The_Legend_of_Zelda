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
		m_eShaderID = SHADER_ONECOLOR_ALPHASET;
		m_vColorFront = m_EffectDesc.vColor;
		break;
	case BLAST_RING:
		m_eShaderID = SHADERM_TWOCOLOR_DEFAULT;
		m_vColorFront = XMVectorSet(240, 233, 223, 255);
		m_vColorBack = XMVectorSet(165, 124, 94, 255);
		break;
	case OCTOROCK_STONE:
		m_eShaderID = SHADERM_ONLY_TEXTURE;
		m_vDirection = _float3((rand() % 20 - 10) * 0.1f, (rand() % 20 - 10)*0.f, (rand() % 20 - 10)* 0.1f);
		break;
	case CLAW_SMOKE:
		m_eShaderID = SHADER_ONECOLOR_ALPHASET;
		m_vColorFront = m_EffectDesc.vColor;
		m_fAlpha = 0.f;
		break;
	case FLAPPING_SMOKE:
		m_eShaderID = SHADERM_TWOCOLOR_DEFAULT;
		m_vColorFront = XMVectorSet(255, 255, 255, 255);
		m_vColorBack = XMVectorSet(138, 136, 135, 255);
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
	case BLAST_RING:
	case OCTOROCK_STONE:
		Tick_OctoRockEffect(fTimeDelta);
		break;
	case CLAW_SMOKE:
	case FLAPPING_SMOKE:
		Tick_AlbatossEffect(fTimeDelta);
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
	case BULLET_SMOKE:
		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_SmokeDeath"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
		break;
	case BLAST_RING:
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_Octorock_Ring"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case OCTOROCK_STONE:
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_Octorock_Stone"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case CLAW_SMOKE:
		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Smoke"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
		break;
	case FLAPPING_SMOKE:
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_SmokeSphere"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
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
	switch (m_EffectDesc.eEffectID)
	{
	case CLAW_SMOKE:
		m_EffectDesc.iTextureNum++;

		if (m_EffectDesc.iTextureNum >= m_pTextureCom->Get_TextureSize())
		{
			m_EffectDesc.iTextureNum  = 0;
		}
		break;
	}
}


void CMonsterEffect::Tick_Grass(_float fTimeDelta)
{
	m_fDeadtime += fTimeDelta;


	if (m_fAlpha <= 0)
		m_bDead = true;

}

void CMonsterEffect::Tick_OctoRockEffect(_float fTimeDelta)
{
	m_fDeadtime += fTimeDelta;

	switch (m_EffectDesc.eEffectID)
	{
	case BULLET_SMOKE:
		SetUp_BillBoard();

		m_fAlpha -= 0.03f;
		m_vScale.x += 0.01f;
		m_vScale.y += 0.01f;
		m_vScale.z += 0.01f;
		break;
	case BLAST_RING:

		m_fAlpha -= 0.02f;
		m_vScale.x += 0.1f;
		m_vScale.y += 0.1f;
		m_vScale.z += 0.1f;
		break;
	case OCTOROCK_STONE:
		SetUp_BillBoard();
		m_pTransformCom->Go_PosDir(fTimeDelta, XMLoadFloat3(&m_vDirection));
	
		if (m_fDeadtime > m_EffectDesc.fDeadTime)
		{
			m_fAlpha -= 0.1f;
			m_vScale.x -= 0.1f;
			m_vScale.y -= 0.1f;
			m_vScale.z -= 0.1f;
		}
		break;
	default:
		break;
	}

	Set_Scale(m_vScale);

	if (m_vScale.x <= 0 || m_fAlpha <= 0)
		m_bDead = true;
}

void CMonsterEffect::Tick_AlbatossEffect(_float fTimeDelta)
{

	switch (m_EffectDesc.eEffectID)
	{
	case CLAW_SMOKE:
		SetUp_BillBoard();

		if (!m_bMax)
		{
			m_fAlpha += 0.05f;

			if (m_fAlpha >= 1.f)
			{
				m_fAlpha = 1.f;
				m_bMax = true;
			}
				
		}
		else
			m_fAlpha -= 0.01f;

		m_vScale.x += 0.1f;
		m_vScale.y += 0.05f;
	
		break;
	case FLAPPING_SMOKE:
		m_pTransformCom->LookDir(m_EffectDesc.vLook);
		m_fAlpha -= 0.01f;
		m_vScale.x += 0.2f;
		m_vScale.y += 0.2f;
		m_vScale.z += 0.2f;
		break;
	default:
		break;
	}
	
	m_pTransformCom->Go_PosDir(fTimeDelta, m_EffectDesc.vLook);

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

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
	m_fScale = m_EffectDesc.vInitScale.x;

	switch (m_EffectDesc.eEffectID)
	{
	case HITRING:
		m_eShaderID = SHADER_HITRING;
		break;
	case HITSPARK:
		m_eShaderID = SHADER_HITSPARK;
		m_vDirection = _float3((rand() % 20  - 10) * 0.1f, /*(rand() % 20 -10) * 0.1f*/ 0.f, (rand() % 20 - 10 )* 0.1f);
		break;
	case HITFLASH:
		m_eShaderID = SHADER_HITFLASH;
		break;
	case HITFLASH_TEX:
		m_eShaderID = SHADER_HITFLASH_TEX2;
		m_fScale = m_EffectDesc.vInitScale.x;

		if(m_EffectDesc.iTextureNum == 1)
			m_eShaderID = SHADER_HITFLASH_TEX;

		if (m_EffectDesc.iTextureNum == 2)
			m_eShaderID = SHADER_HITFLASH_TEX3;
		break;
	case DEADSMOKE:
		m_eShaderID = SHADER_DEADSMOKE;
		break;
	case DEADGLOW:
		m_fAngle = XMConvertToRadians(float(rand() % 180));
		m_eShaderID = SHADER_DEADGLOW;
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
	case HITFLASH:
	case HITRING:
	case HITSPARK:
	case HITFLASH_TEX:
		Tick_HitFlash(fTimeDelta);
		break;
	case DEADSMOKE:
	case DEADGLOW:
		Tick_DeadEffect(fTimeDelta);
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

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.0f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(1.0f);
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	/* For.Com_Model*/
	switch (m_EffectDesc.eEffectType)
	{
	case MESH:
		/* For.Com_Shader */
		if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Model"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
		break;
	case VIBUFFER_RECT:
		/* For.Com_Shader */
		if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
		break;
	default:
		break;
	}


	switch (m_EffectDesc.eEffectID)
	{
	case HITFLASH:
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_HitFlash"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case HITRING:
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_HitRing"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case HITSPARK:
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_HitSpark"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case HITFLASH_TEX:
		/* For.Com_VIBuffer */
		if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
			return E_FAIL;

		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Flash"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
		break;
	case DEADGLOW:
		/* For.Com_VIBuffer */
		if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
			return E_FAIL;

		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Glow"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
		break;
	case DEADSMOKE:
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_SmokeSphere"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	default:
		break;
	}
	

	return S_OK;
}

HRESULT CMonsterEffect::SetUp_ShaderResources()
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

	if (m_EffectDesc.eEffectType == MESH)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_TexUV", &m_fTexUV, sizeof(_float))))
			return E_FAIL;
	}
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;
	
		

	RELEASE_INSTANCE(CGameInstance);

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


void CMonsterEffect::Tick_HitFlash(_float fTimeDelta)
{
	m_fDeadtime += fTimeDelta;

	if (m_EffectDesc.fDeadTime < m_fDeadtime)
		m_bDead = true;


	switch (m_EffectDesc.eEffectID)
	{
	case HITRING:
		SetUp_BillBoard();
		if (m_fDeadtime > m_EffectDesc.fDeadTime*0.5f)
			m_fAlpha -= 0.1f;
		m_fScale += 0.05f;
		Set_Scale(_float3(m_fScale, m_fScale, m_fScale));
		break;
	case HITSPARK:
	{
		m_pTransformCom->Go_PosDir(fTimeDelta, XMLoadFloat3(&m_vDirection));
		m_pTransformCom->LookAt(m_EffectDesc.vInitPositon);

		_float4x4 ViewMatrix;

		ViewMatrix = CGameInstance::Get_Instance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW); // Get View Matrix
		_matrix matViewInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(&ViewMatrix));      // Get Inverse of View Matrix (World Matrix of Camera)

		_vector vRight = Get_TransformState(CTransform::STATE_RIGHT);
		_vector vLook = (_vector)matViewInv.r[2];
		_vector vUp = XMVector3Cross(vLook, vRight);

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) *Get_Scale().x);
		m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * Get_Scale().y);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) *Get_Scale().z);

		
		if (m_fDeadtime > m_EffectDesc.fDeadTime*0.5f)
		{
			m_fAlpha -= 0.1f;
			m_fScale -= 0.1f;
		}
		Set_Scale(_float3(m_fScale, m_fScale, m_fScale));
		break;
		}
	case HITFLASH:
		if (m_fDeadtime > m_EffectDesc.fDeadTime*0.5f)
		{
			m_fAlpha -= 0.1f;
			m_fScale -= 0.1f;
		}
		Set_Scale(_float3(m_fScale, m_fScale, m_fScale));
		break;
	case HITFLASH_TEX:
		SetUp_BillBoard();

		if (m_eShaderID != SHADER_HITFLASH_TEX3)
		{
			if (m_fDeadtime < m_EffectDesc.fDeadTime*0.5f)
				m_fScale += 0.1f;
			else
				m_fScale -= 0.1f;
		}
		else
		{
			m_fAlpha -= 0.1f;
			m_fScale -= 0.1f;
		}

		Set_Scale(_float3(m_fScale, m_fScale, m_fScale));
		break;
	default:
		break;
	}


}

void CMonsterEffect::Tick_DeadEffect(_float fTimeDelta)
{
	m_fDeadtime += fTimeDelta;

	switch (m_EffectDesc.eEffectID)
	{
	case DEADGLOW:
		SetUp_BillBoard();

		if (m_EffectDesc.pTarget != nullptr && m_EffectDesc.pTarget->Get_Dead() == false)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.pTarget->Get_TransformState(CTransform::STATE_POSITION) + XMVectorSet(0.f, m_EffectDesc.pTarget->Get_Scale().y + 1.f, 0.f, 0.f));
		m_pTransformCom->Rotation(Get_TransformState(CTransform::STATE_LOOK), m_fAngle);

		if (m_fDeadtime < m_EffectDesc.fDeadTime*0.5f)
			m_fScale += 0.05f;
		else
			m_fScale += 0.1f;

		Set_Scale(_float3(m_fScale, m_fScale, m_fScale));
		break;
	default:
		break;
	}

	if (m_EffectDesc.fDeadTime < m_fDeadtime)
		m_fAlpha -= 0.05f;

	if (m_fAlpha <= 0.f)
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

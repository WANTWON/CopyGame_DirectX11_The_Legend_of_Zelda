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

	switch (m_EffectDesc.eEffectID)
	{
	case FOOTSMOKE:
		m_fScale = m_EffectDesc.vInitScale.x;
		m_EffectDesc.iTextureNum = rand() % 3;
		break;
	case ROLLCUT:
		m_eShaderID = ROLLCUT;
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
	case FOOTSMOKE:
		Tick_Smoke(fTimeDelta);
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
	case FOOTSMOKE:
		/* For.Com_VIBuffer */
		if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
			return E_FAIL;

		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Smoke"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
		break;
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

HRESULT CPlayerEffect::SetUp_ShaderResources()
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
	else
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
			return E_FAIL;
	}
		

	RELEASE_INSTANCE(CGameInstance);

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
	switch (m_EffectDesc.eEffectID)
	{
	case FOOTSMOKE:
		m_EffectDesc.iTextureNum++;

		if (m_EffectDesc.iTextureNum >= m_pTextureCom->Get_TextureSize())
		{
			m_bDead = true;
			m_EffectDesc.iTextureNum--;
		}
		break;
	}
}


void CPlayerEffect::Tick_Smoke(_float fTimeDelta)
{

	SetUp_BillBoard();

	m_fAlpha -= 0.01f;
	m_fScale -= 0.02f;
	Set_Scale(_float3(m_fScale, m_fScale, m_fScale));

	if (m_fScale <= 0 || m_fAlpha <= 0)
		m_bDead = true;
	
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

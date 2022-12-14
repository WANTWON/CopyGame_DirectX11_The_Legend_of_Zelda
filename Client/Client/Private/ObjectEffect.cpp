#include "stdafx.h"
#include "..\Public\ObjectEffect.h"
#include "Weapon.h"
#include "Player.h"
#include "GameInstance.h"

CObjectEffect::CObjectEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CObjectEffect::CObjectEffect(const CObjectEffect & rhs)
	: CEffect(rhs)
{
}

HRESULT CObjectEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CObjectEffect::Initialize(void * pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Set_Scale(m_EffectDesc.vInitScale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vInitPositon);
	m_pTransformCom->LookDir(m_EffectDesc.vLook);
	m_vScale = m_EffectDesc.vInitScale;

	switch (m_EffectDesc.eEffectID)
	{
	case GRASS:
	case LAWN:
		m_pTransformCom->Rotation(XMVectorSet((rand()%10 + 1)*0.1f, (rand() % 10)*0.1f, (rand() % 10)*0.1f, 0.f), XMConvertToRadians(float(rand() % 180)));
		m_eShaderID = SHADER_GRASS;
		break;
	case GRASS_TEX:
		m_eShaderID = SHADER_GRASS_TEX;
		m_vDirection = _float3((rand() % 20 - 10) * 0.1f,  1.f, (rand() % 20 - 10)* 0.1f);
		m_fAngle = XMConvertToRadians(float(rand() % 180));
		break;
	default:
		break;
	}

	
	return S_OK;
}

int CObjectEffect::Tick(_float fTimeDelta)
{
	if (m_bDead)
	{
		return OBJ_DEAD;
	}
		
	__super::Tick(fTimeDelta);

	switch (m_EffectDesc.eEffectID)
	{
	case GRASS:
	case LAWN:
	case GRASS_TEX:
		Tick_Grass(fTimeDelta);
		break;
	default:
		break;
	}

	return OBJ_NOEVENT;
}

void CObjectEffect::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CObjectEffect::Render()
{

	__super::Render();
		

	return S_OK;
}

HRESULT CObjectEffect::Ready_Components(void * pArg)
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
	case GRASS:
		/* For.Com_Model*/
		if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_GrassLeaf"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case LAWN:
		/* For.Com_Model*/
		if (m_EffectDesc.iTextureNum == 0)
		{
			if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_LawnLeaf_0"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(__super::Add_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_LawnLeaf_1"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}
		break;
	case GRASS_TEX:
		/* For.Com_VIBuffer */
		if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
			return E_FAIL;

		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Grass"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
		break;
	default:
		break;
	}
	

	return S_OK;
}

HRESULT CObjectEffect::SetUp_ShaderResources()
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

	
	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (m_EffectDesc.eEffectType == MESH)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_TexUV", &m_fTexUV, sizeof(_float))))
			return E_FAIL;
	}
	else
	{

		if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(m_EffectDesc.iTextureNum))))
			return E_FAIL;

		if (FAILED(pGameInstance->Bind_RenderTarget_SRV(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
			return E_FAIL;
	}
		

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CObjectEffect::SetUp_ShaderID()
{
	return S_OK;
}

void CObjectEffect::Change_Animation(_float fTimeDelta)
{
}

void CObjectEffect::Change_Texture(_float fTimeDelta)
{
	
}


void CObjectEffect::Tick_Grass(_float fTimeDelta)
{
	m_fDeadtime += fTimeDelta;

	switch (m_EffectDesc.eEffectID)
	{
	case GRASS:
	case LAWN:
		//SetUp_BillBoard();
		if (m_fDeadtime > m_EffectDesc.fDeadTime*0.5f)
			m_fAlpha -= 0.05f;
		
		m_pTransformCom->Go_PosDir(fTimeDelta*0.5f, m_EffectDesc.vLook);
		break;
	case GRASS_TEX:
	{
		m_pTransformCom->Go_PosDir(fTimeDelta, XMLoadFloat3(&m_vDirection));
		SetUp_BillBoard();
		m_pTransformCom->Rotation(Get_TransformState(CTransform::STATE_LOOK), m_fAngle);
		
		if (m_fDeadtime > m_EffectDesc.fDeadTime*0.5f)
			m_fAlpha -= 0.05f;
		break;
	}
	default:
		break;
	}



	if ( m_fAlpha <= 0)
		m_bDead = true;
	
}


CObjectEffect * CObjectEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CObjectEffect*	pInstance = new CObjectEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CObjectEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CObjectEffect::Clone(void * pArg)
{
	CObjectEffect*	pInstance = new CObjectEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CObjectEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObjectEffect::Free()
{
	__super::Free();
}

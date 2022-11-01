#include "stdafx.h"
#include "..\Public\Hp.h"
#include "GameInstance.h"

CHp::CHp(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CObj_UI(pDevice, pContext)
{
}

CHp::CHp(const CHp & rhs)
	: CObj_UI(rhs)
{
}

HRESULT CHp::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHp::Initialize(void * pArg)
{

	m_fSize.x = 30;
	m_fSize.y = 30;
	m_fPosition = *(_float2*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eShaderID = UI_ALPHABLEND;

	CUI_Manager::Get_Instance()->Add_HpGroup(this);

	return S_OK;
}

int CHp::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	return OBJ_NOEVENT;
}

void CHp::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CHp::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CHp::Ready_Components(void * pArg)
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hp"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHp::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(m_iTextureNum))))
		return E_FAIL;

	return S_OK;
}

CHp * CHp::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHp*	pInstance = new CHp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CHp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHp::Clone(void * pArg)
{
	CHp*	pInstance = new CHp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CInvenItem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHp::Free()
{
	__super::Free();
}

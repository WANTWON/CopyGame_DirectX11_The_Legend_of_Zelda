#include "stdafx.h"
#include "..\Public\PrizeItem.h"
#include "GameInstance.h"

CPrizeItem::CPrizeItem(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CBaseObj(pDevice, pContext)
{
}

CPrizeItem::CPrizeItem(const CPrizeItem & rhs)
	: CBaseObj(rhs)
{
}

HRESULT CPrizeItem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPrizeItem::Initialize(void * pArg)
{
	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;
}

int CPrizeItem::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return OBJ_DEAD;

	return OBJ_NOEVENT;
}

void CPrizeItem::Late_Tick(_float fTimeDelta)
{
}

HRESULT CPrizeItem::Render()
{
	if (m_iCurrentLevel == LEVEL_LOADING)
		return E_FAIL;

	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderID()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(m_eShaderID);

	m_pVIBufferCom->Render();
}

HRESULT CPrizeItem::Ready_Components(void * pArg)
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 3.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(1.0f);
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hp"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return E_NOTIMPL;
}

HRESULT CPrizeItem::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(m_iTextureNum))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPrizeItem::SetUp_ShaderID()
{
	return S_OK;
}

CPrizeItem * CPrizeItem::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPrizeItem*	pInstance = new CPrizeItem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CPrizeItem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPrizeItem::Clone(void * pArg)
{
	CPrizeItem*	pInstance = new CPrizeItem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CPrizeItem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPrizeItem::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}

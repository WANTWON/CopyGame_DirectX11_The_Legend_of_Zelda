#include "stdafx.h"
#include "..\Public\BackGround.h"

#include "GameInstance.h"


CBackGround::CBackGround(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CObj_UI(pDevice, pContext)
{
}

CBackGround::CBackGround(const CBackGround & rhs)
	: CObj_UI(rhs)
{
}

HRESULT CBackGround::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBackGround::Initialize(void * pArg)
{
	m_fSize.x = g_iWinSizeX;
	m_fSize.y = g_iWinSizeY;
	m_fPosition.x = g_iWinSizeX >> 1;
	m_fPosition.y = g_iWinSizeY >> 1;
	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eShaderID = UI_ALPHABLEND;

	if (m_BackgroundDesc.eVisibleScreen == VISIBLE_SCREEN)
	{
		m_fAlpha = 0.f;
		m_eShaderID = UI_SCREEN;
	}

	return S_OK;
}

int CBackGround::Tick(_float fTimeDelta)
{
	
	return S_OK;
}

void CBackGround::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_BackgroundDesc.eVisibleScreen == VISIBLE_SCREEN)
	{
		if (CUI_Manager::Get_Instance()->Get_NextLevel() == true)
		{
			m_fAlpha += 0.05f;

			if (m_fAlpha >= 1.f)
			{
				CUI_Manager::Get_Instance()->Set_NextLevelFinished(true);
				m_fAlpha = 1.f;
			}	
			else
				CUI_Manager::Get_Instance()->Set_NextLevelFinished(false);
		}
		else
		{
			m_fAlpha -= 0.05f;
			if (m_fAlpha <= 0.f)
				m_fAlpha = 0.f;
		}
	}


}

HRESULT CBackGround::Render()
{
	if (!CUI_Manager::Get_Instance()->Get_UI_Open() && m_BackgroundDesc.eVisibleScreen == VISIBLE_PLAYGAME)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CBackGround::Ready_Components(void * pArg)
{
	if (pArg != nullptr)
		memcpy(&m_BackgroundDesc, pArg, sizeof(BACKGROUNDESC));


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (m_BackgroundDesc.pTextureTag != nullptr)
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, m_BackgroundDesc.pTextureTag, (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}
	

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackGround::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;
	

	if (m_BackgroundDesc.eVisibleScreen == VISIBLE_SCREEN)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
			return E_FAIL;
	}

	if (m_BackgroundDesc.pTextureTag != nullptr)
	{
		if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(0))))
			return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CBackGround::SetUp_ShaderID()
{
	return S_OK;
}

CBackGround * CBackGround::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBackGround*	pInstance = new CBackGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CBackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CBackGround::Clone(void * pArg)
{
	CBackGround*	pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CBackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBackGround::Free()
{
	__super::Free();
}

#include "stdafx.h"
#include "..\Public\UIButton.h"
#include "GameInstance.h"


CUIButton::CUIButton(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CObj_UI(pDevice, pContext)
{
}

CUIButton::CUIButton(const CUIButton & rhs)
	: CObj_UI(rhs)
{
}

HRESULT CUIButton::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIButton::Initialize(void * pArg)
{
	if (pArg != nullptr)
		memcpy(&m_ButtonDesc, pArg, sizeof(BUTTONDESC));

	
	m_fSize.x = 30;
	m_fSize.y = 30;
		

	m_fPosition.x = m_ButtonDesc.vPosition.x;
	m_fPosition.y = m_ButtonDesc.vPosition.y;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eShaderID = SHADER_ALPHABLEND;

	return S_OK;
}

int CUIButton::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	return OBJ_NOEVENT;
}

void CUIButton::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_ButtonDesc.eButtonType == BTN_INVEN)
	{
		CObj_UI* pInvenObj = CUI_Manager::Get_Instance()->Get_EquipItem((CUI_Manager::EQUIP_BT)m_ButtonDesc.eState);

		if (pInvenObj == nullptr)
			m_fPosition = _float2(0.f, 0.f);
		else
		{
			m_fPosition.x = pInvenObj->Get_Position().x - 30;
			m_fPosition.y = pInvenObj->Get_Position().y + 30;
		}

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fPosition.x - g_iWinSizeX * 0.5f, -m_fPosition.y + g_iWinSizeY * 0.5f, 0.f, 1.f));
	}
}

HRESULT CUIButton::Render()
{
	if (!CUI_Manager::Get_Instance()->Get_UI_Open() && m_ButtonDesc.eButtonType == BTN_INVEN)
		return E_FAIL;

	if (m_ButtonDesc.eButtonType == BTN_INVEN && nullptr == CUI_Manager::Get_Instance()->Get_EquipItem((CUI_Manager::EQUIP_BT)m_ButtonDesc.eState))
		return E_FAIL;	

	if (nullptr == m_pShaderCom ||
		nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin();

	m_pVIBufferCom->Render();


	return S_OK;
}

HRESULT CUIButton::Ready_Components()
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

	switch (m_ButtonDesc.eState)
	{
	case BTN_X:
		/* For.Com_Texture */
		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_ButtonX"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
		break;
	case BTN_Y:
		/* For.Com_Texture */
		if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_ButtonY"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
		break;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIButton::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_World4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (m_ButtonDesc.eButtonType == BTN_FIX)
	{
		if (CUI_Manager::Get_Instance()->Get_UI_Open())
			m_ButtonDesc.eColor = BTN_GREEN;
		else
			m_ButtonDesc.eColor = BTN_BLACK;
	}


	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTextureCom->Get_SRV(m_ButtonDesc.eColor))))
		return E_FAIL;

	return S_OK;
}


CUIButton * CUIButton::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUIButton*	pInstance = new CUIButton(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CInvenTile"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CUIButton::Clone(void * pArg)
{
	CUIButton*	pInstance = new CUIButton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CUIButton"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIButton::Free()
{
	__super::Free();
}

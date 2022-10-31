#include "stdafx.h"
#include "..\Public\Hp.h"

CHp::CHp(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CObj_UI(pDevice, pContext)
{
}

CHp::CHp(const CHp & rhs)
	: CObj_UI(rhs)
{
}

HRESULT CHp::Initialize(void * pArg)
{

	m_fSize.x = 100;
	m_fSize.y = 100;
//	m_fPosition = m_ItemDesc.vPosition;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eShaderID = UI_ALPHABLEND;

	return S_OK;
}

HRESULT CHp::Ready_Components(void * pArg)
{
	return E_NOTIMPL;
}

HRESULT CHp::SetUp_ShaderResources()
{
	return E_NOTIMPL;
}

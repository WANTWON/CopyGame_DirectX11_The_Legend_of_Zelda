#include "stdafx.h"
#include "..\Public\BaseObj.h"


CBaseObj::CBaseObj(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CBaseObj::CBaseObj(const CBaseObj & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBaseObj::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CBaseObj::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

int CBaseObj::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	return OBJ_NOEVENT;
}

void CBaseObj::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBaseObj::Render()
{
	return S_OK;
}

void CBaseObj::Change_Direction()
{
	if (m_eDir[DIR_X] != m_ePreDir[DIR_X] || m_eDir[DIR_Z] != m_ePreDir[DIR_Z])
	{
		m_pTransformCom->Change_Direction(m_eDir[DIR_Z], m_eDir[DIR_X]);
		m_ePreDir[DIR_X] = m_eDir[DIR_X];
		m_ePreDir[DIR_Z] = m_eDir[DIR_Z];
	}
}



_vector CBaseObj::Get_Position()
{
	if (m_pTransformCom == nullptr)
		return _vector();


	return m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}

void CBaseObj::Set_State(CTransform::STATE eState, _fvector vState)
{
	if (m_pTransformCom == nullptr)
		return;

	m_pTransformCom->Set_State(eState, vState);
}

void CBaseObj::Set_Scale(_float3 vScale)
{
	m_vScale = vScale;
}

void CBaseObj::Free()
{
	__super::Free();
}

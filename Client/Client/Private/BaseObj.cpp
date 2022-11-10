#include "stdafx.h"
#include "..\Public\BaseObj.h"
#include "GameInstance.h"
#include "VIBuffer_Navigation.h"
#include "PipeLine.h"

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

void CBaseObj::Update_Collider()
{
	if (m_pAABBCom != nullptr)
		m_pAABBCom->Update(m_pTransformCom->Get_WorldMatrix());
	if (m_pOBBCom != nullptr)
		m_pOBBCom->Update(m_pTransformCom->Get_WorldMatrix());
	if (m_pSPHERECom != nullptr)
		m_pSPHERECom->Update(m_pTransformCom->Get_WorldMatrix());
}


_vector CBaseObj::Get_TransformState(CTransform::STATE eState)
{
	if (m_pTransformCom == nullptr)
		return _vector();


	return m_pTransformCom->Get_State(eState);
}

_float2 CBaseObj::Get_ProjPosition()
{
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_matrix ViewMatrix =  pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	_matrix ProjMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);

	vPosition = XMVector3TransformCoord(vPosition, ViewMatrix);
	vPosition = XMVector3TransformCoord(vPosition, ProjMatrix);

	_float ScreenX = XMVectorGetX(vPosition)* (g_iWinSizeX / 2) + (g_iWinSizeX / 2);
	_float ScreenY = XMVectorGetY(vPosition) * (g_iWinSizeY / 2) + (g_iWinSizeY / 2);

	RELEASE_INSTANCE(CGameInstance);

	return _float2( ScreenX, ScreenY);
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

	m_pTransformCom->Set_Scale(CTransform::STATE_RIGHT, m_vScale.x);
	m_pTransformCom->Set_Scale(CTransform::STATE_UP, m_vScale.y);
	m_pTransformCom->Set_Scale(CTransform::STATE_LOOK, m_vScale.z);

}

CCollider * CBaseObj::Get_Collider()
{
	if (m_pAABBCom != nullptr)
		return m_pAABBCom;
	else if (m_pOBBCom != nullptr)
		return m_pOBBCom;
	else if (m_pSPHERECom != nullptr)
		return m_pSPHERECom;
	else
		return nullptr;
}

void CBaseObj::Free()
{
	__super::Free();

	Safe_Release(m_pAABBCom);
	Safe_Release(m_pOBBCom);
	Safe_Release(m_pSPHERECom);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}

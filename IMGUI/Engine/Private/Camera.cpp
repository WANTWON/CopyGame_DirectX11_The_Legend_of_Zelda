#include "..\Public\Camera.h"
#include "PipeLine.h"

_tchar*	CCamera::m_pTransformTag = TEXT("Com_Transform");

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void * pArg)
{
	m_pTransform = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransform)
		return E_FAIL;	

	memcpy(&m_CameraDesc, pArg, sizeof(CAMERADESC));

	m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_CameraDesc.vEye));
	m_pTransform->LookAt(XMLoadFloat4(&m_CameraDesc.vAt));

	m_pTransform->Set_TransformDesc(m_CameraDesc.TransformDesc);

	m_Components.emplace(m_pTransformTag, m_pTransform);

	Safe_AddRef(m_pTransform);

	return S_OK;
}

void CCamera::Tick(_float fTimeDelta)
{
}

void CCamera::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}

HRESULT CCamera::Bind_OnGraphicDev()
{
	/* 카메라의 월드행렬을 얻어온다. */
	_matrix		WorldMatrix = m_pTransform->Get_WorldMatrix();

	_matrix TstMatrix = XMMatrixIdentity();
	_matrix Invmatrix = XMMatrixInverse(nullptr, TstMatrix);

	/* 월드 행렬의 역행렬(뷰스페이스 변환행렬)을 구하자. */
	_matrix		ViewMatrix = XMMatrixInverse(nullptr, WorldMatrix);
	_matrix		ProjMatrix = XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar);


	CPipeLine::Get_Instance()->Set_Transform(CPipeLine::D3DTS_VIEW, ViewMatrix);
	CPipeLine::Get_Instance()->Set_Transform(CPipeLine::D3DTS_PROJ, ProjMatrix);

	return S_OK;
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);

}

#include "stdafx.h"
#include "..\Public\Camera_Dynamic.h"
#include "GameInstance.h"
#include "Player.h"

CCamera_Dynamic::CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Dynamic::CCamera_Dynamic(const CCamera_Dynamic & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Dynamic::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CCamera_Dynamic::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(&((CAMERADESC_DERIVED*)pArg)->CameraDesc)))
		return E_FAIL;
	
	m_vDistance = ((CAMERADESC_DERIVED*)pArg)->CameraDesc.vEye;
	return S_OK;
}

int CCamera_Dynamic::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	switch (m_eCamMode)
	{
	case Client::CCamera_Dynamic::CAM_PLAYER:
		Player_Camera(fTimeDelta);
		break;
	case Client::CCamera_Dynamic::CAM_SHAKING:
		Shaking_Camera(fTimeDelta, m_fPower);
		break;
	default:
		break;
	}

	


	if (FAILED(Bind_OnPipeLine()))
		return OBJ_NOEVENT;

	return OBJ_NOEVENT;
}

void CCamera_Dynamic::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CCamera_Dynamic::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	
	return S_OK;
}

void CCamera_Dynamic::Player_Camera(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_lMouseWheel > 0)
		m_lMouseWheel -= 0.001;
	if (m_lMouseWheel < 0)
		m_lMouseWheel += 0.001;

	if (m_lMouseWheel += (pGameInstance->Get_DIMMoveState(DIMM_WHEEL)*0.05))
	{
		m_vDistance.y -= _float(fTimeDelta*m_lMouseWheel*0.01f);
		m_vDistance.z += _float(fTimeDelta*m_lMouseWheel*0.01f);
	}


	if (pGameInstance->Key_Pressing(DIK_F1))
	{
		m_vDistance.y -= 0.03f;
		m_vDistance.z -= 0.06f;
	}
	if (pGameInstance->Key_Pressing(DIK_F2))
	{
		m_vDistance.y += 0.03f;
		m_vDistance.z += 0.06f;
	}


	CPlayer* pTarget = (CPlayer*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));

	Safe_AddRef(pTarget);

	_vector m_TargetPos = pTarget->Get_TransformState(CTransform::STATE_POSITION);

	Safe_Release(pTarget);

	m_pTransform->LookAt(m_TargetPos);

	m_pTransform->Follow_Target(fTimeDelta, m_TargetPos, XMVectorSet(m_vDistance.x , m_vDistance.y, m_vDistance.z, 0.f));

	RELEASE_INSTANCE(CGameInstance);
}

void CCamera_Dynamic::Shaking_Camera(_float fTimeDelta, _float fPower)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CPlayer* pTarget = (CPlayer*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));

	Safe_AddRef(pTarget);

	_float3 vTargetPos;
	XMStoreFloat3(&vTargetPos, pTarget->Get_TransformState(CTransform::STATE_POSITION));

	++m_iShakingCount;
	if (m_iShakingCount % 4 == 0)
	{
		vTargetPos.y += fPower*m_fVelocity;
		if (rand() % 2 == 0)
			vTargetPos.z -= fPower*m_fVelocity;
		else
			vTargetPos.z += fPower*m_fVelocity;

		if (rand() % 2 == 0)
			vTargetPos.x -= fPower*m_fVelocity;
		else
			vTargetPos.x += fPower*m_fVelocity;
	}
	else if (m_iShakingCount % 4 == 1)
	{
		vTargetPos.y -= fPower*m_fVelocity;
		if (rand() % 2 == 0)
			vTargetPos.z -= fPower*m_fVelocity;
		else
			vTargetPos.z += fPower*m_fVelocity;

		if (rand() % 2 == 0)
			vTargetPos.x -= fPower*m_fVelocity;
		else
			vTargetPos.x += fPower*m_fVelocity;

	}

	m_fVelocity -= m_fMinusVelocity;
	if (m_fVelocity < 0.0f)
	{
		m_eCamMode = CAM_PLAYER;
		Safe_Release(pTarget);
		Safe_Release(pGameInstance);
		return;
	}

	Safe_Release(pTarget);

	_vector vecTargetPos = XMLoadFloat3(&vTargetPos);
	vecTargetPos = XMVectorSetW(vecTargetPos, 1.f);

	m_pTransform->LookAt(vecTargetPos);
	m_pTransform->Follow_Target(fTimeDelta, vecTargetPos, XMVectorSet(m_vDistance.x, m_vDistance.y, m_vDistance.z, 0.f));
	Safe_Release(pGameInstance);
}

CCamera_Dynamic * CCamera_Dynamic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Dynamic*	pInstance = new CCamera_Dynamic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CCamera_Dynamic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Dynamic::Clone(void* pArg)
{
	CCamera_Dynamic*	pInstance = new CCamera_Dynamic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CCamera_Dynamic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Dynamic::Free()
{
	__super::Free();

	
}

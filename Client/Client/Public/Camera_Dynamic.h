#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
END

BEGIN(Client)

class CCamera_Dynamic final : public CCamera
{
public:
	enum CAMERAMODE {CAM_PLAYER, CAM_SHAKING, CAM_TAILCAVE, CAM_ITEMGET, CAM_END };

	typedef struct tagCameraDesc_Derived
	{
		_float4						InitPostion = _float4(0.f, 0.f, 0.f, 1.f);
		_uint						iTest;
		CCamera::CAMERADESC			CameraDesc;
	}CAMERADESC_DERIVED;

private:
	CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Dynamic(const CCamera_Dynamic& rhs);
	virtual ~CCamera_Dynamic() = default;

public:
	void Set_CamMode(CAMERAMODE _eCamMode, _float fPower, _float fVelocity, _float fMinusVelocity)
	{
		if (m_eCamMode == CAM_PLAYER)
			m_ePreCamMode = CAM_PLAYER;
		if (m_eCamMode == CAM_TAILCAVE)
			m_ePreCamMode = CAM_TAILCAVE;
		if (m_eCamMode == CAM_ITEMGET)
			m_ePreCamMode = CAM_ITEMGET;

		m_eCamMode = _eCamMode; m_fPower = fPower; m_fVelocity = fVelocity; m_fMinusVelocity = fMinusVelocity;
	
	}
	void Set_CamMode(CAMERAMODE _eCamMode) { m_eCamMode = _eCamMode; m_fTime = 0.f; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

public:
	void Player_Camera(_float fTimeDelta);
	void Shaking_Camera(_float fTimeDelta, _float fPower);
	void Terrain_Camera(_float fTimeDelta);
	void Item_Camera(_float fTimeDelta);
	void Set_Position(_vector vPosition);

private:
	CAMERAMODE		m_ePreCamMode = CAM_PLAYER;
	CAMERAMODE		m_eCamMode = CAM_PLAYER;
	_float4			m_vDistance = _float4(0, 10, -10, 0.f);
	_double			m_lMouseWheel = 0;
	_float4			m_fTargetPos = _float4(0.f, 0.f, 0.f, 1.f);

	/* For Shaking Camera */
	_float m_fPower = 0.f;
	_float m_fVelocity = 0.f;
	_float m_fMinusVelocity = 0.f;
	_float m_fTime = 0.f;
	_int   m_iShakingCount = 0; 

public:
	static CCamera_Dynamic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
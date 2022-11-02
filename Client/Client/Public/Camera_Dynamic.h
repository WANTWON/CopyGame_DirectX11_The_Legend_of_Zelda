#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
END

BEGIN(Client)

class CCamera_Dynamic final : public CCamera
{
public:
	enum CAMERAMODE {CAM_PLAYER, CAM_TURNMODE};

	typedef struct tagCameraDesc_Derived
	{
		_uint						iTest;
		CCamera::CAMERADESC			CameraDesc;
	}CAMERADESC_DERIVED;
private:
	CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Dynamic(const CCamera_Dynamic& rhs);
	virtual ~CCamera_Dynamic() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

public:
	void Player_Camera(_float fTimeDelta);
	void Turn_Camera(_float fTimeDelta);

private:
	CAMERAMODE m_eCamMode = CAM_PLAYER;
	_float3 m_vDistance = _float3(0, 8, -8);
	_long			m_lMouseWheel = 0;
public:
	static CCamera_Dynamic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
#pragma once

#include "BaseObj.h"
BEGIN(Engine)
class CModel;
END


BEGIN(Client)

class CDoor final : public CBaseObj
{
public:
	enum DOORTYPE { DOOR_CLOSED, DOOR_KEY, DOOR_BOSS };
	enum STATE { CLOSE, CLOSE2, OPEN, OPEN2, OPEN_WAIT, OPEN_WAIT2};

	typedef struct DoorTag
	{
		DOORTYPE eType = DOOR_CLOSED;
		_float3 InitPosition = _float3(0.f, 0.f, 0.f);
		_float fAngle = 0.f;
	}DOORDESC;

private:
	CDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDoor() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Change_Animation(_float fTimeDelta);
	virtual HRESULT Ready_Components(void* pArg = nullptr) override;
	virtual HRESULT SetUp_ShaderResources() override;

private:
	CModel*					m_pModelCom = nullptr;
	_bool					m_bOpen = false;
	_bool					m_bPlay = false;

	STATE m_eState = OPEN;
	STATE m_ePreState = OPEN2;

	DOORDESC m_DoorDesc;

public:
	static CDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
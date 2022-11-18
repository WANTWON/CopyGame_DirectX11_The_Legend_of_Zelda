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
	enum STATE_CLOSED { CLOSE_CD, CLOSE2_CD, OPEN_CD, OPEN2_CD, OPEN_WAIT_CD, OPEN_WAIT2_CD};
	enum STATE_LOCK { CLOSE_LD, OPEN_LD, OPEN2_LD};
	enum STATEBOSS { OPEN_BOSS, REMOVE_KEY };
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
	void Tick_ClosedDoor(_float fTimeDelta);
	void Tick_LockDoor(_float fTimeDelta);
	void Tick_BossDoor(_float fTimeDelta);
	void Change_Animation_ClosedDoor(_float fTimeDelta);
	void Change_Animation_LockDDoor(_float fTimeDelta);
	void Change_Animation_BossDoor(_float fTimeDelta);

private:
	CModel*					m_pModelCom = nullptr;
	_bool					m_bOpen = false;
	_bool					m_bPlay = false;

	_uint m_eState = OPEN_CD;
	_uint m_ePreState = OPEN2_CD;

	DOORDESC m_DoorDesc;

public:
	static CDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
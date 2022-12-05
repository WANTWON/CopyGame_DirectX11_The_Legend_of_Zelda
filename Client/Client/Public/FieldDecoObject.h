#pragma once
#include "BaseObj.h"

BEGIN(Engine)
class CModel;
class CNavigation;
END

BEGIN(Client)
class CFieldDecoObject final : public CBaseObj
{
public:
	enum TYPE { BUTTERFLY };
	enum STATE { IDLE, WALK };

	typedef struct DecoObjectTag
	{
		TYPE eDecoType = BUTTERFLY;
		_float3 vInitPos = _float3(0.f, 0.f, 0.f);

	}DECODESC;

private:
	CFieldDecoObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CFieldDecoObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual HRESULT Ready_Components(void* pArg = nullptr) override;
	virtual HRESULT SetUp_ShaderResources() override;
	void Check_Navigation(_float fTimeDelta);

private:
	void Change_Animation(_float fTimeDelta);

private:
	CNavigation*			m_pNavigationCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	STATE					m_eState = IDLE;
	_float					m_fAnimSpeed = 0.f;
	_float					m_fAngle = 0.f;
	_float					m_fHeight = 0.f;

	DWORD					m_dwTime = GetTickCount();
	DECODESC				m_DecoDesc;
	
public:
	static CFieldDecoObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
	

};

END
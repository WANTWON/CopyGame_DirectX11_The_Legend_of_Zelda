#pragma once

#include "NonAnim.h"


BEGIN(Client)

class CPortal final : public CNonAnim
{
public:
	typedef struct Portaltag
	{
		_float3 vInitPos = _float3(0.f, 0.f, 0.f);
		_float3 vConnectPos = _float3(0.f, 0.f, 0.f);
		_bool bConnectPortal2D = false;
	}PORTALDESC;

private:
	CPortal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPortal(const CPortal& rhs);
	virtual ~CPortal() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual int Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();


private:
	virtual HRESULT Ready_Components(void* pArg) override;

private:
	PORTALDESC  m_PortalDesc;


public:
	static CPortal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
#pragma once

#include "NonAnim.h"


BEGIN(Client)

class CCollapseTile final : public CNonAnim
{
public:

private:
	CCollapseTile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollapseTile(const CCollapseTile& rhs);
	virtual ~CCollapseTile() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual int Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();


private:
	virtual HRESULT Ready_Components(void* pArg) override;

public:
	static CCollapseTile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
#pragma once

#include "BaseObj.h"

BEGIN(Client)

class CTerrain final : public CBaseObj
{
public:
	enum TEXTURE { TYPE_DIFFUSE, TYPE_BRUSH, TYPE_FILTER, TYPE_END };
private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual int Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render() override;

public:
	virtual _bool Picking(_float3* PickingPoint);
	virtual void PickingTrue();

private:
	HRESULT Ready_Components(void* pArg);
	HRESULT SetUp_ShaderID();
	HRESULT SetUp_ShaderResources();

private:
	_bool	m_bWireFrame = false;
	_bool	m_bDebugTerrain = false;
	_bool	m_bDebugShow = true;
	_float3 m_vMousePickPos;

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
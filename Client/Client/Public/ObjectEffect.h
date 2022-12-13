#pragma once
#include "Client_Defines.h"
#include "Effect.h"
#include "GameInstance.h"



BEGIN(Client)
class CPlayerEffect final : public CEffect
{
public:
	enum TYPE { FOOTSMOKE, ROLLCUT,};



protected:
	CPlayerEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerEffect(const CPlayerEffect& rhs);
	virtual ~CPlayerEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Target(CBaseObj* pObject) { m_pTarget = pObject; }


private:
	virtual HRESULT Ready_Components(void* pArg = nullptr);
	virtual HRESULT SetUp_ShaderResources();
	virtual HRESULT SetUp_ShaderID();
	virtual void Change_Animation(_float fTimeDelta);
	virtual void Change_Texture(_float fTimeDelta) override;

private:
	void Tick_Smoke(_float fTimeDelta);
	void Tick_RollCut(_float fTimeDelta);

private:
	_float m_fScale = 0.f;

public:
	static CPlayerEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
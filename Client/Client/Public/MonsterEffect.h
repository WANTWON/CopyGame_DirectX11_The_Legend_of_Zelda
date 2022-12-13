#pragma once
#include "Client_Defines.h"
#include "Effect.h"
#include "GameInstance.h"



BEGIN(Client)
class CMonsterEffect final : public CEffect
{
public:
	enum TYPE { HITFLASH, HITFLASH_TEX, HITRING};



protected:
	CMonsterEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterEffect(const CMonsterEffect& rhs);
	virtual ~CMonsterEffect() = default;

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
	void Tick_HitFlash(_float fTimeDelta);

private:
	_float m_fScale = 0.f;

public:
	static CMonsterEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
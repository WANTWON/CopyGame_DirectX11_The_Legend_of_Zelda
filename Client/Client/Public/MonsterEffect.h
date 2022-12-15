#pragma once
#include "Client_Defines.h"
#include "Effect.h"
#include "GameInstance.h"



BEGIN(Client)
class CMonsterEffect final : public CEffect
{
public:
	enum TYPE { HITFLASH, HITFLASH_TEX, HITRING, HITSPARK, 
		DEADSMOKE, GLOW_SPHERE, SMOKEBACK, SMOKEFRONT,
		GLOW_LARGE, GLOW_MIDDLE, GLOW_SMALL, GLOW_MINI
	};



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
	void Tick_DeadEffect(_float fTimeDelta);
	void Tick_GlowEffect(_float fTimeDelta);
	void Make_GlowEffect();

private:
	_float m_fAngle = 0.f;
	_float m_fSpeed = 0.f;

	_bool  m_bMakeGlow = false;
	_bool  m_bMax = false;
	_float m_fGlowMaxSize = 0.f;
	_float m_fGlowAddScale = 0.f;

	_vector m_vColorBack = XMVectorSet(1.f, 1.f, 1.f, 1.f);
	_vector m_vColorFront = XMVectorSet(1.f, 1.f, 1.f, 1.f);


	CTexture* m_pDissolveTexture = nullptr;
	CTexture* m_pSmokeDstTexture = nullptr;


public:
	static CMonsterEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
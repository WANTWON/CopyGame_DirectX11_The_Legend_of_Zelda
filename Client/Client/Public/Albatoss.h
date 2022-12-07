#pragma once
#include "Monster.h"


BEGIN(Client)
class CAlbatoss final : public CMonster
{
public:
	enum STATE {
		ATTACK_CLAW, ATTACK_CLAW_ED, ATTACK_CLAW_LP, ATTACK_CLAW_ST,
		ATTACK_FLAPPING, ATTACK_FLAPPING_ED, ATTACK_FLAPPING_ST,
		DAMAGE_HOVERING, DAMAGE_RUSH, DEAD, DEMO_POP, DEMO_POP_SHORT,
		HOVERING_LP, HOVERING_LP_FAST, HOVERING_ST,
		PICCOLO_WAIT, RUSH, RUSH_RETURN, RUSH_ST,
		WEAK_HOVERING, WEAK_HOVERING_ST
	};

	enum ATTACK_STATE { RUSH_STATE, CLAW_STATE , ATTACK_2 };

private:
	CAlbatoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CAlbatoss() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Check_Navigation(_float fTimeDelta)  override;

public:
	virtual _uint Take_Damage(float fDamage, void* DamageType, CBaseObj* DamageCauser) override;


private:
	virtual void Change_Animation(_float fTimeDelta) override;
	virtual HRESULT Ready_Components(void* pArg = nullptr) override;
	virtual HRESULT SetUp_ShaderResources() override;
	virtual _bool IsDead() override;
	virtual void Find_Target() override;
	virtual void Follow_Target(_float fTimeDelta) override;

private:
	virtual void AI_Behaviour(_float fTimeDelta) override;
	void Opening_Motion(_float fTimeDelta);
	void Rush_Attack(_float fTimeDelta);
	void Claw_Attack(_float fTimeDelta);

private:
	STATE m_eState = PICCOLO_WAIT;
	_bool m_bOpening = false;
	_bool m_bFirst = false;
	_bool m_bDownHovering = false;

	DWORD m_dwOpeningTime = GetTickCount();

	_float3			m_vTargetDistance = _float3(0.f, 0.f, 0.f);
	_float4			m_ClawingPos = _float4(0.f, 30.f, 0.f, 1.f);
	_float4			m_RushRightPos = _float4(10.f, 19.f, 0.f, 1.f);
	_float4			m_RushLeftPos = _float4(-10.f, 19.f, 0.f, 1.f);

	_uint	m_eAttackMode = RUSH_STATE;
	_vector m_vLastDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	_uint	m_iRushCount = 0;
	_uint	m_iClawCount = 0;

public:
	static CAlbatoss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;


};

END
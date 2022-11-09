#pragma once

#include "BaseObj.h"


BEGIN(Engine)
class CModel;
class CNavigation;
END


BEGIN(Client)

class CPlayer final : public CBaseObj
{
public:
	//ST : Start,  LP : Loop, ED : End
	enum ANIM {
		IDLE, RUN, WALK, D_FALL, D_JUMP, D_LAND, JUMP, LAND, S_SLASH, SLASH, SLASH_HOLD_B, SLASH_HOLD_ED, SLASH_HOLD_F,
		SLASH_HOLD_L, SLASH_HOLD_LP, SLASH_HOLD_R, SLASH_HOLD_ST, SHIELD_ED, SHIELD_LP, SHIELD_ST, SHIELD_HIT,
		BOW_ED, BOW_ST, DASH_ED, DASH_LP, DASH_ST, DMG_B, DMG_F, DMG_PRESS, DMG_QUAKE, ITEM_GET_ED, ITEM_GET_LP, ITEM_GET_ST,
		KEY_OPEN
	};

	enum MESH_NAME {
		MESH_HAIR, MESH_CLOTHES, MESH_SWORD, MESH_EAR, MESH_HAT, MESH_HOOK, MESH_FACE, MESH_SHEILD,
		MESH_SWORD2, MESH_OCARINA, MESH_SHOVEL, MESH_SHOES, MESH_WAND, MESH_BELT, MESH_MOUSE, MESH_FILPPER, MESH_NONE
	};

	enum PARTS { PARTS_BOW, PARTS_END };

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual int Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	ANIM Get_AnimState() { return m_eState; }
	void Set_AnimState(ANIM eAnim) { m_eState = eAnim; }
	OBJINFO Get_Info() { return m_tInfo; }
	void Set_Info(OBJINFO Info) { m_tInfo = Info; }
	void Set_JumpingHeight(_float fHeight) { m_fStartHeight = fHeight; m_fEndHeight = fHeight; }
	virtual _uint Take_Damage(float fDamage, void* DamageType, CBaseObj* DamageCauser) override;

private:
	void Key_Input(_float fTimeDelta);
	void Change_Direction(_float fTimeDelta);
	void Change_Animation(_float fTimeDelta);
	void Render_Model(MESH_NAME eMeshName);

private:
	HRESULT Ready_Parts();
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT SetUp_ShaderResources() override; /* 셰이더 전역변수에 값을 전달한다. */
	virtual HRESULT SetUp_ShaderID() override;
	void SetDirection_byLook(_float fTimeDelta);
	void SetDirection_byPosition(_float fTimeDelta);
	

private:
	vector<class CGameObject*>			m_Parts;
	CNavigation*			m_pNavigationCom = nullptr;
private:
	OBJINFO					m_tInfo;

	CModel*					m_pModelCom = nullptr;
	ANIM					m_eState = IDLE;
	ANIM					m_ePreState = IDLE;
	_float					m_eAnimSpeed = 1.f;

	_int					m_iDash[DIR_END] = { 0 };
	//LeftHand : MESH_SHEILD, MESH_OCARINA 
	//RightHand : MESH_SWORD MESH_SWORD2 MESH_HOOK, MESH_SHOVEL, MESH_WAND, MESH_FILPPER
	MESH_NAME				m_eLeftHand = MESH_SHEILD;
	MESH_NAME				m_eRightHand = MESH_SWORD;
	_bool					m_bIsLoop = true;
	_bool					m_bPressed = false;

	_float					m_fTime = 0.f;
	_float					m_fStartHeight = 2.2f;
	_float					m_fEndHeight = 2.2f;
	_float					m_fPressedScale = 1.f;

	DWORD					m_dwDashTime = GetTickCount();
	DWORD					m_dwPressedTime = GetTickCount();


public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
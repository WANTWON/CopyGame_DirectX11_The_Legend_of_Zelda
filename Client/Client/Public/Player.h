#pragma once

#include "BaseObj.h"


BEGIN(Engine)
class CModel;
END


BEGIN(Client)

class CPlayer final : public CBaseObj
{
public:
	//ST : Start,  LP : Loop, ED : End
	enum ANIM {
		IDLE, RUN, WALK, JUMP, LAND, SLASH, SLASH_HOLD_ED, SLASH_HOLD_LP, SLASH_HOLD_ST,
		SHIELD_ED, SHIELD_LP, SHIELD_ST, SHIELD_HIT
	};

	enum MESH_NAME {
		MESH_HAIR, MESH_CLOTHES, MESH_SWORD, MESH_EAR, MESH_HAT, MESH_HOOK, MESH_FACE, MESH_SHEILD,
		MESH_SWORD2, MESH_OCARINA, MESH_SHOVEL, MESH_SHOES, MESH_WAND, MESH_BELT, MESH_MOUSE, MESH_FILPPER, MESH_NONE
	};

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

private:
	void Key_Input(_float fTimeDelta);
private:
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT SetUp_ShaderResources() override; /* 셰이더 전역변수에 값을 전달한다. */
	virtual HRESULT SetUp_ShaderID() override;
	void Render_Model(MESH_NAME eMeshName);
	void Change_Direction();
	void Change_Animation(_float fTimeDelta);


private:
	CModel*					m_pModelCom = nullptr;
	ANIM					m_eState = IDLE;
	ANIM					m_ePreState = IDLE;
	_float					m_eAnimSpeed = 1.f;

	//LeftHand : MESH_SHEILD, MESH_OCARINA 
	//RightHand : MESH_SWORD MESH_SWORD2 MESH_HOOK, MESH_SHOVEL, MESH_WAND, MESH_FILPPER
	MESH_NAME				m_eLeftHand = MESH_SHEILD;
	MESH_NAME				m_eRightHand = MESH_SWORD;
	_bool					m_bIsLoop = true;
	_float					m_fTime = 0.f;
	_float					m_MaxTime = 0.f;
	
public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
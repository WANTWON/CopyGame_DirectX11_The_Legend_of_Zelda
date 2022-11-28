#pragma once
#include "Client_Defines.h"
#include "BaseObj.h"
#include "GameInstance.h"
#include "Cell.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)
class CNpc abstract : public CBaseObj
{
public:
	enum NPC_ID { SHOP  };
	enum DMG_DIRECTION {FRONT, BACK};

	typedef struct NpcTad
	{
		_float3 vInitPos = _float3(0.f, 0.f, 0.f); 

	}NPCDESC;
	

protected:
	CNpc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNpc(const CNpc& rhs);
	virtual ~CNpc() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Check_Navigation(_float fTimeDelta);
	virtual void Send_Answer_toNPC(_uint iTextureNum) {};

protected:
	virtual HRESULT Ready_Components(void* pArg = nullptr) = 0;
	virtual HRESULT SetUp_ShaderResources() { return S_OK; };
	virtual HRESULT SetUp_ShaderID() { return S_OK; };
	virtual void Change_Animation(_float fTimeDelta) {};


protected:
	_vector Calculate_PosDirction();
	virtual void AI_Behaviour(_float fTimeDelta) { };
	virtual void Find_Target();
	virtual void Follow_Target(_float fTimeDelta) { };
	

protected:
	CNavigation*			m_pNavigationCom = nullptr;
	CBaseObj*				m_pTarget = nullptr;
	_bool					m_bAggro = false;
	_float					m_fDistanceToTarget = 0.f;
	CModel*					m_pModelCom = nullptr;
	_float					m_fAnimSpeed = 1.f;


	NPCDESC					m_NpcDesc;

public:
	virtual void Free() override;
};
END
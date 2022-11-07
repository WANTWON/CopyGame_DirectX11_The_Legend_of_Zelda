#pragma once

#include "BaseObj.h"
BEGIN(Engine)
class CModel;
END


BEGIN(Client)

class CDgnKey final : public CBaseObj
{
public:
	enum KEY_TYPE { SMALL_KEY, TAILCAVE_KEY };

	typedef struct KeyTag
	{
		KEY_TYPE eType = SMALL_KEY;
		_float3 vPosition = _float3(0.f, 0.f, 0.f);

	}DGNKEYDESC;


private:
	CDgnKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDgnKey(const CDgnKey& rhs);
	virtual ~CDgnKey() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual int Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CModel*					m_pModelCom = nullptr;
	DGNKEYDESC				m_eKeyDesc;


private:
	virtual HRESULT Ready_Components(void* pArg) override;
	virtual HRESULT SetUp_ShaderResources() override; /* 셰이더 전역변수에 값을 전달한다. */
	virtual HRESULT SetUp_ShaderID() override;

public:
	static CDgnKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
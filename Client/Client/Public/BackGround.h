#pragma once
#include "Obj_UI.h"

BEGIN(Client)

class CBackGround final : public CObj_UI
{
public:
	enum VISIBLESCENE { VISIBLE_LOGO, VISIBLE_LOADING, VISIBLE_PLAYGAME };

	typedef struct backgroundTag
	{
		_tchar* pTextureTag = nullptr;
		VISIBLESCENE eVisibleScreen = VISIBLE_PLAYGAME;
	}BACKGROUNDESC;

private:
	CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBackGround(const CBackGround& rhs);
	virtual ~CBackGround() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual int Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	virtual HRESULT Ready_Components(void * pArg = nullptr) override;
	virtual HRESULT SetUp_ShaderResources()override; /* 셰이더 전역변수에 값을 전달한다. */
	virtual HRESULT SetUp_ShaderID() override;

private:
	BACKGROUNDESC m_BackgroundDesc;

public:
	static CBackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
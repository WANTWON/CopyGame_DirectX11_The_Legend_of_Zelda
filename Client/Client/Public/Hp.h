#pragma once
#include "Obj_UI.h"

BEGIN(Client)

class CHp final : public CObj_UI
{
public:
	enum HP_STATE { HP100, HP75, HP50, HP25, HP0 };

private:
	CHp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHp(const CHp& rhs);
	virtual ~CHp() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual int Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void Set_TextureNum(_uint iNum) { m_iTextureNum = iNum; }
	_uint Get_TextureNum() { return m_iTextureNum; }

private:
	_uint m_iTextureNum = 0;

private:
	virtual HRESULT Ready_Components(void * pArg = nullptr) override;
	virtual HRESULT SetUp_ShaderResources()override;  /* 셰이더 전역변수에 값을 전달한다. */

public:
	static CHp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
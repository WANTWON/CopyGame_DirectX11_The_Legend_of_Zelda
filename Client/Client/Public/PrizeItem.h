#pragma once
#include "BaseObj.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
END



BEGIN(Client)

class CPrizeItem final : public CBaseObj
{

private:
	CPrizeItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPrizeItem(const CPrizeItem& rhs);
	virtual ~CPrizeItem() = default;


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
	CTexture*			m_pTextureCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;

private:
	_uint m_iTextureNum = 0;

public:
	static CPrizeItem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
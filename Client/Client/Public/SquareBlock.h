#pragma once

#include "NonAnim.h"


BEGIN(Client)

class CSquareBlock final : public CNonAnim
{
public:
	enum TYPE {SQUARE_BLOCK , LOCK_BLOCK};

	typedef struct BlockTag
	{
		_float3 vInitPosition = _float3(0.f, 0.f, 0.f);
		TYPE	eType = SQUARE_BLOCK;
	}BLOCKDESC;

private:
	CSquareBlock(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSquareBlock(const CSquareBlock& rhs);
	virtual ~CSquareBlock() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual int Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	virtual HRESULT Ready_Components(void* pArg) override;
	void Tick_SquareBlock(_float fTimeDelta);
	void Tick_LockBlock(_float fTimeDelta);

private:
	BLOCKDESC m_BlockDesc;

public:
	static CSquareBlock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
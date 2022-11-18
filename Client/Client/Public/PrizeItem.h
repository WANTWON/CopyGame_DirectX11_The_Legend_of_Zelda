#pragma once

#include "NonAnim.h"



BEGIN(Client)

class CPrizeItem final : public CNonAnim
{
public:
	enum TYPE { SMALL_KEY, COMPASS, MAP, FEATHER, BOSS_KEY, HEART,  RUBY, CELLO };

	typedef struct KeyTag
	{
		TYPE eType = SMALL_KEY;
		_float3 vPosition = _float3(0.f, 0.f, 0.f);
		_bool m_bPrize = false;
		
	}ITEMDESC;


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
	ITEMDESC				m_ItemDesc;
	_bool					m_bGet = false;

private:
	virtual HRESULT Ready_Components(void* pArg) override;

public:
	static CPrizeItem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
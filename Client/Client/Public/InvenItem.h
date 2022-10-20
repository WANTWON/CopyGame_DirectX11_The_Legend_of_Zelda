#pragma once
#include "Obj_UI.h"

BEGIN(Client)

class CInvenItem final : public CObj_UI
{
public:
	enum ITEM_STATE { ITEM_EQUIP, ITEM_USABLE};
	enum ITEM_USAGE { WEAPON, SHIELD, CLOTHES, RING, USAGE_END };

	enum EQUIP_TEXLIST { EQUIP_NONE, EQUIP_SWORD, EQUIP_SHIELD, EQUIP_CLOTHES, EQUIP_RING};
	enum ITEM_TEXLIST { ITEM_NONE, ITEM_POWDER, ITEM_WAND, ITEM_FEATHER, ITEM_BOMB, ITEM_BOW, ITEM_BOOMERANG };

	typedef struct Itemtag
	{
		ITEM_STATE eItemType = ITEM_USABLE;
		ITEM_USAGE eItemUsage = USAGE_END;
		_float2 vPosition = _float2(0.f, 0.f);

	}ITEMDESC;

private:
	CInvenItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInvenItem(const CInvenItem& rhs);
	virtual ~CInvenItem() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual int Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void Set_TextureNum(_uint iNum) { m_iTextureNum = iNum; }
	void Set_bShow(_bool tyoe) { m_bShow = tyoe; }
	_uint Get_TextureNum() { return m_iTextureNum; }

private:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT SetUp_ShaderResources()override;  /* 셰이더 전역변수에 값을 전달한다. */

private:
	ITEMDESC  m_ItemDesc;
	_bool m_bEmpty = true;
	_bool m_bShow = true;
	_uint m_iTextureNum = 0;

public:
	static CInvenItem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
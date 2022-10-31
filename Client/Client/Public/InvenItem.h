#pragma once
#include "Obj_UI.h"

BEGIN(Client)

class CInvenItem final : public CObj_UI
{
public:
	enum ITEM_STATE { ITEM_EQUIP, ITEM_USABLE, ITEM_COLLECT };
	
	enum EQUIP_TEXLIST { EQUIP_NONE, EQUIP_SWORD, EQUIP_SHIELD, EQUIP_CLOTHES, EQUIP_BELT, EQUIP_SHOES, EQUIP_FLIPPER, EQUIP_SWORD2};
	enum ITEM_TEXLIST { ITEM_NONE, ITEM_POWDER, ITEM_WAND, ITEM_FEATHER, ITEM_BOMB, ITEM_BOW, ITEM_BOOMERANG };
	enum COLLECT_TEXLIST {COLLECT_NONE, };
	enum DGNKEY_TEXLIST {};

	typedef struct Itemtag
	{
		ITEM_STATE eItemType = ITEM_USABLE;
		_uint m_iTextureNum = 0;
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
	void Set_ItemSize(_float2 fSize) { m_fSize = fSize; }
	void Set_TextureNum(_uint iNum) { m_ItemDesc.m_iTextureNum = iNum; }
	void Set_bShow(_bool tyoe) { m_bShow = tyoe; }
	_uint Get_TextureNum() { return m_ItemDesc.m_iTextureNum; }

private:
	virtual HRESULT Ready_Components(void * pArg = nullptr) override;
	virtual HRESULT SetUp_ShaderResources()override;  /* 셰이더 전역변수에 값을 전달한다. */

private:
	ITEMDESC  m_ItemDesc;
	_bool m_bEmpty = true;
	_bool m_bShow = true;

public:
	static CInvenItem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
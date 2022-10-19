#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CObj_UI;
class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)
public:
	enum UITYPE { UI_INVEN, UI_MAP, UI_OPTION, UI_END};
	enum EQUIP_BT { EQUIP_X, EQUIP_Y, EQUIP_END };

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	void Tick();

public:
	void Set_UI_Open();
	void Set_EquipItem(EQUIP_BT eEquipBt, CObj_UI* pObj);
	_bool Get_UI_Open() { return m_bUIOpen; }
	CObj_UI* Get_EquipItem(EQUIP_BT eEquipBt) { return m_EquipTile[eEquipBt];}

public:
	void Add_InvenGroup(CObj_UI* pObj) { m_InvenTile.push_back(pObj);  }

private:
	_bool m_bUIOpen = false;
	
	/*For Inventory*/
	_int m_iPickedIndex = 0;
	CObj_UI* m_pPickedTile = nullptr;
	CObj_UI* m_EquipTile[EQUIP_END] = { nullptr };
	vector<CObj_UI*> m_InvenTile;


public:
	void Free() override;
};

END
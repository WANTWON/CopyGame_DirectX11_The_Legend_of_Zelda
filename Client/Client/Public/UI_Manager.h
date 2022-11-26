#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CObj_UI;
class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)
public:
	enum MESSAGETYPE { DGN_KEY, COMPOSS, DGN_MAP, FEATHER, BOSS_KEY, MSG_HEART, MGS_RUBY, CELLO, MSG_END };
	enum UITYPE { UI_INVEN, UI_MAP, UI_OPTION, UI_END};
	enum EQUIP_BT { EQUIP_X, EQUIP_Y, EQUIP_END };

	enum ROOMTYPE {MARINHOUSE, SHOP};

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	void Initialize_PlayerState();
	void Tick_UI();
	void Tick_Inventory();
	void Tick_PlayerState();
	void Tick_Coin();

	
public: /*Getter Setter*/
	void		Set_UI_Open();
	void		Set_EquipItem(EQUIP_BT eEquipBt, CObj_UI* pObj);
	void		Set_NextLevelIndex(LEVEL eLevel) { m_eNextLevel = eLevel; }
	void		Set_NextLevel(_bool type) { m_bNextLevel = type; m_bFinishedReady = false; }
	void		Set_NextLevelFinished(_bool type) { m_bFinishedReady = type; }
	void		Set_RoomType(ROOMTYPE eType) { m_eRoomType = eType; }

	void		Get_Key();
	_uint		Get_KeySize() { return (_uint)m_KeyList.size(); }
	CObj_UI*	Get_Button() { return m_pButton; }
	_bool		Get_UI_Open() { return m_bUIOpen; }
	_bool		Get_OpenMessage() { return m_bOpenMessage; }

	LEVEL		Get_NextLevelIndex() { return m_eNextLevel; }
	_bool		Get_NextLevel() { return m_bNextLevel; }
	_bool		Get_NextLevelFinished() { return m_bFinishedReady; }
	ROOMTYPE	Get_RoomType() { return m_eRoomType; }

	CObj_UI*	Get_EquipItem(EQUIP_BT eEquipBt) { return m_EquipTile[eEquipBt];}
	MESSAGETYPE Get_MessageType() { return m_eMessageType; }

public:
	void Add_Button(CObj_UI* pObj) { m_pButton = pObj; }
	void Add_InvenGroup(CObj_UI* pObj) { m_InvenTile.push_back(pObj);  }
	void Add_HpGroup(CObj_UI* pObj) { m_HpList.push_back(pObj); }
	void Add_KeyGroup(CObj_UI* pObj) { m_KeyList.push_back(pObj); }
	void Add_RubyGroup(CObj_UI* pObj) { m_RubyList.push_back(pObj); }
	void Use_Key();
	void Open_Message(MESSAGETYPE eType) { m_bOpenMessage = true; m_eMessageType = eType; }
	void Close_Message() {m_bOpenMessage = false;}

private:
	LEVEL m_eNextLevel = LEVEL_GAMEPLAY;
	ROOMTYPE m_eRoomType = MARINHOUSE;
	_bool m_bUIOpen = false;
	_bool m_bOpenMessage = false;
	_bool m_bNextLevel = false;
	_bool m_bFinishedReady = false;
	MESSAGETYPE m_eMessageType = DGN_KEY;
	
	/*For Inventory*/
	_int m_iPickedIndex = 0;
	CObj_UI* m_pPickedTile = nullptr;
	CObj_UI* m_EquipTile[EQUIP_END] = { nullptr };
	vector<CObj_UI*> m_InvenTile;

	/*For Player State*/
	vector<CObj_UI*> m_HpList;
	vector<CObj_UI*> m_KeyList;
	vector<CObj_UI*> m_RubyList;
	_int			 m_iRubyShareSize = 0;

	/*For Button */
	CObj_UI* m_pButton = nullptr;

public:
	void Free() override;
};

END
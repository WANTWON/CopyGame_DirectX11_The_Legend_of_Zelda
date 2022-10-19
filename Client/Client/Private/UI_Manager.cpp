#include "stdafx.h"
#include "..\Public\UI_Manager.h"
#include "Obj_UI.h"
#include "GameInstance.h"
#include "InvenTile.h"

IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
{
}

void CUI_Manager::Tick()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Up(DIK_RIGHT))
	{
		m_iPickedIndex++;

		if (m_iPickedIndex >= m_InvenTile.size())
			m_iPickedIndex = 0;
	}
		

	if (pGameInstance->Key_Up(DIK_LEFT))
	{
		m_iPickedIndex--;

		if (m_iPickedIndex < 0)
			m_iPickedIndex = m_InvenTile.size() -1 ;
	}
	

	if (pGameInstance->Key_Up(DIK_DOWN))
	{
		m_iPickedIndex += 4;

		if (m_iPickedIndex >= m_InvenTile.size())
			m_iPickedIndex %= 4;
	}

	if (pGameInstance->Key_Up(DIK_UP))
	{
		m_iPickedIndex -= 4;

		if (m_iPickedIndex < 0)
			m_iPickedIndex += 12;
	}
		
	dynamic_cast<CInvenTile*>(m_pPickedTile)->Set_SelectTile(false);
	m_pPickedTile = m_InvenTile[m_iPickedIndex];
	dynamic_cast<CInvenTile*>(m_pPickedTile)->Set_SelectTile(true);


	if (pGameInstance->Key_Up(DIK_X))
	{
		if(m_EquipTile[EQUIP_X] != nullptr)
			dynamic_cast<CInvenTile*>(m_EquipTile[EQUIP_X])->Set_TileState(CInvenTile::STATE_DEFAULT);

		m_EquipTile[EQUIP_X] = m_pPickedTile;
		dynamic_cast<CInvenTile*>(m_EquipTile[EQUIP_X])->Set_TileState(CInvenTile::STATE_EQUIP);
	}
		

	if (pGameInstance->Key_Up(DIK_Y))
	{
		if (m_EquipTile[EQUIP_Y] != nullptr)
			dynamic_cast<CInvenTile*>(m_EquipTile[EQUIP_Y])->Set_TileState(CInvenTile::STATE_DEFAULT);

		m_EquipTile[EQUIP_Y] = m_pPickedTile;
		dynamic_cast<CInvenTile*>(m_EquipTile[EQUIP_Y])->Set_TileState(CInvenTile::STATE_EQUIP);
	}
		

	RELEASE_INSTANCE(CGameInstance);
}

void CUI_Manager::Set_UI_Open()
{
	m_bUIOpen = !m_bUIOpen;

	for(_uint i =0; i < m_InvenTile.size() ; ++i)
		dynamic_cast<CInvenTile*>(m_InvenTile[i])->Set_SelectTile(false);

	m_pPickedTile = m_InvenTile[0];
	dynamic_cast<CInvenTile*>( m_pPickedTile)->Set_SelectTile(true);
}

void CUI_Manager::Set_EquipItem(EQUIP_BT eEquipBt, CObj_UI * pObj)
{
	m_EquipTile[eEquipBt] = pObj;
}


void CUI_Manager::Free()
{
}

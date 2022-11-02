#include "stdafx.h"
#include "..\Public\UI_Manager.h"
#include "Obj_UI.h"
#include "GameInstance.h"
#include "InvenTile.h"
#include "Player.h"
#include "Hp.h"

IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
{
}

void CUI_Manager::Initialize_PlayerState()
{

}

void CUI_Manager::Tick_Inventory()
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
			m_iPickedIndex = (int)m_InvenTile.size() -1 ;
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

void CUI_Manager::Tick_PlayerState()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(CGameInstance::Get_Instance()->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")));

	_int iMaxHp = (pPlayer->Get_Info().iMaxHp) / 4;
	if (iMaxHp > m_HpList.size())
	{
		for (auto& iter : m_HpList)
			Safe_Release(iter);
		m_HpList.clear();

	
		if ((pPlayer->Get_Info().iMaxHp) % 4 != 0)
			iMaxHp++;
		int iCol = 0;
		for (int i = 0; i < iMaxHp; ++i)
		{
			_float2 vPostion = _float2(30.f + (i % 8) * 30.f, 30.f + iCol * 20.f);

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Hp"), LEVEL_STATIC, TEXT("Layer_Hp"), &vPostion)))
				return;

			if (i % 8 == 7)
				++iCol;
		}
		
		

	}

	_int iCurrentFullHp = (pPlayer->Get_Info().iCurrentHp) / 4;
	_int iCurrentLastHp = (pPlayer->Get_Info().iCurrentHp) % 4;

	for (int i = 0; i < iCurrentFullHp; ++i)
		dynamic_cast<CHp*>(m_HpList[i])->Set_TextureNum(CHp::HP100);
	
	if(iCurrentLastHp == 0)
		dynamic_cast<CHp*>(m_HpList[iCurrentFullHp])->Set_TextureNum(CHp::HP0);
	if (iCurrentLastHp == 1)
		dynamic_cast<CHp*>(m_HpList[iCurrentFullHp])->Set_TextureNum(CHp::HP25);
	if (iCurrentLastHp == 2)
		dynamic_cast<CHp*>(m_HpList[iCurrentFullHp])->Set_TextureNum(CHp::HP50);
	if (iCurrentLastHp == 3)
		dynamic_cast<CHp*>(m_HpList[iCurrentFullHp])->Set_TextureNum(CHp::HP75);

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

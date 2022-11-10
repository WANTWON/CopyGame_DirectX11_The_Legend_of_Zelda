#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "GameInstance.h"
#include "Camera_Dynamic.h"
#include "UI_Manager.h"
#include "InvenTile.h"
#include "UIButton.h"
#include "InvenItem.h"
#include "BackGround.h"
#include "NonAnim.h"
#include "Player.h"
#include "Level_Loading.h"
#include "CameraManager.h"

_bool g_bUIMadefirst = false;

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
	, m_pCollision_Manager(CCollision_Manager::Get_Instance())
{
	Safe_AddRef(m_pCollision_Manager);
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	//	return E_FAIL;	

	if (g_bUIMadefirst == false)
	{
		if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
			return E_FAIL;

		CUI_Manager::Get_Instance()->Initialize_PlayerState();
		g_bUIMadefirst = true;
	}
	
	CCameraManager::Get_Instance()->Ready_Camera(LEVEL::LEVEL_GAMEPLAY);
	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);	

	CUI_Manager::Get_Instance()->Tick_UI();

	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		m_pCollision_Manager->Clear_CollisionGroup(CCollision_Manager::COLLISION_MONSTER);
		pGameInstance->Set_DestinationLevel(LEVEL_TAILCAVE);
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_TAILCAVE))))
			return;

		Safe_Release(pGameInstance);
	}


}

void CLevel_GamePlay::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	//SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
	SetWindowText(g_hWnd, TEXT("GamePlaye Level."));

	m_pCollision_Manager->Update_Collider();
	m_pCollision_Manager->CollisionwithBullet();
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;

	/* For.Directional*/
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);	

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	///* For.Point */
	//ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	//LightDesc.eType = LIGHTDESC::TYPE_POINT;
	//LightDesc.vPosition = _float4(10.f, 3.f, 10.f, 1.f);
	//LightDesc.fRange = 7.f;	
	//LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	//LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	//if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_STATIC, pLayerTag, nullptr)))
		return E_FAIL;	


	if (pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")) == nullptr)
	{
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_STATIC, pLayerTag, nullptr)))
			return E_FAIL;
	}
	else
	{
		CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));

		LEVEL ePastLevel = (LEVEL)CLevel_Manager::Get_Instance()->Get_PastLevelIndex();
		switch (ePastLevel)
		{
		case Client::LEVEL_TAILCAVE:
			pPlayer->Set_State(CTransform::STATE_POSITION, XMVectorSet(10.f, 4.2f, 10.f, 1.f));
			break;
		}


	}


	Safe_Release(pGameInstance);

	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	LEVEL eLevel = LEVEL_GAMEPLAY;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Terrain"), LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), &eLevel)))
		return E_FAIL;

	HANDLE hFile = 0;
	_ulong dwByte = 0;
	CNonAnim::NONANIMDESC  ModelDesc;
	_uint iNum = 0;

	hFile = CreateFile(TEXT("../../../Bin/Data/Filed_Map.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	/* 타일의 개수 받아오기 */
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(ModelDesc), sizeof(CNonAnim::NONANIMDESC), &dwByte, nullptr);
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_NonAnim"), LEVEL_GAMEPLAY, pLayerTag, &ModelDesc)))
			return E_FAIL;
	}

	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	for (_uint i = 0; i < 20; ++i)
	{
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Effect"), LEVEL_GAMEPLAY, pLayerTag, nullptr)))
			return E_FAIL;
	}	

	Safe_Release(pGameInstance);
	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CCamera_Dynamic::CAMERADESC_DERIVED				CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera_Dynamic::CAMERADESC_DERIVED));

	CameraDesc.iTest = 10;

	CameraDesc.CameraDesc.vEye = _float4(0.f, 8.0f, -8.f, 1.f);
	CameraDesc.CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

	CameraDesc.CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.CameraDesc.fNear = 0.2f;
	CameraDesc.CameraDesc.fFar = 500.f;

	CameraDesc.CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_GAMEPLAY, pLayerTag, &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	//if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI"), LEVEL_GAMEPLAY, pLayerTag)))
		//return E_FAIL;

	CBackGround::BACKGROUNDESC BackgroundDesc;
	BackgroundDesc.eVisibleScreen = CBackGround::VISIBLE_PLAYGAME;
	BackgroundDesc.pTextureTag = TEXT("Prototype_Component_Texture_InventoryBackGround_UI");
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BackGround_UI"), LEVEL_STATIC, pLayerTag, 
		&BackgroundDesc)))
		return E_FAIL;


	CInvenTile::INVENDESC InvenDesc;
	InvenDesc.eTileType = CInvenTile::EQUIP_TILE;
	InvenDesc.eState = CInvenTile::STATE_EQUIP;
	InvenDesc.eEquipKey = CInvenTile::EQUIP_X;
	InvenDesc.vPosition = _float2(1100, 50);

	CUIButton::BUTTONDESC ButtonDesc;
	ButtonDesc.eButtonType = CUIButton::BTN_FIX;
	ButtonDesc.eColor = CUIButton::BTN_BLACK;
	ButtonDesc.eState = CUIButton::BTN_X;
	ButtonDesc.vPosition = _float2(InvenDesc.vPosition.x - 20, InvenDesc.vPosition.y + 20);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_InvenTile_UI"), LEVEL_STATIC, pLayerTag, &InvenDesc)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CUIButton"), LEVEL_STATIC, pLayerTag, &ButtonDesc)))
		return E_FAIL;

	InvenDesc.vPosition = _float2(1200, 100);
	InvenDesc.eEquipKey = CInvenTile::EQUIP_Y;
	ButtonDesc.eState = CUIButton::BTN_Y;
	ButtonDesc.vPosition = _float2(InvenDesc.vPosition.x - 20, InvenDesc.vPosition.y + 20);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_InvenTile_UI"), LEVEL_STATIC, pLayerTag, &InvenDesc)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CUIButton"), LEVEL_STATIC, pLayerTag, &ButtonDesc)))
		return E_FAIL;
	

	ButtonDesc.eButtonType = CUIButton::BTN_OPEN;
	ButtonDesc.eState = CUIButton::BTN_A;
	ButtonDesc.vPosition = _float2(0, 0);
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CUIButton"), LEVEL_STATIC, pLayerTag, &ButtonDesc)))
		return E_FAIL;



	for (_int i = 0; i < 3; ++i)
	{
		for (_int j = 0; j < 4; ++j)
		{			

			CInvenTile::INVENDESC InvenDesc;
			InvenDesc.eTileType = CInvenTile::INVEN_TILE;
			InvenDesc.eState = CInvenTile::STATE_DEFAULT;
			InvenDesc.eEquipKey = CInvenTile::EQUIP_NONE;
			InvenDesc.vPosition = _float2(_float(780 + j*110), _float(260 + i*120));

			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_InvenTile_UI"), LEVEL_STATIC, pLayerTag, &InvenDesc)))
				return E_FAIL;
		}
	}

	
	ButtonDesc.eButtonType = CUIButton::BTN_INVEN;
	ButtonDesc.eColor = CUIButton::BTN_GREEN;
	ButtonDesc.eState = CUIButton::BTN_X;
	ButtonDesc.vPosition = _float2(780 - 30, 260 + 30);
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CUIButton"), LEVEL_STATIC, pLayerTag, &ButtonDesc)))
		return E_FAIL;

	ButtonDesc.eState = CUIButton::BTN_Y;
	ButtonDesc.vPosition = _float2(890 - 30, 260 + 30);
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CUIButton"), LEVEL_STATIC, pLayerTag, &ButtonDesc)))
		return E_FAIL;


	for (int i = 0; i < 5; ++i)
	{
		CInvenItem::ITEMDESC ItemDesc;
		ItemDesc.eItemType = CInvenItem::ITEM_EQUIP;
		ItemDesc.m_iTextureNum = (CInvenItem::EQUIP_TEXLIST)(i+1);
		ItemDesc.vPosition = _float2(165.f, 245.f + 70*i);

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CInvenItem"), LEVEL_STATIC, TEXT("Layer_EquipItem"), &ItemDesc)))
			return E_FAIL;
	}

	for (int i = 0; i < 5; ++i)
	{
		CInvenItem::ITEMDESC ItemDesc;
		ItemDesc.eItemType = CInvenItem::ITEM_QUEST;
		ItemDesc.m_iTextureNum = (CInvenItem::PRIZE_TEXLIST)(0);
		ItemDesc.vPosition = _float2(645.f, 245.f + 70 * i);

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CInvenItem"), LEVEL_STATIC, TEXT("Layer_EquipItem"), &ItemDesc)))
			return E_FAIL;
	}


	for (int i = 0; i < 5; ++i)
	{
		CInvenItem::ITEMDESC ItemDesc;
		ItemDesc.eItemType = CInvenItem::ITEM_DGNKEY;
		ItemDesc.m_iTextureNum = (CInvenItem::EQUIP_TEXLIST)(i + 1);
		ItemDesc.vPosition = _float2(285.f + 55 * i, 545.f);

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CInvenItem"), LEVEL_STATIC, TEXT("Layer_DongeonKey"), &ItemDesc)))
			return E_FAIL;
	}

	for (int i = 0; i < 4; ++i)
	{
		_float2 vPosition = _float2(0.f, 0.f);
		
		vPosition.x = (i == 1 || i == 2) ?  400 + 95.f : 400 + 40.f;

		vPosition.y = 230.f + i * 70.f;

		CInvenItem::ITEMDESC ItemDesc;
		ItemDesc.eItemType = CInvenItem::ITEM_COLLECT;
		ItemDesc.m_iTextureNum = (CInvenItem::EQUIP_TEXLIST)(i*2 + 1);
		ItemDesc.vPosition = vPosition;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CInvenItem"), LEVEL_STATIC, TEXT("Layer_Collect"), &ItemDesc)))
			return E_FAIL;

		vPosition.x = (i == 1 || i == 2) ? 400 - 95.f : 400 - 40.f;
		ItemDesc.m_iTextureNum = (CInvenItem::EQUIP_TEXLIST)(i*2 + 2);
		ItemDesc.vPosition = vPosition;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CInvenItem"), LEVEL_STATIC, TEXT("Layer_Collect"), &ItemDesc)))
			return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MessageBox"), LEVEL_STATIC, TEXT("Layer_UI"), nullptr)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	HANDLE hFile = 0;
	_ulong dwByte = 0;
	CNonAnim::NONANIMDESC  ModelDesc;
	_uint iNum = 0;

	hFile = CreateFile(TEXT("../../../Bin/Data/Filed_Monster.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	/* 타일의 개수 받아오기 */
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(ModelDesc), sizeof(CNonAnim::NONANIMDESC), &dwByte, nullptr);

		_tchar pModeltag[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, ModelDesc.pModeltag, MAX_PATH, pModeltag, MAX_PATH);
		if (!wcscmp(pModeltag, TEXT("Octorock.fbx")))
		{
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Octorock"), LEVEL_GAMEPLAY, pLayerTag, &ModelDesc.vPosition)))
				return E_FAIL;
		}
		else if (!wcscmp(pModeltag, TEXT("MoblinSword.fbx")))
		{
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_MoblinSword"), LEVEL_GAMEPLAY, pLayerTag, &ModelDesc.vPosition)))
				return E_FAIL;
		}

	}

	CloseHandle(hFile);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay*	pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

	Safe_Release(m_pCollision_Manager);

}

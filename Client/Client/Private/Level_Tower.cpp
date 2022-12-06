#include "stdafx.h"
#include "..\Public\Level_Tower.h"

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
#include "Door.h"
#include "TreasureBox.h"
#include "SquareBlock.h"
#include "Grass.h"
#include "Portal.h"


CLevel_Tower::CLevel_Tower(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
	, m_pCollision_Manager(CCollision_Manager::Get_Instance())
{
	Safe_AddRef(m_pCollision_Manager);
}

HRESULT CLevel_Tower::Initialize()
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

	/*if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Object(TEXT("Layer_Object"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Portal(TEXT("Layer_Portal"))))
		return E_FAIL;*/

	//if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	//	return E_FAIL;	

	
	CCameraManager* pCameraManager = CCameraManager::Get_Instance();

	pCameraManager->Ready_Camera(LEVEL::LEVEL_TOWER);
	CCamera* pCamera = pCameraManager->Get_CurrentCamera();
	dynamic_cast<CCamera_Dynamic*>(pCamera)->Set_CamMode(CCamera_Dynamic::CAM_PLAYER);
	CUI_Manager::Get_Instance()->Set_NextLevel(false);
	return S_OK;
}

void CLevel_Tower::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);	

	CUI_Manager::Get_Instance()->Tick_UI();


	if (CUI_Manager::Get_Instance()->Get_NextLevelFinished())
	{
		CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		m_pCollision_Manager->Clear_CollisionGroup(CCollision_Manager::COLLISION_MONSTER);
		pGameInstance->Set_DestinationLevel(LEVEL_GAMEPLAY);
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_TAILCAVE))))
			return;
		Safe_Release(pGameInstance);
	}

}

void CLevel_Tower::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	//SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
	SetWindowText(g_hWnd, TEXT("GamePlaye Level."));

	m_pCollision_Manager->Update_Collider();
	m_pCollision_Manager->CollisionwithBullet();
}

HRESULT CLevel_Tower::Ready_Lights()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;

	/* For.Directional*/
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 0.7f, 1.f);
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

HRESULT CLevel_Tower::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player")) == nullptr)
	{
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_STATIC, pLayerTag, nullptr)))
			return E_FAIL;
	}
	else
	{
		CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
		pPlayer->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 5.4f, -3.76, 1.f));
		pPlayer->Change_Navigation(LEVEL_TOWER);
		pPlayer->Set_JumpingHeight(0.1f);
		pPlayer->Compute_CurrentIndex(LEVEL_TOWER);
		pPlayer->Set_2DMode(true);
	}

	Safe_Release(pGameInstance);

	
	return S_OK;
}

HRESULT CLevel_Tower::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	LEVEL eLevel = LEVEL_TOWER;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sky"), eLevel, pLayerTag, nullptr)))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Terrain"), eLevel, TEXT("Layer_Terrain"), &eLevel)))
		return E_FAIL;

	CNonAnim::NONANIMDESC  ModelDesc;
	ModelDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ModelDesc.vScale = _float3(3.f, 3.f, 3.f);
	strcpy(ModelDesc.pModeltag, "Tower.fbx");
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_NonAnim"), eLevel, pLayerTag, &ModelDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Tower::Ready_Layer_Effect(const _tchar * pLayerTag)
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

HRESULT CLevel_Tower::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CCamera_Dynamic::CAMERADESC_DERIVED				CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera_Dynamic::CAMERADESC_DERIVED));

	CameraDesc.iTest = 10;

	CameraDesc.CameraDesc.vEye = _float4(0.f, 2.0f, -12.f, 1.f);
	CameraDesc.CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

	CameraDesc.CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.CameraDesc.fNear = 0.2f;
	CameraDesc.CameraDesc.fFar = 500.f;

	CameraDesc.CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_TOWER, pLayerTag, &CameraDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Tower::Ready_Layer_Monster(const _tchar * pLayerTag)
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

HRESULT CLevel_Tower::Ready_Layer_Object(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	HANDLE hFile = 0;
	_ulong dwByte = 0;
	CNonAnim::NONANIMDESC  ModelDesc;
	_uint iNum = 0;

	hFile = CreateFile(TEXT("../../../Bin/Data/Filed_Object.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	/* 타일의 개수 받아오기 */
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(ModelDesc), sizeof(CNonAnim::NONANIMDESC), &dwByte, nullptr);

		_tchar pModeltag[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, ModelDesc.pModeltag, MAX_PATH, pModeltag, MAX_PATH);
		if (!wcscmp(pModeltag, TEXT("TailCaveShutter.fbx")))
		{

			CDoor::DOORDESC DoorDesc;
			DoorDesc.eType = CDoor::DOOR_TAIL;
			DoorDesc.InitPosition = ModelDesc.vPosition;
			DoorDesc.fAngle = ModelDesc.m_fAngle;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Door"), LEVEL_GAMEPLAY, TEXT("Layer_Shutter"), &DoorDesc)))
				return E_FAIL;

		}
		else if (!wcscmp(pModeltag, TEXT("TailCaveStatue.fbx")))
		{

			CDoor::DOORDESC DoorDesc;
			CSquareBlock::BLOCKDESC BlockDesc;
			BlockDesc.eType = CSquareBlock::TAIL_STATUE;
			BlockDesc.vInitPosition = ModelDesc.vPosition;
			BlockDesc.fAngle = ModelDesc.m_fAngle;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_SquareBlock"), LEVEL_GAMEPLAY, pLayerTag, &BlockDesc)))
				return E_FAIL;

		}
		else if (!wcscmp(pModeltag, TEXT("Lawn.fbx")))
		{

			CGrass::GRASSDESC GrassDesc;
			GrassDesc.eType = CGrass::LAWN;
			GrassDesc.vPosition = ModelDesc.vPosition;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Grass"), LEVEL_GAMEPLAY, pLayerTag, &GrassDesc)))
				return E_FAIL;

		}
		else if (!wcscmp(pModeltag, TEXT("Grass.fbx")))
		{

			CGrass::GRASSDESC GrassDesc;
			GrassDesc.eType = CGrass::GRASS;
			GrassDesc.vPosition = ModelDesc.vPosition;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Grass"), LEVEL_GAMEPLAY, pLayerTag, &GrassDesc)))
				return E_FAIL;

		}
		else if (!wcscmp(pModeltag, TEXT("Grass2x2.fbx")))
		{

			CGrass::GRASSDESC GrassDesc;
			GrassDesc.eType = CGrass::GRASS2x2;
			GrassDesc.vPosition = ModelDesc.vPosition;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Grass"), LEVEL_GAMEPLAY, pLayerTag, &GrassDesc)))
				return E_FAIL;

		}

	}

	CloseHandle(hFile);


	hFile = 0;
	dwByte = 0;
	CTreasureBox::BOXTAG  TreasureBoxDesc;
	iNum = 0;

	hFile = CreateFile(TEXT("../../../Bin/Data/Field_TreasureBox.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	/* 타일의 개수 받아오기 */
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(TreasureBoxDesc), sizeof(CTreasureBox::BOXTAG), &dwByte, nullptr);

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TreasureBox"), LEVEL_GAMEPLAY, pLayerTag, &TreasureBoxDesc)))
			return E_FAIL;

	}

	CloseHandle(hFile);


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_Tower::Ready_Layer_Portal(const _tchar * pLayerTag)
{
	HANDLE hFile = 0;
	_ulong dwByte = 0;
	CNonAnim::NONANIMDESC  ModelDesc;
	_uint iNum = 0;


	hFile = CreateFile(TEXT("../../../Bin/Data/Field_Portal.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* 타일의 개수 받아오기 */
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);


	ReadFile(hFile, &(ModelDesc), sizeof(CNonAnim::NONANIMDESC), &dwByte, nullptr);
	
	CPortal::PORTALDESC PortalDesc;
	PortalDesc.ePortalType = CPortal::PORTAL_LEVEL;
	PortalDesc.vInitPos = ModelDesc.vPosition;
	PortalDesc.eConnectLevel = LEVEL_TOWER;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Portal"), LEVEL_GAMEPLAY, pLayerTag, &PortalDesc)))
		return E_FAIL;

	ReadFile(hFile, &(ModelDesc), sizeof(CNonAnim::NONANIMDESC), &dwByte, nullptr);

	PortalDesc.ePortalType = CPortal::PORTAL_LEVEL;
	PortalDesc.vInitPos = ModelDesc.vPosition;
	PortalDesc.eConnectLevel = LEVEL_ROOM;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Portal"), LEVEL_GAMEPLAY, pLayerTag, &PortalDesc)))
		return E_FAIL;


	CloseHandle(hFile);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CLevel_Tower * CLevel_Tower::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Tower*	pInstance = new CLevel_Tower(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Created : CLevel_Tower"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Tower::Free()
{
	__super::Free();

	Safe_Release(m_pCollision_Manager);

}

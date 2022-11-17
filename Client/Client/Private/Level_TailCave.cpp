#include "stdafx.h"
#include "..\Public\Level_TailCave.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "CameraManager.h"

#include "UI_Manager.h"
#include "UIButton.h"
#include "InvenTile.h"
#include "InvenItem.h"

#include "BackGround.h"
#include "NonAnim.h"
#include "Player.h"

#include "DgnKey.h"
#include "TreasureBox.h"
#include "TailBoss.h"
#include "Door.h"
#include "Tail.h"
#include "Portal.h"

CLevel_TailCave::CLevel_TailCave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_TailCave::Initialize()
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

	if (FAILED(Ready_Layer_Object(TEXT("Layer_Object"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Portal(TEXT("Layer_Portal"))))
		return E_FAIL;
	
	CCameraManager* pCameraManager = CCameraManager::Get_Instance();

	pCameraManager->Ready_Camera(LEVEL::LEVEL_TAILCAVE);

	CCamera* pCamera = pCameraManager->Get_CurrentCamera();
	dynamic_cast<CCamera_Dynamic*>(pCamera)->Set_CamMode(CCamera_Dynamic::CAM_TERRAIN);
	
	return S_OK;
}

void CLevel_TailCave::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);	

	CUI_Manager::Get_Instance()->Tick_UI();

	Check_Solved_Puzzle();


}

void CLevel_TailCave::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	//SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
	SetWindowText(g_hWnd, TEXT("TailCave Level."));

	CCollision_Manager::Get_Instance()->Update_Collider();
	CCollision_Manager::Get_Instance()->CollisionwithBullet();
}

HRESULT CLevel_TailCave::Ready_Lights()
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

HRESULT CLevel_TailCave::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_Object(LEVEL_STATIC, TEXT("Layer_Player"));
	LEVEL ePastLevel = (LEVEL)CLevel_Manager::Get_Instance()->Get_PastLevelIndex();
	pPlayer->Set_State(CTransform::STATE_POSITION, XMVectorSet(54.f, 0.1f, 2.8f, 1.f));
	pPlayer->Set_JumpingHeight(0.1f);
			
	Safe_Release(pGameInstance);

	
	return S_OK;
}

HRESULT CLevel_TailCave::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);



	LEVEL eLevel = LEVEL_TAILCAVE;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Terrain"), LEVEL_TAILCAVE, TEXT("Layer_Terrain"), &eLevel)))
		return E_FAIL;

	HANDLE hFile = 0;
	_ulong dwByte = 0;
	CNonAnim::NONANIMDESC  ModelDesc;
	_uint iNum = 0;

	hFile = CreateFile(TEXT("../../../Bin/Data/TailCave_Map.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	/* 타일의 개수 받아오기 */
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(ModelDesc), sizeof(CNonAnim::NONANIMDESC), &dwByte, nullptr);
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_NonAnim"), LEVEL_TAILCAVE, pLayerTag, &ModelDesc)))
			return E_FAIL;
	}

	CloseHandle(hFile);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_TailCave::Ready_Layer_Effect(const _tchar * pLayerTag)
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

HRESULT CLevel_TailCave::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

#pragma region Camera Dynamic
	CCamera_Dynamic::CAMERADESC_DERIVED				CameraDesc_Dynamic;
	ZeroMemory(&CameraDesc_Dynamic, sizeof(CCamera_Dynamic::CAMERADESC_DERIVED));

	CameraDesc_Dynamic.iTest = 10;

	CameraDesc_Dynamic.CameraDesc.vEye = _float4(0.f, 11.8f, -3.5f, 1.f);
	CameraDesc_Dynamic.CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc_Dynamic.InitPostion = _float4(54.f, 10.1f, 2.8f, 1.f);

	CameraDesc_Dynamic.CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc_Dynamic.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc_Dynamic.CameraDesc.fNear = 0.2f;
	CameraDesc_Dynamic.CameraDesc.fFar = 500.f;

	CameraDesc_Dynamic.CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc_Dynamic.CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_Dynamic"), LEVEL_TAILCAVE, pLayerTag, &CameraDesc_Dynamic)))
		return E_FAIL;
#pragma endregion Camera Dynamic

#pragma region Camera 2D


	HANDLE hFile = 0;
	_ulong dwByte = 0;
	_float3 vInitPos = _float3(0.f,0.f,0.f);
	_float3 vMaxXPos = _float3(0.f,0.f,0.f);
	_float3 vMinXPos = _float3(0.f,0.f,0.f);
	_uint iNum = 0;

	hFile = CreateFile(TEXT("../../../Bin/Data/TailCave_Camera2D.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	/* 타일의 개수 받아오기 */
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &(vInitPos), sizeof(_float3), &dwByte, nullptr);
	ReadFile(hFile, &(vMaxXPos), sizeof(_float3), &dwByte, nullptr);
	ReadFile(hFile, &(vMinXPos), sizeof(_float3), &dwByte, nullptr);

	CCamera_2D::CAMERADESC_DERIVED				CameraDesc_2D;
	ZeroMemory(&CameraDesc_2D, sizeof(CCamera_2D::CAMERADESC_DERIVED));

	XMStoreFloat4(&CameraDesc_2D.InitPostion, XMVectorSetW(XMLoadFloat3(&vInitPos), 1.f));
	CameraDesc_2D.fMaxXPos = vMaxXPos.x;
	CameraDesc_2D.fMinXPos = vMinXPos.x;

	CameraDesc_2D.CameraDesc.vEye = _float4(0.f, 11.8f, -3.5f, 1.f);
	CameraDesc_2D.CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);

	CameraDesc_2D.CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc_2D.CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc_2D.CameraDesc.fNear = 0.2f;
	CameraDesc_2D.CameraDesc.fFar = 500.f;

	CameraDesc_2D.CameraDesc.TransformDesc.fSpeedPerSec = 3.f;
	CameraDesc_2D.CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Camera_2D"), LEVEL_TAILCAVE, pLayerTag, &CameraDesc_2D)))
		return E_FAIL;

	CloseHandle(hFile);
#pragma endregion Camera 2D

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


HRESULT CLevel_TailCave::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	HANDLE hFile = 0;
	_ulong dwByte = 0;
	CNonAnim::NONANIMDESC  ModelDesc;
	_uint iNum = 0;

	hFile = CreateFile(TEXT("../../../Bin/Data/TailCave_Monster.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	/* 타일의 개수 받아오기 */
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(ModelDesc), sizeof(CNonAnim::NONANIMDESC), &dwByte, nullptr);

		_tchar pModeltag[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, ModelDesc.pModeltag, MAX_PATH, pModeltag, MAX_PATH);
		if (!wcscmp(pModeltag, TEXT("Pawn.fbx")))
		{
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Pawn"), LEVEL_TAILCAVE, pLayerTag, &ModelDesc.vPosition)))
				return E_FAIL;
		}
		else if (!wcscmp(pModeltag, TEXT("Rola.fbx")))
		{
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Rola"), LEVEL_TAILCAVE, pLayerTag, &ModelDesc.vPosition)))
				return E_FAIL;
		}
		else if (!wcscmp(pModeltag, TEXT("BuzzBlob.fbx")))
		{
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BuzzBlob"), LEVEL_TAILCAVE, pLayerTag, &ModelDesc.vPosition)))
				return E_FAIL;
		}
		else if (!wcscmp(pModeltag, TEXT("ZolRed.fbx")))
		{
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_RedZol"), LEVEL_TAILCAVE, pLayerTag, &ModelDesc.vPosition)))
				return E_FAIL;
		}
		else if (!wcscmp(pModeltag, TEXT("Keese.fbx")))
		{
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Keese"), LEVEL_TAILCAVE, pLayerTag, &ModelDesc.vPosition)))
				return E_FAIL;
		}
		else if (!wcscmp(pModeltag, TEXT("Togezo.fbx")))
		{
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Togezo"), LEVEL_TAILCAVE, pLayerTag, &ModelDesc.vPosition)))
				return E_FAIL;
		}
		else if (!wcscmp(pModeltag, TEXT("TailBoss1.fbx")))
		{
			CTailBoss::TAILDESC TailDesc;
			TailDesc.eTailType = CTailBoss::TAIL1;
			TailDesc.InitPostion = XMLoadFloat3(&ModelDesc.vPosition);
			TailDesc.InitPostion = XMVectorSetW(TailDesc.InitPostion, 1.f);
			TailDesc.pParent = nullptr;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TailBoss"), LEVEL_TAILCAVE, pLayerTag, &TailDesc)))
				return E_FAIL;
		}
		else if (!wcscmp(pModeltag, TEXT("Tail1.fbx")))
		{
			CTail::TAILDESC TailDesc;
			TailDesc.eTailType = CTail::TAIL1;
			TailDesc.InitPostion = XMLoadFloat3(&ModelDesc.vPosition);
			TailDesc.InitPostion = XMVectorSetW(TailDesc.InitPostion, 1.f);
			TailDesc.pParent = nullptr;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Tail"), LEVEL_TAILCAVE, pLayerTag, &TailDesc)))
				return E_FAIL;
		}
		

	}

	CloseHandle(hFile);


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_TailCave::Ready_Layer_Object(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTreasureBox::TreasureBoxTag Boxtag;
	Boxtag.eItemType = CTreasureBox::COMPASS;
	Boxtag.vPosition = _float3(15.09f, 0.1f, 6.66f);
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_TreasureBox"), LEVEL_TAILCAVE, pLayerTag, &Boxtag)))
		return E_FAIL;


	HANDLE hFile = 0;
	_ulong dwByte = 0;
	CNonAnim::NONANIMDESC  ModelDesc;
	_uint iNum = 0;

	hFile = CreateFile(TEXT("../../../Bin/Data/TailCave_Object.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	/* 타일의 개수 받아오기 */
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNum; ++i)
	{
		ReadFile(hFile, &(ModelDesc), sizeof(CNonAnim::NONANIMDESC), &dwByte, nullptr);

		_tchar pModeltag[MAX_PATH];
		MultiByteToWideChar(CP_ACP, 0, ModelDesc.pModeltag, MAX_PATH, pModeltag, MAX_PATH);
		if (!wcscmp(pModeltag, TEXT("SmallKey.fbx")))
		{

			CDgnKey::DGNKEYDESC DgnKeyDesc;
			DgnKeyDesc.eType = CDgnKey::SMALL_KEY;
			DgnKeyDesc.vPosition = ModelDesc.vPosition;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DgnKey"), LEVEL_TAILCAVE, pLayerTag, &DgnKeyDesc)))
				return E_FAIL;
		}
		else if (!wcscmp(pModeltag, TEXT("FootSwitch.fbx")))
		{
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_FootSwitch"), LEVEL_TAILCAVE, pLayerTag, &ModelDesc.vPosition)))
				return E_FAIL;
		}
		else if (!wcscmp(pModeltag, TEXT("CollapseTile.fbx")))
		{
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_CollapeTile"), LEVEL_TAILCAVE, pLayerTag, &ModelDesc.vPosition)))
				return E_FAIL;
		}
		else if (!wcscmp(pModeltag, TEXT("SquareBlock.fbx")))
		{
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_SquareBlock"), LEVEL_TAILCAVE, pLayerTag, &ModelDesc.vPosition)))
				return E_FAIL;
		}
		else if (!wcscmp(pModeltag, TEXT("BladeTrap.fbx")))
		{
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BladeTrap"), LEVEL_TAILCAVE, pLayerTag, &ModelDesc.vPosition)))
				return E_FAIL;
		}
		else if (!wcscmp(pModeltag, TEXT("ClosedDoor.fbx")))
		{
			CDoor::DOORDESC DoorDesc;
			DoorDesc.eType = CDoor::DOOR_CLOSED;
			DoorDesc.InitPosition = ModelDesc.vPosition;
			DoorDesc.fAngle = ModelDesc.m_fAngle;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Door"), LEVEL_TAILCAVE, pLayerTag, &DoorDesc)))
				return E_FAIL;
		}
		else if (!wcscmp(pModeltag, TEXT("LockDoor.fbx")))
		{
			CDoor::DOORDESC DoorDesc;
			DoorDesc.eType = CDoor::DOOR_KEY;
			DoorDesc.InitPosition = ModelDesc.vPosition;
			DoorDesc.fAngle = ModelDesc.m_fAngle;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Door"), LEVEL_TAILCAVE, pLayerTag, &DoorDesc)))
				return E_FAIL;
		}

	}

	CloseHandle(hFile);

	
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_TailCave::Ready_Layer_Portal(const _tchar * pLayerTag)
{
	HANDLE hFile = 0;
	_ulong dwByte = 0;
	CNonAnim::NONANIMDESC  ModelDesc1;
	CNonAnim::NONANIMDESC  ModelDesc2;
	_uint iNum = 0;
	
	CPortal::PORTALDESC PortalDesc;


	hFile = CreateFile(TEXT("../../../Bin/Data/TailCave_Portal.dat"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* 타일의 개수 받아오기 */
	ReadFile(hFile, &(iNum), sizeof(_uint), &dwByte, nullptr);

	
		ReadFile(hFile, &(ModelDesc1), sizeof(CNonAnim::NONANIMDESC), &dwByte, nullptr);
		ReadFile(hFile, &(ModelDesc2), sizeof(CNonAnim::NONANIMDESC), &dwByte, nullptr);

		PortalDesc.vInitPos = ModelDesc1.vPosition;
		PortalDesc.vConnectPos = ModelDesc2.vPosition;
		PortalDesc.bConnectPortal2D = true;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Portal"), LEVEL_TAILCAVE, pLayerTag, &PortalDesc)))
			return E_FAIL;

		PortalDesc.vInitPos = ModelDesc2.vPosition;
		PortalDesc.vConnectPos = ModelDesc1.vPosition;
		PortalDesc.bConnectPortal2D = false;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Portal"), LEVEL_TAILCAVE, pLayerTag, &PortalDesc)))
			return E_FAIL;

	
		ReadFile(hFile, &(ModelDesc1), sizeof(CNonAnim::NONANIMDESC), &dwByte, nullptr);
		ReadFile(hFile, &(ModelDesc2), sizeof(CNonAnim::NONANIMDESC), &dwByte, nullptr);

		PortalDesc.vInitPos = ModelDesc1.vPosition;
		PortalDesc.vConnectPos = ModelDesc2.vPosition;
		PortalDesc.bConnectPortal2D = false;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Portal"), LEVEL_TAILCAVE, pLayerTag, &PortalDesc)))
			return E_FAIL;

		PortalDesc.vInitPos = ModelDesc2.vPosition;
		PortalDesc.vConnectPos = ModelDesc1.vPosition;
		PortalDesc.bConnectPortal2D = true;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Portal"), LEVEL_TAILCAVE, pLayerTag, &PortalDesc)))
			return E_FAIL;



	CloseHandle(hFile);
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CLevel_TailCave::Check_Solved_Puzzle()
{
	if (CGameInstance::Get_Instance()->Key_Up(DIK_0))
		m_bDoorOpen = !m_bDoorOpen;
}

CLevel_TailCave * CLevel_TailCave::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_TailCave*	pInstance = new CLevel_TailCave(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		ERR_MSG(TEXT("Failed to Created : CLevel_TailCave"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_TailCave::Free()
{
	__super::Free();


}

#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"
#include "Camera_Dynamic.h"
#include "Camera_2D.h"
#include "Data_Manager.h"
#include "Level_Manager.h"

//for Player
#include "Player.h"
#include "PlayerBullet.h"
#include "Weapon.h"

//for Monster
#include "Octorock.h"
#include "MoblinSword.h"
#include "Rola.h"
#include "MonsterBullet.h"
#include "Pawn.h"
#include "BuzzBlob.h"
#include "TailBoss.h"
#include "RedZol.h"
#include "Keese.h"
#include "Tail.h"
#include "Togezo.h"

//for NPC
#include "ShopNpc.h"
#include "MarinNpc.h"
#include "CraneGameNpc.h"

//for UI
#include "BackGround.h"
#include "InvenTile.h"
#include "UIButton.h"
#include "InvenItem.h"
#include "PlayerState.h"
#include "MessageBox.h"

//for Object 
#include "Terrain.h"
#include "NonAnim.h"
#include "DgnKey.h"
#include "TreasureBox.h"
#include "FootSwitch.h"
#include "CollapseTile.h"
#include "Door.h"
#include "SquareBlock.h"
#include "BladeTrap.h"
#include "Portal.h"
#include "PrizeItem.h"
#include "Grass.h"


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

unsigned int APIENTRY Thread_Main(void* pArg)
{
	CLoader*		pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CriticalSection());

	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_LOGO:
		pLoader->Loading_ForLogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		pLoader->Loading_ForGamePlayLevel();
		break;
	case LEVEL_TAILCAVE:
		pLoader->Loading_ForTailCaveLevel();
		break;
	case LEVEL_ROOM:
		pLoader->Loading_ForRoomLevel();
		break;
	}

	LeaveCriticalSection(&pLoader->Get_CriticalSection());

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevel)
{
	CoInitializeEx(nullptr, 0);

	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;
		
	return S_OK;
}

HRESULT CLoader::Loading_ForLogoLevel()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	/* 텍스쳐 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("UI 텍스쳐 로딩 중."));
	if (FAILED(Loading_For_UITexture()))
		return E_FAIL;

	/* 객체 원형 생성 중. */
	lstrcpy(m_szLoadingText, TEXT("Static data 생성 중."));
	if (FAILED(Loading_ForStaticLevel()))
		return E_FAIL;

	/* 객체 원형 생성 중. */
	lstrcpy(m_szLoadingText, TEXT("객체 생성 중."));
	if (FAILED(Loading_For_ObjectPrototype()))
		return E_FAIL;
	

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForStaticLevel()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr == pGameInstance)
		return E_FAIL;
	CData_Manager* pData_Manager = GET_INSTANCE(CData_Manager);

	/* 텍스쳐 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("텍스쳐 로딩 중."));
	/*For.Prototype_Component_Texture_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2))))
		return E_FAIL;

	/* 모델 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("모델 생성 중."));
	/*For.Prototype_Component_VIBuffer_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 30, 30, -0.01f))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("네비게이션 생성 중."));
	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Field"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Data/Filed_Navi.dat")))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("네비게이션 생성 중."));
	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_TailCave"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Data/TailCave_Navi.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("네비게이션 생성 중."));
	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Shop"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Data/Shop_Navi.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("네비게이션 생성 중."));
	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Room"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Data/Room_Navi.dat")))))
		return E_FAIL;

	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_CraneGame"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Data/CraneGame_Navi.dat")))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Link*/
	_matrix			PivotMatrix = XMMatrixIdentity();
	//PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Link"),
	//CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/Link/Link_Anim.fbx", PivotMatrix))))
	//return E_FAIL;

	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Marin"),
	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/Marin/Marin.fbx", PivotMatrix))))
		return E_FAIL;

	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Link"), LEVEL_STATIC, CData_Manager::DATA_ANIM);

	PivotMatrix = XMMatrixIdentity();

	/*For.Prototype_Component_Model_BladeTrap*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Boulder"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/Boulder/Boulder.fbx", PivotMatrix))))
		return E_FAIL;
	
	/*For.Prototype_Component_Model_Lawn*/
	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Lawn"), LEVEL_STATIC, CData_Manager::DATA_NONANIM);

	/*For.Prototype_Component_Model_Grass*/
	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Grass"), LEVEL_STATIC, CData_Manager::DATA_NONANIM);

	/*For.Prototype_Component_Model_Grass2x2*/
	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Grass2x2"), LEVEL_STATIC, CData_Manager::DATA_NONANIM);


	PivotMatrix = XMMatrixIdentity();
	
	/*For.Prototype_Component_Model_Arrow*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Arrow"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/Arrow/Arrow.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_DogFood*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_DogFood"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/DogFood/DogFood.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_HeartContainer*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_HeartContainer"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/HeartContainer/HeartContainer.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_MagicRod*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_MagicRod"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/MagicRod/MagicRod.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Bow*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_ItemBow"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/Anim/Link/Bow.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_SmallKey*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_SmallKey"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/DgnKey/SmallKey/SmallKey.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_BossKey*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_BossKey"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/DgnKey/BossKey/BossKey.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Compass*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Compass"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/Compass/Compass.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_DgnMap*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_DgnMap"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/DgnMap/DgnMap.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_HeartRecovery*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_HeartRecovery"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/HeartRecovery/HeartRecovery.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Ruby*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Ruby"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/Ruby/Ruby.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Feather*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Feather"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/Feather/Feather.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_FullMoonCello*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_FullMoonCello"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/FullMoonCello/FullMoonCello.fbx", PivotMatrix))))
		return E_FAIL;



	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Necklace"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/Necklace/Necklace.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_YoshiDoll"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/YoshiDoll/YoshiDoll.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_MagicPowder"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/MagicPowder/MagicPowder.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_RubyPurple"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/Ruby/RubyPurple.fbx", PivotMatrix))))
		return E_FAIL;


	/*For.Prototype_Component_Model_Bow*/
	PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixTranslation(0.f, 0.2f, 0.2f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Bow"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Link/Bow.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_DogFood*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_DogFoodParts"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/DogFood/DogFood.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_MagicRod*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_MagicRodParts"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/MagicRod/MagicRod.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Arrow*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_ArrowParts"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/Arrow/Arrow.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_HeartContainer*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_HeartContainerParts"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/HeartContainer/HeartContainer.fbx", PivotMatrix))))
		return E_FAIL;


	/*For.Prototype_Component_Model_TreasureBox*/
	PivotMatrix = XMMatrixIdentity();
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_TreasureBox"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/Obj/TreasureBox/TreasureBox.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_FootSwitch*/
	PivotMatrix = XMMatrixIdentity();
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_FootSwitch"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/Obj/Switch/FootSwitch.fbx", PivotMatrix))))
		return E_FAIL;



	/* 셰이더 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("셰이더 로딩 중."));
	/* For.Prototype_Component_Shader_VtxNorTex*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Shaderfiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Shaderfiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Shaderfiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_SPHERE */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	RELEASE_INSTANCE(CData_Manager);
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlayLevel()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr == pGameInstance)
		return E_FAIL;
	CData_Manager* pData_Manager = GET_INSTANCE(CData_Manager);


	/* 모델 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("모델 생성 중."));
	_matrix			PivotMatrix = XMMatrixIdentity();

	for (int i = 4; i < 16; ++i)
	{
		for (int j = 0; j < 7; ++j)
		{
			_tchar*			pModeltag = new _tchar[MAX_PATH];
			_tchar*			szFilePath = new _tchar[MAX_PATH];
			wsprintf(pModeltag, TEXT("Field_%02d%c.fbx"), i, j + 65);
	
			if (FAILED(pData_Manager->Create_Try_BinModel(pModeltag, LEVEL_GAMEPLAY, CData_Manager::DATA_NONANIM)))
			{
				Safe_Delete(pModeltag);
				Safe_Delete(szFilePath);
			}
			Safe_Delete(szFilePath);
			pData_Manager->Add_ModelTag(pModeltag);
		}
	}

	/*For.Prototype_Component_Model_Octorock*/
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	/*if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Octorock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/Octorock/Octorock.fbx", PivotMatrix))))
		return E_FAIL;*/
	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("Octorock"), LEVEL_GAMEPLAY, CData_Manager::DATA_ANIM);

	/*For.Prototype_Component_Model_MoblinSword*/
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("MoblinSword"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/MoblinSword/MoblinSword.fbx", PivotMatrix))))
	//	return E_FAIL;
	CData_Manager::Get_Instance()->Create_Try_BinModel(TEXT("MoblinSword"), LEVEL_GAMEPLAY, CData_Manager::DATA_ANIM);


	/*For.Prototype_Component_Model_TailStatue*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TailStatue"),
	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/TailStatue/TailStatue.fbx", PivotMatrix))))
	return E_FAIL;


	/*For.Prototype_Component_Model_TailShutter*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TailCaveShutter"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/Obj/Door/TailCaveShutter/TailCaveShutter.fbx", PivotMatrix))))
		return E_FAIL;


	/*For.Prototype_Component_Model_OctorockBullet*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_OctorockBullet"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Bullet/Octorock/Octorock.fbx", PivotMatrix))))
		return E_FAIL;




	/* 콜라이더 생성 중. */
	lstrcpy(m_szLoadingText, TEXT("콜라이더 생성 중."));

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_SPHERE */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;


	/* 객체 생성 중. */
	lstrcpy(m_szLoadingText, TEXT("객체 생성 중."));


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	RELEASE_INSTANCE(CData_Manager);
	RELEASE_INSTANCE(CGameInstance);

	m_isFinished = true;


	return S_OK;
}

HRESULT CLoader::Loading_ForTailCaveLevel()
{

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr == pGameInstance)
		return E_FAIL;
	CData_Manager* pData_Manager = GET_INSTANCE(CData_Manager);


	_matrix			PivotMatrix = XMMatrixIdentity();
	/* 모델 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("모델 생성 중."));
	for (int i = 1; i < 9; ++i)
	{
		for (int j = 0; j < 7; ++j)
		{
			//const char* pFilePath = "../Bin/Resources/Meshes/Field/Field_%02d%c.fbx";

			/*_tchar*			pModeltag = new _tchar[MAX_PATH];
			_tchar*			szFilePath = new _tchar[MAX_PATH];
			wsprintf(pModeltag, TEXT("Lv01TailCave_%02d%c.fbx"), i, j + 65);
			wsprintf(szFilePath, TEXT("../../../Bin/Resources/Meshes/NonAnim/TailCave/Lv01TailCave_%02d%c.fbx"), i, j + 65);

			char* FilePath = new char[MAX_PATH];
			WideCharToMultiByte(CP_ACP, 0, szFilePath, MAX_PATH, FilePath, MAX_PATH, NULL, NULL);

			if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, pModeltag,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, FilePath, PivotMatrix))))
			{
				Safe_Delete(pModeltag);
				Safe_Delete(szFilePath);
				Safe_Delete(FilePath);

				continue;
			}
			Safe_Delete(szFilePath);
			Safe_Delete(FilePath);
			CData_Manager::Get_Instance()->Add_ModelTag(pModeltag);*/


			_tchar*			pModeltag = new _tchar[MAX_PATH];
			_tchar*			szFilePath = new _tchar[MAX_PATH];
			wsprintf(pModeltag, TEXT("Lv01TailCave_%02d%c.fbx"), i, j + 65);

			if (FAILED(pData_Manager->Create_Try_BinModel(pModeltag, LEVEL_TAILCAVE, CData_Manager::DATA_NONANIM)))
			{
				Safe_Delete(pModeltag);
				Safe_Delete(szFilePath);
			}
			Safe_Delete(szFilePath);
			pData_Manager->Add_ModelTag(pModeltag);
		}

		int a = 0;
	}


	/*For.Prototype_Component_Model_Rola*/
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_Rola"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/Obj/Monster/Rola/Rola.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_RolaBullet*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_RolaBullet"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Bullet/RollingSpike/RollingSpike.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Pawn*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_Pawn"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/Obj/Monster/Pawn/Pawn.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Keese*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_Keese"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/Obj/Monster/Keese/Keese.fbx", PivotMatrix))))
		return E_FAIL;


	/*For.Prototype_Component_Model_BuzzBlob*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_BuzzBlob"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/Obj/Monster/BuzzBlob/BuzzBlob.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_TailBoss*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_TailBoss1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/Obj/Monster/TailBoss/TailBoss1.fbx", PivotMatrix))))
		return E_FAIL;
	/*For.Prototype_Component_Model_TailBoss*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_TailBoss2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/Obj/Monster/TailBoss/TailBoss2.fbx", PivotMatrix))))
		return E_FAIL;
	/*For.Prototype_Component_Model_TailBoss*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_TailBoss3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/Obj/Monster/TailBoss/TailBoss3.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Tail*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_Tail1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/Obj/Monster/Tail/Tail1.fbx", PivotMatrix))))
		return E_FAIL;
	/*For.Prototype_Component_Model_Tail*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_Tail2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/Obj/Monster/Tail/Tail2.fbx", PivotMatrix))))
		return E_FAIL;
	/*For.Prototype_Component_Model_Tail*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_Tail3"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/Obj/Monster/Tail/Tail3.fbx", PivotMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_CollapseTile */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_CollapseTile"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/CollapseTile/CollapseTile.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_RedZol*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_RedZol"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/Obj/Monster/ZolRed/ZolRed.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_ClosedDoor*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_ClosedDoor"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/Obj/Door/ClosedDoor/ClosedDoor.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_LockDoor*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_LockDoor"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/Obj/Door/LockDoor/LockDoor.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_SquareBlock*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_SquareBlock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/SquareBlock/SquareBlock.fbx", PivotMatrix))))
		return E_FAIL;


	/*For.Prototype_Component_Model_LockBlock*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_LockBlock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/LockBlock/LockBlock.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Togezo*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_Togezo"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/Obj/Monster/Togezo/Togezo.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_BladeTrap*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_BladeTrap"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/Obj/BladeTrap/BladeTrap.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_BossDoor*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_BossDoor"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/Obj/Door/BossDoor/BossDoor.fbx", PivotMatrix))))
		return E_FAIL;

	/* 콜라이더 생성 중. */
	lstrcpy(m_szLoadingText, TEXT("콜라이더 생성 중."));

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_SPHERE */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	RELEASE_INSTANCE(CData_Manager);
	RELEASE_INSTANCE(CGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForRoomLevel()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* 콜라이더 생성 중. */
	lstrcpy(m_szLoadingText, TEXT("콜라이더 생성 중."));

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ROOM, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ROOM, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_SPHERE */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ROOM, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	_matrix			PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	/*For.Prototype_Component_Model_ShopNpc*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ROOM, TEXT("Prototype_Component_Model_ShopNpc"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/ShopNpc/ShopNpc.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_GameShopOwner*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ROOM, TEXT("Prototype_Component_Model_GameShopOwner"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/GameShopOwner/GameShopOwner.fbx", PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixIdentity();
	
	/*For.Prototype_Component_Model_MarinHouse*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ROOM, TEXT("MarinHouse.fbx"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/MarinHouse/MarinHouse.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Shop*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ROOM, TEXT("Shop.fbx"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Shop/Shop.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_CraneGame*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ROOM, TEXT("CraneGame.fbx"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/CraneGame/CraneGame.fbx", PivotMatrix))))
		return E_FAIL;


	/*For.Prototype_Component_Model_Crane*/
	PivotMatrix = XMMatrixIdentity();
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ROOM, TEXT("Crane.fbx"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/Crane/Crane.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Crane*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ROOM, TEXT("Prototype_Component_Model_CraneFence"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/CraneFence/CraneFence.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ROOM, TEXT("Prototype_Component_Model_MovingFloorCrane"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/NonAnim/Obj/MovingFloorCrane/MovingFloorCrane.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_ROOM, TEXT("CraneGameButton.fbx"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Anim/CraneGameButton/CraneGameButton.fbx", PivotMatrix))))
		return E_FAIL;

	


	RELEASE_INSTANCE(CGameInstance);


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_ObjectPrototype()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/*For.Prototype_GameObject_MarinNpc*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CraneGameNpc"),
		CCraneGameNpc::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_MarinNpc*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MarinNpc"),
		CMarinNpc::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_ShopNpc*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ShopNpc"),
		CShopNpc::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Grass*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Grass"),
		CGrass::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_PrizeItem*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PrizeItem"),
		CPrizeItem::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Camera2D*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_2D"),
		CCamera_2D::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Portal*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Portal"),
		CPortal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_BladeTrap*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BladeTrap"),
		CBladeTrap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Togezo*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Togezo"),
		CTogezo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Tail*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tail"),
		CTail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_SquareBlock*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SquareBlock"),
		CSquareBlock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Keese*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Keese"),
		CKeese::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_RedZol*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RedZol"),
		CRedZol::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Door*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Door"),
		CDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_CollapseTile*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CollapeTile"),
		CCollapseTile::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_TailBoss*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TailBoss"),
		CTailBoss::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_BuzzBlob*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BuzzBlob"),
		CBuzzBlob::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Camera_Dynamic */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"),
		CCamera_Dynamic::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround_UI"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/*For.Prototype_GameObject_UI */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_InvenTile_UI"),
		CInvenTile::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CUIButton"),
		CUIButton::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CInvenItem"),
		CInvenItem::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NonAnim"),
		CNonAnim::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerState"),
		CPlayerState::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MessageBox"),
		CMessageBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	/*For.Prototype_GameObject_Player */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon"),
		CWeapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerBullet"),
		CPlayerBullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	/*For.Prototype_GameObject_Monster */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pawn"),
		CPawn::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rola"),
		CRola::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Octorock"),
		COctorock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MoblinSword"),
		CMoblinSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterBullet"),
		CMonsterBullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/*For.Prototype_GameObject_Object */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DgnKey"),
		CDgnKey::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TreasureBox"),
		CTreasureBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FootSwitch"),
		CFootSwitch::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_For_UITexture()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_CraneGameTalk"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/UI/Message/CraneGameTalk_%d.dds"), 6))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MarinTalk"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/UI/Message/MarinTalk_%d.dds"), 21))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rupee"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/UI/Ruby/Rupee_00.dds"), 1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Number"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/UI/Number/Number_%d.dds"), 10))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GetMessage"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/UI/Message/MesGetFrame_%02d.dds"), 9))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopTalk"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/UI/Message/ShopTalk_%d.dds"), 9))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LoadingScreen_UI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/UI/Screen/LoadingScreen.dds"), 1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TitleScreen"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/UI/Screen/TitleScreen.dds"), 1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InventoryBackGround_UI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/UI/Inventory.dds"), 1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InvenTile"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/UI/InvenTile/InvenTile_%d.dds"), 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipTile"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/UI/InvenTile/EquipTile_%d.dds"), 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ButtonX"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/UI/Button/BtnX_%02d.png"), 3))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ButtonY"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/UI/Button/BtnY_%02d.png"), 3))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InteractButton"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/UI/Button/InteractBT_%d.png"), 3))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ChoiceButton"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/UI/Button/ChoiceBT_%d.dds"), 27))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipItem"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/UI/Item/EqItem_%d.png"), 8))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UsableItem"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/UI/Item/Item_%d.png"), 7))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DungeonKey"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/UI/Item/CollectDgnKey_%02d.png"), 6))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_CollectItem"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/UI/Item/CollectItem_%d.png"), 9))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_QuestItem"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/UI/Item/QuestItem_%d.png"), 3))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PrizeItem"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/UI/Item/DgnItem_%d.png"), 3))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/UI/Hp/Heart%02d.dds"), 5))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SmallKey"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/UI/Key/SmallKey_00.dds"), 1))))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel)
{
	CLoader*	pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		ERR_MSG(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

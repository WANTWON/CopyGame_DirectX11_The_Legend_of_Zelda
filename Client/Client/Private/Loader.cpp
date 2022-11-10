#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"
#include "Camera_Dynamic.h"

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

//for UI
#include "BackGround.h"
#include "InvenTile.h"
#include "UIButton.h"
#include "InvenItem.h"
#include "PlayerState.h"

//for Object 
#include "Terrain.h"
#include "NonAnim.h"
#include "DgnKey.h"
#include "TreasureBox.h"
#include "FootSwitch.h"

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
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Data/Navi_test.dat")))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("네비게이션 생성 중."));
	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_TailCave"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Data/TailCave_Navi.dat")))))
		return E_FAIL;


	/*For.Prototype_Component_Model_Link*/
	_matrix			PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Link"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Link/Link_Anim.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_SmallKey*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_SmallKey"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/Obj/DgnKey/SmallKey/SmallKey.fbx", PivotMatrix))))
		return E_FAIL;


	/*For.Prototype_Component_Model_Bow*/
	PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixTranslation(0.f, 0.2f, 0.2f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Bow"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/Link/Bow.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_TreasureBox*/
	PivotMatrix = XMMatrixIdentity();
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_TreasureBox"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Obj/TreasureBox/TreasureBox.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_FootSwitch*/
	PivotMatrix = XMMatrixIdentity();
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_FootSwitch"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Obj/Switch/FootSwitch.fbx", PivotMatrix))))
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

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlayLevel()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr == pGameInstance)
		return E_FAIL;




	_matrix			PivotMatrix = XMMatrixIdentity();

	for (int i = 1; i < 17; ++i)
	{
		for (int j = 0; j < 7; ++j)
		{
			_tchar*			pModeltag = new _tchar[MAX_PATH];
			_tchar*			szFilePath = new _tchar[MAX_PATH];
			wsprintf(pModeltag, TEXT("Field_%02d%c.fbx"), i, j + 65);
			wsprintf(szFilePath, TEXT("../../../Bin/Resources/Meshes/Field/Field_%02d%c.fbx"), i, j + 65);

			char* FilePath = new char[MAX_PATH];
			WideCharToMultiByte(CP_ACP, 0, szFilePath, MAX_PATH, FilePath, MAX_PATH, NULL, NULL);

			if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, pModeltag,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, FilePath, PivotMatrix))))
			{
				Safe_Delete(pModeltag);
				Safe_Delete(szFilePath);
				Safe_Delete(FilePath);

				continue;
			}
			Safe_Delete(szFilePath);
			Safe_Delete(FilePath);
		}
	}

	/*For.Prototype_Component_Model_Octorock*/
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Octorock"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Obj/Monster/Octorock/Octorock.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_MoblinSword*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MoblinSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Obj/Monster/MoblinSword/MoblinSword.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_OctorockBullet*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_OctorockBullet"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/Bullet/Octorock/Octorock.fbx", PivotMatrix))))
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

	RELEASE_INSTANCE(CGameInstance);

	m_isFinished = true;


	return S_OK;
}

HRESULT CLoader::Loading_ForTailCaveLevel()
{

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr == pGameInstance)
		return E_FAIL;


	_matrix			PivotMatrix = XMMatrixIdentity();
	/* 모델 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("모델 생성 중."));
	for (int i = 1; i < 9; ++i)
	{
		for (int j = 0; j < 7; ++j)
		{
			//const char* pFilePath = "../Bin/Resources/Meshes/Field/Field_%02d%c.fbx";

			_tchar*			pModeltag = new _tchar[MAX_PATH];
			_tchar*			szFilePath = new _tchar[MAX_PATH];
			wsprintf(pModeltag, TEXT("Lv01TailCave_%02d%c.fbx"), i, j + 65);
			wsprintf(szFilePath, TEXT("../../../Bin/Resources/Meshes/TailCave/Lv01TailCave_%02d%c.fbx"), i, j + 65);

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

		}

		int a = 0;
	}


	/*For.Prototype_Component_Model_Rola*/
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_Rola"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Obj/Monster/Rola/Rola.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_RolaBullet*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_RolaBullet"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Bullet/RollingSpike/RollingSpike.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Pawn*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TAILCAVE, TEXT("Prototype_Component_Model_Pawn"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../../Bin/Resources/Meshes/Obj/Monster/Pawn/Pawn.fbx", PivotMatrix))))
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

	RELEASE_INSTANCE(CGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_ObjectPrototype()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

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

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hp"),
		CPlayerState::Create(m_pDevice, m_pContext))))
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
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/UI/Item/QuestItem_%d.png"), 2))))
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

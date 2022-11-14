#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"


#include "Camera_Dynamic.h"
#include "BackGround.h"
#include "Terrain.h"
#include "NonAnim.h"
#include "ModelManager.h"
#include "Imgui_Manager.h"
//#include "Effect.h"
//#include "Sky.h"
//#include "UI.h"


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
	lstrcpy(m_szLoadingText, TEXT("텍스쳐 로딩 중."));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;
	
	/* 모델 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("모델 로딩 중."));
	

	/* 셰이더 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("셰이더 로딩 중."));
	

	

	// CShader::Create(m_pDevice, m_pContext, TEXT(""), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	

	

	/* 객체 원형 생성 중. */
	lstrcpy(m_szLoadingText, TEXT("객체 생성 중."));
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlayLevel()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	CModelManager*		pModelManager = GET_INSTANCE(CModelManager);

	/* 텍스쳐 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("텍스쳐 로딩 중."));

	///*For.Prototype_Component_Texture_UI */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
	//	return E_FAIL;

	/*For.Prototype_Component_Texture_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2))))
		return E_FAIL;

	///*For.Prototype_Component_Texture_Sky */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_CUBEMAP, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
	//	return E_FAIL;

	///*For.Prototype_Component_Texture_Explosion */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"),
	//	CTexture::Create(m_pGraphic_Device, CTexture::TYPE_DEFAULT, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
	//	return E_FAIL;	
	//

	/* 모델 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("모델 로딩 중."));

	

	if (FAILED(Loading_ForModel(TEXT("../../../Bin/Resources/Meshes/Field/"))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Fiona*/
	//PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	/*if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/Fiona/Fiona.fbx", PivotMatrix))))
		return E_FAIL;*/

	///*For.Prototype_Component_Model_ForkLift*/
	//PivotMatrix = XMMatrixScaling(0.11f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Meshes/ForkLift/ForkLift.fbx", PivotMatrix))))
	//	return E_FAIL;

	/*For.Prototype_Component_Model_Field05*/
	/*PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (pModelManager->Create_Model_Prototype(LEVEL_GAMEPLAY, TEXT("Model_Field05%c"),
		m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../Bin/Resources/Meshes/Test/Field_05%c.fbx"), PivotMatrix, 16))
		return E_FAIL;*/


	///*For.Prototype_Component_VIBuffer_Cube */
	//if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
	//	CVIBuffer_Cube::Create(m_pGraphic_Device))))
	//	return E_FAIL;
	//

	/* 셰이더 로딩 중. */
	lstrcpy(m_szLoadingText, TEXT("셰이더 로딩 중."));

	/* For.Prototype_Component_Shader_VtxModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Shaderfiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../../Bin/Shaderfiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* 객체 생성 중. */
	lstrcpy(m_szLoadingText, TEXT("객체 생성 중."));

	///*For.Prototype_GameObject_UI */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI"),
	//	CUI::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	///*For.Prototype_GameObject_Sky */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
	//	CSky::Create(m_pGraphic_Device))))
	//	return E_FAIL;

	/*For.Prototype_GameObject_Player*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NonAnim"),
		CNonAnim::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Terrain*/
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*For.Prototype_GameObject_Camera_Dynamic */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"),
		CCamera_Dynamic::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	///*For.Prototype_GameObject_Effect */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect"),
	//	CEffect::Create(m_pGraphic_Device))))
	//	return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	RELEASE_INSTANCE(CModelManager);
	RELEASE_INSTANCE(CGameInstance);

	m_isFinished = true;


	return S_OK;
}

HRESULT CLoader::Loading_ForModel(_tchar* cFolderPath)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_matrix			PivotMatrix = XMMatrixIdentity();

//#pragma region Field
//	for (int i = 1; i < 17; ++i)
//	{
//		for (int j = 0; j < 7; ++j)
//		{
//			//const char* pFilePath = "../Bin/Resources/Meshes/Field/Field_%02d%c.fbx";
//
//			_tchar*			pModeltag = new _tchar[MAX_PATH];
//			_tchar*			szFilePath = new _tchar[MAX_PATH];
//			wsprintf(pModeltag, TEXT("Field_%02d%c.fbx"), i, j+65);
//			wsprintf(szFilePath, TEXT("../../../Bin/Resources/Meshes/Field/Field_%02d%c.fbx"), i, j + 65);
//
//			char* FilePath= new char[MAX_PATH];
//			WideCharToMultiByte(CP_ACP, 0, szFilePath, MAX_PATH, FilePath, MAX_PATH, NULL, NULL);
//
//			if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, pModeltag,
//				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, FilePath, PivotMatrix))))
//			{
//				delete pModeltag;
//				delete szFilePath;
//				delete FilePath;
//
//				continue;
//			}
//
//			CImgui_Manager::Get_Instance()->Add_TempTag(pModeltag);
//			CImgui_Manager::Get_Instance()->Add_TempTag(szFilePath);
//			
//			delete FilePath;
//				
//		}
//	}
//
//#pragma endregion Field
//

#pragma region TailCave
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

			if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, pModeltag,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, FilePath, PivotMatrix))))
			{
				delete pModeltag;
				delete szFilePath;
				delete FilePath;

				continue;
			}

			CImgui_Manager::Get_Instance()->Add_TempTag(pModeltag);
			CImgui_Manager::Get_Instance()->Add_TempTag(szFilePath);

			delete FilePath;

		}
	}
#pragma endregion TailField

	/* Model for Monster */
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("MoblinSword.fbx"), CModel::Create(m_pDevice, m_pContext,
		CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/Obj/Monster/MoblinSword/MoblinSword.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Octorock.fbx"), CModel::Create(m_pDevice, m_pContext,
		CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/Obj/Monster/Octorock/Octorock.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Pawn.fbx"), CModel::Create(m_pDevice, m_pContext,
		CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/Obj/Monster/Pawn/Pawn.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Rola.fbx"), CModel::Create(m_pDevice, m_pContext,
		CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/Obj/Monster/Rola/Rola.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("SmallKey.fbx"), CModel::Create(m_pDevice, m_pContext,
		CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/Obj/DgnKey/SmallKey/SmallKey.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("FootSwitch.fbx"), CModel::Create(m_pDevice, m_pContext,
		CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/Obj/Switch/FootSwitch.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("TreasureBox.fbx"), CModel::Create(m_pDevice, m_pContext,
		CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/Obj/TreasureBox/TreasureBox.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Picking_Symbol"), CModel::Create(m_pDevice, m_pContext,
		CModel::TYPE_NONANIM, "../Bin/Resources/Picking_Symbol/Picking_Symbol.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_BuzzBlob*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("BuzzBlob.fbx"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/Obj/Monster/BuzzBlob/BuzzBlob.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_TailBoss*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("TailBoss1.fbx"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/Obj/Monster/TailBoss/TailBoss1.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_CollapseTile*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("CollapseTile.fbx"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/Obj/CollapseTile/CollapseTile.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_ClosedDoor*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("ClosedDoor.fbx"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/Obj/Door/ClosedDoor/ClosedDoor.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_RedZol*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("ZolRed.fbx"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/Obj/Monster/ZolRed/ZolRed.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_Keese*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Keese.fbx"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/Obj/Monster/Keese/Keese.fbx", PivotMatrix))))
		return E_FAIL;

	/*For.Prototype_Component_Model_LockDoor*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("LockDoor.fbx"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../../Bin/Resources/Meshes/Obj/Door/LockDoor/LockDoor.fbx", PivotMatrix))))
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

#include "stdafx.h"
#include "..\Public\Terrain_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CTerrain_Manager)

CTerrain_Manager::CTerrain_Manager()
{
	
}

HRESULT CTerrain_Manager::Create_Terrain(LEVEL eLevel, const _tchar* pLayerTag)
{

	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	TERRAINDESC TerrainDesc = m_TerrainDesc;
	TerrainDesc.m_bTestShowTerrain = false;


	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Terrain"), eLevel, pLayerTag, &TerrainDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CTerrain_Manager::Free()
{
}

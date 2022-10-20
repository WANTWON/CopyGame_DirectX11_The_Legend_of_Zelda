#include "stdafx.h"
#include "..\Public\ModelManager.h"
#include "GameInstance.h"
#include "Model.h"


IMPLEMENT_SINGLETON(CModelManager)

CModelManager::CModelManager()
{
}


HRESULT CModelManager::Create_Model_Prototype(LEVEL eLevel, const _tchar * pLayerTag, ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CModel::TYPE eModelType, const _tchar * pModelFilePath, _fmatrix PivotMatrix, _uint iNum)
{

	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);


	_tchar			szFullPath[MAX_PATH] = TEXT("");
	_tchar			szLayerTag[MAX_PATH] = TEXT("");

	for (_uint i = 0; i < iNum; ++i)
	{
		wsprintf(szFullPath, pModelFilePath, i+65);
		wsprintf(szLayerTag, pLayerTag, i+65);

		char szRealPath[MAX_PATH] = "";
		WideCharToMultiByte(CP_ACP, 0, szFullPath, MAX_PATH, szRealPath, MAX_PATH, NULL, NULL);

		if (FAILED(pGameinstance->Add_Prototype(eLevel, szLayerTag,
			CModel::Create(pDevice, pContext, eModelType, szRealPath, PivotMatrix))))
			return E_FAIL;

		m_LayerTags.push_back(szLayerTag);
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CModelManager::Free()
{
	m_LayerTags.clear();
}

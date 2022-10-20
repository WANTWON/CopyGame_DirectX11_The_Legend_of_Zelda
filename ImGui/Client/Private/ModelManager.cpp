#include "stdafx.h"
#include "..\Public\ModelManager.h"
#include "GameInstance.h"
#include "Model.h"
#include "NonAnim.h"


IMPLEMENT_SINGLETON(CModelManager)

CModelManager::CModelManager()
{
}


HRESULT CModelManager::Create_Model_Prototype(LEVEL eLevel, const _tchar * pLayerTag, ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CModel::TYPE eModelType, const _tchar * pModelFilePath, _fmatrix PivotMatrix, _uint iNum)
{

	CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);

	for (_uint i = 0; i < iNum; ++i)
	{
		_tchar*         szFullPath = new _tchar[MAX_PATH]; // = TEXT("");
		_tchar*         szLayerTag = new _tchar[MAX_PATH]; // = TEXT("");

		wsprintf(szFullPath, pModelFilePath, i + 65);
		wsprintf(szLayerTag, pLayerTag, i + 65);

		char szRealPath[MAX_PATH] = "";
		WideCharToMultiByte(CP_ACP, 0, szFullPath, MAX_PATH, szRealPath, MAX_PATH, NULL, NULL);

		if (FAILED(pGameinstance->Add_Prototype(eLevel, szLayerTag,
			CModel::Create(pDevice, pContext, eModelType, szRealPath, PivotMatrix))))
			return E_FAIL;

		m_LayerTags.push_back(szLayerTag);

		Safe_Delete(szFullPath);
		
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CModelManager::Create_Model_Clone(LEVEL eLevel, const _tchar* pModelTag, const _tchar* pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CNonAnim::NONANIMDESC  NonAnimDesc;
	NonAnimDesc.pModeltag = pModelTag;
	NonAnimDesc.vPosition = _float3(0.f, 0.f, 0.f);
	NonAnimDesc.vScale = _float3(1.f, 1.f, 1.f);
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Player"), eLevel, pLayerTag, &NonAnimDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);


	return S_OK;
}

void CModelManager::Free()
{
	for (auto& iter : m_LayerTags)
		Safe_Delete(iter);

	m_LayerTags.clear();
}

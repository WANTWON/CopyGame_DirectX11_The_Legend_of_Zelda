#include "stdafx.h"
#include "..\Public\ModelManager.h"
#include "GameInstance.h"
#include "Model.h"
#include "GameObject.h"


IMPLEMENT_SINGLETON(CModelManager)

CModelManager::CModelManager()
{
	m_LayerTags.reserve(0);
	m_CreatedModel.reserve(0);
	
}


HRESULT CModelManager::Create_Model_Prototype(LEVEL eLevel, const _tchar * pModelTag, ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CModel::TYPE eModelType, _fmatrix PivotMatrix)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	const _tchar* pModelFilePath = Find_ModelTag(pModelTag);
	if (pModelFilePath == nullptr)
		return E_FAIL;

	char szLayertag[MAX_PATH] = "";
	WideCharToMultiByte(CP_ACP, 0, pModelFilePath, MAX_PATH, szLayertag, MAX_PATH, NULL, NULL);

	if (FAILED(pGameInstance->Add_Prototype(eLevel, pModelTag, CModel::Create(pDevice, pContext, eModelType, szLayertag, PivotMatrix))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CModelManager::Create_Model(LEVEL eLevel, const _tchar* pModelTag, const _tchar* pLayerTag, ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CModel::TYPE eModelType, _fmatrix PivotMatrix)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	Create_Model_Prototype(eLevel, pModelTag, pDevice, pContext, eModelType, PivotMatrix);

	CNonAnim::NONANIMDESC  NonAnimDesc;
	memcpy(&NonAnimDesc, &m_InitModelDesc, sizeof(CNonAnim::NONANIMDESC));
	NonAnimDesc.pModeltag = pModelTag;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_NonAnim"), eLevel, pLayerTag, &NonAnimDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CModelManager::Add_FileName(const _tchar * Layertag, const _tchar * FileName)
{

	const _tchar* pModelTag = Find_ModelTag(Layertag);
	if (pModelTag != nullptr)
		return;

	_tchar*         szFullPath = new _tchar[MAX_PATH]; // = TEXT("");
	_tchar*         szLayerTag = new _tchar[MAX_PATH]; // = TEXT("");
	
	_tcscpy(szLayerTag, Layertag);
	_tcscpy(szFullPath, FileName);


	m_ModelTags.emplace(szLayerTag, szFullPath);
	m_LayerTags.push_back(szLayerTag);
}

void CModelManager::Set_AllPickedFalse()
{
	for (auto& pGameObject : m_CreatedModel)
		dynamic_cast<CGameObject*>(pGameObject)->Set_Picked(false);
}

const _tchar * CModelManager::Find_ModelTag(const _tchar * ModelTag)
{
	auto	iter = find_if(m_ModelTags.begin(), m_ModelTags.end(), CTag_Finder(ModelTag));

	if (iter == m_ModelTags.end())
		return nullptr;

	return iter->second;
}

void CModelManager::Out_CreatedModel(CNonAnim * pGameObject)
{
	auto& iter = m_CreatedModel.begin();
	while (iter != m_CreatedModel.end())
	{
		if (*iter == pGameObject)
			iter = m_CreatedModel.erase(iter);
		else
			++iter;
	}
}

void CModelManager::Free()
{

	for (auto& iter : m_ModelTags)
		Safe_Delete(iter.second);
	m_ModelTags.clear();


	for (auto& iter : m_LayerTags)
		Safe_Delete(iter);
	m_LayerTags.clear();

	m_CreatedModel.clear();
}

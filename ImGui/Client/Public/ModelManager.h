#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Model.h"


BEGIN(Client)

class CModelManager final : public CBase
{
	DECLARE_SINGLETON(CModelManager)

private:
	CModelManager();
	virtual ~CModelManager() = default;

public:
	HRESULT Create_Model_Prototype (LEVEL eLevel, const _tchar* pLayerTag, ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CModel::TYPE eModelType, _fmatrix PivotMatrix);
	HRESULT Create_Model(LEVEL eLevel, const _tchar* pPrototypeTag, const _tchar* pLayerTag, ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CModel::TYPE eModelType, _fmatrix PivotMatrix);
	void Set_FilePath(const _tchar * FilePath) { m_pModelFilePath = FilePath; }

public:
	const _tchar * m_pModelFilePath = nullptr;
	vector<const _tchar*> Get_LayerTags() { return m_LayerTags;}
	void Add_FileName(const _tchar* Layertag, const _tchar* FileName);
	

private:
	const _tchar* Find_ModelTag(const _tchar* ModelTag);


private:
	map<const _tchar*, const _tchar*>  m_ModelTags;
	vector<const _tchar*> m_LayerTags;

public:
	void Free() override;
};

END
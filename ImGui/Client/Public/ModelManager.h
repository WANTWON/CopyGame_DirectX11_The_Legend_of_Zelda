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
	HRESULT Create_Model_Prototype (LEVEL eLevel, const _tchar* pLayerTag, ID3D11Device * pDevice, ID3D11DeviceContext * pContext,
		CModel::TYPE eModelType, const _tchar * pModelFilePath, _fmatrix PivotMatrix, _uint iNum);
	HRESULT Create_Model_Clone(LEVEL eLevel, const _tchar* pPrototypeTag, const _tchar* pLayerTag);

public:
	vector<const _tchar*> Get_LayerTags() { return m_LayerTags;}

private:
	vector<const _tchar*>  m_LayerTags;

public:
	void Free() override;
};

END
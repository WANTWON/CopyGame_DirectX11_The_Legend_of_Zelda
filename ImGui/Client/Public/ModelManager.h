#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Model.h"

BEGIN(Client)
class CNonAnim;
class CModelManager final : public CBase
{
	DECLARE_SINGLETON(CModelManager)

private:
	CModelManager();
	virtual ~CModelManager() = default;

public:
	HRESULT Create_Model_Prototype (LEVEL eLevel, const _tchar* pLayerTag, ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CModel::TYPE eModelType, _fmatrix PivotMatrix);
	HRESULT Create_Model(LEVEL eLevel, const _tchar* pPrototypeTag, const _tchar* pLayerTag, ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CModel::TYPE eModelType, _fmatrix PivotMatrix);

public:
	vector<const _tchar*> Get_LayerTags() { return m_LayerTags;}
	vector<class CNonAnim*> Get_CreatedModel() { return m_CreatedModel; }


	void Add_FileName(const _tchar* Layertag, const _tchar* FileName);
	void Add_CreatedModel( class CNonAnim* pNonAnimModel) { m_CreatedModel.push_back(pNonAnimModel); }
	void Set_AllPickedFalse();

private:
	const _tchar* Find_ModelTag(const _tchar* ModelTag);


private:
	map<const _tchar*, const _tchar*>  m_ModelTags;
	vector<const _tchar*> m_LayerTags;
	vector<class CNonAnim*> m_CreatedModel;

public:
	void Free() override;
};

END
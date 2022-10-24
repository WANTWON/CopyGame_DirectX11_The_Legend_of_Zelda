#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Model.h"
#include "NonAnim.h"

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

public:
	vector<const _tchar*> Get_LayerTags() { return m_LayerTags;}
	const vector<class CNonAnim*> Get_CreatedModel() const { return m_CreatedModel; }


	void Add_FileName(const _tchar* Layertag, const _tchar* FileName);
	void Add_CreatedModel( class CNonAnim* pNonAnimModel) { m_CreatedModel.push_back(pNonAnimModel); }
	void Set_AllPickedFalse();
	void Set_InitModelDesc(CNonAnim::NONANIMDESC ModelDesc) { memcpy(&m_InitModelDesc, &ModelDesc, sizeof(CNonAnim::NONANIMDESC)); }
	void Out_CreatedModel(CNonAnim* pGameObject);

private:
	const _tchar* Find_ModelTag(const _tchar* ModelTag);
	

private:
	map<const _tchar*, const _tchar*>  m_ModelTags;  //Prototype Tag & Path
	vector<const _tchar*> m_LayerTags; //Prototype Tag ¸¸ ¸ð¾ÆµÐ°Å  
	vector<class CNonAnim*> m_CreatedModel;
	CNonAnim::NONANIMDESC  m_InitModelDesc;

public:
	void Free() override;
};

END
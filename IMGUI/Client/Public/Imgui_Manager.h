#pragma once

#include "Client_Defines.h"
#include "Base.h"

#include "Level_Manager.h"
#include "Terrain_Manager.h"
#include "ModelManager.h"
#include "NonAnim.h"
#include "Navigation_Manager.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager)

public:
	enum PICKING_TYPE { PICKING_TERRAIN_TRANSFORM, PICKING_TERRAIN_SHAPE };

private:
	CImgui_Manager();
	virtual ~CImgui_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Tick(_float fTimeDelta);
	void Render();
	void Show_GuiTick();

public:
	PICKING_TYPE Get_PickingType() {return (PICKING_TYPE)m_PickingType;}

public:
	/* For Debug*/
	void ShowSimpleMousePos(bool* p_open);
	void ShowPickedObjLayOut(bool* p_open);
	void ShowPickedObj();
	void Show_PopupBox();

	/* For Terrain Tool */
	void Set_Terrain_Map();
	void Set_Terrain_Shape();

	/* For Model Tool */
	void Set_Object_Map();
	void Set_File_Path_Dialog();
	void Show_ModelList();
	void Show_CurrentModelList();
	void BrowseForFolder();
	void Set_FilePath();
	void Set_LayerTag();
	void Set_Macro();

	/* For Navigation Tool */
	void Set_Navigation();
	void Save_Navigation();
	void Load_Navigation();

public:
	void Create_Model(const _tchar* pPrototypeTag, const _tchar* pLayerTag, _bool bCreatePrototype = false);
	void Read_Objects_Name( _tchar* cFolderPath);
	void Add_TempTag(_tchar* TempTag) { m_TempLayerTags.push_back(TempTag); }
	
private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	_bool m_bShowSimpleMousePos = false;
	_bool m_bShowPickedObject = false;
	_bool m_bShow_app_style_editor = false;
	_bool m_bFilePath = false;
	_bool m_bSave = false;
	_bool m_bLoad = false;
	LEVEL m_iCurrentLevel = LEVEL_GAMEPLAY;
private:
	/* For Terrain Manager */
	CTerrain_Manager*						m_pTerrain_Manager = nullptr;
	CTerrain_Manager::TERRAINDESC			m_TerrainDesc;
	CTerrain_Manager::TERRAINSHAPEDESC		m_TerrainShapeDesc;
	_bool									m_bTerrain_Show = true;

	/* For Object */
	_tchar									m_pFilePath[MAX_PATH] = L"../../../Bin/Resources/Meshes/";
	CModelManager*							m_pModel_Manager = nullptr;
	vector<string>							m_stLayerTags;
	vector<const _tchar*>					m_TempLayerTags;
	CNonAnim::NONANIMDESC					m_InitDesc;
	_int									m_iCreatedSelected = 0;
	_int									m_iSeletecLayerNum = 0;

	OBJID									m_eObjID = OBJ_END;
	_int									m_iObjectList = 0;
	_float									m_fDist = 1.f;


	/* For Picking */
	_float3									m_vPickedObjPos = _float3(1.f, 1.f, 1.f);
	_float3									m_vPickedObjScale = _float3(1.f, 1.f, 1.f);
	_bool									m_bPickingMode = false;
	_int									m_PickingType = 0;
	_float3									m_vPickedRotAxis = _float3(0.f, 1.f, 0.f);
	_float									m_fRotAngle = 0.f;


	/*For Navigation*/
	CNavigation_Manager*					m_pNavigation_Manager = nullptr;
	_int									m_iCellIndex = 0;
	_int									m_iCellType = 0;
	_float3									m_fClickPoint = _float3(0.f, 0.f, 0.f);

public:
	virtual void Free() override;
};

END
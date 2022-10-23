#pragma once

#include "Client_Defines.h"
#include "Base.h"

#include "Level_Manager.h"
#include "Terrain_Manager.h"
#include "ModelManager.h"

BEGIN(Engine)
END

BEGIN(Client)

class CImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager)

public:
	enum PICKING_TYPE { PICKING_OBJECT, PICKING_TERRAIN };

private:
	CImgui_Manager();
	virtual ~CImgui_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Tick(_float fTimeDelta);
	void Render();
	void ShowGui();

public:
	PICKING_TYPE Get_PickingType() {return (PICKING_TYPE)m_PickingType;}

public:
	void Object_Map();
	void Terrain_Map();
	void Set_Terrain_Shape();
	void ShowSimpleMousePos(bool* p_open);
	void ShowPickedObjLayOut(bool* p_open);
	void ShowModelList(bool* p_open);


public:
	void Create_Model(const _tchar* pPrototypeTag, const _tchar* pLayerTag);
	void Read_Objects_Name( _tchar* cFolderPath);

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	_bool m_bShowSimpleMousePos = false;
	_bool m_bShowPickedObject = false;
	_bool m_bShowModelList = false;
	_bool m_bShow_app_style_editor = false;
	_bool m_bFilePath = false;

private:
	/* For Terrain Manager */
	CTerrain_Manager* m_pTerrain_Manager = nullptr;
	CTerrain_Manager::TERRAINDESC  m_TerrainDesc;
	CTerrain_Manager::TERRAINSHAPEDESC  m_TerrainShapeDesc;
	_bool m_bTerrain_Show = true;
	_int m_PickingType = 0;
	

	/* For Object */
	CModelManager* m_pModel_Manager = nullptr;
	OBJID m_eObjID = OBJ_END;
	_int m_iObjectList = 0;


	/* For Picking */
	_float3 m_vPickedObjPos = _float3(1.f, 1.f, 1.f);
	_float3 m_vPickedObjScale = _float3(1.f, 1.f, 1.f);
	_float3 TempPos2 = _float3(0.f, 0.f, 0.f);
	_float3 TempPos3;
	_float fDist = 1.f;

public:
	virtual void Free() override;
};

END
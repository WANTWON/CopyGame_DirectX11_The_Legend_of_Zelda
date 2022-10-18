#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

class CTerrain_Manager final : public CBase
{
	DECLARE_SINGLETON(CTerrain_Manager)

public:
	typedef struct TerrainTagDesc
	{
		_int	m_iPositionX = 0;
		_int	m_iPositionZ = 0;
		_int	m_iVerticeNumX = 10;
		_int	m_iVerticeNumZ = 10;
		_float  m_fHeight = 0;
		_bool	m_bShowWireFrame = false;
		_bool	m_bTestShowTerrain = false;

	}TERRAINDESC;

	typedef struct TerrainShapingTagDesc
	{
		_float  fHeight = 0;
		_float	fRadius = 0.f;
		_float	fSharp = 0.f;

	}TERRAINSHAPEDESC;


private:
	CTerrain_Manager();
	virtual ~CTerrain_Manager() = default;

public:
	void Set_TerrainDesc(TERRAINDESC* eTerrainDesc) { memcpy(&m_TerrainDesc, eTerrainDesc, sizeof(TERRAINDESC)); }
	void Set_TerrainShapeDesc(TERRAINSHAPEDESC* eTerrainDesc) { memcpy(&m_TerrainShapeDesc, eTerrainDesc, sizeof(TERRAINSHAPEDESC)); }
	void Set_bWireFrame(_bool type) { m_TerrainDesc.m_bShowWireFrame = type; }
	void Set_TerrainShow(_bool type) { m_bTerrainShow = type; }
	void Set_MoveOffset(_int iOffset ) { m_iMoveOffset = iOffset; }
	void Set_PickingWorldPos(_float3 vPickingPos) {m_vMousePickPos = vPickingPos;}
	TERRAINDESC Get_TerrainDesc() const { return m_TerrainDesc; }
	TERRAINSHAPEDESC Get_TerrainShapeDesc() const { return m_TerrainShapeDesc; }
	_bool Get_TerrainShow() { return m_bTerrainShow; }
	_float3 Get_PickingPos() { return m_vMousePickPos; }
	_int Get_MoveOffset() { return m_iMoveOffset; }


public:
	HRESULT Create_Terrain(LEVEL eLevel, const _tchar* pLayerTag);

private:
	TERRAINDESC		m_TerrainDesc;
	TERRAINSHAPEDESC m_TerrainShapeDesc;
	_bool m_bTerrainShow = true;
	_float3 m_vMousePickPos = _float3(0,0,0);
	_int m_iMoveOffset = 1;

public:
	void Free() override;
};

END
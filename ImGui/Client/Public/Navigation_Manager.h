#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "NonAnim.h"
#include "Cell.h"

BEGIN(Engine)
class CCell;
class CShader;
END


BEGIN(Client)
class CNavigation_Manager final : public CBase
{
	DECLARE_SINGLETON(CNavigation_Manager)

public:
	enum SYMBOL { SYMBOL1,  SYMBOL2, SYMBOLEND};


public:
	CNavigation_Manager();
	virtual ~CNavigation_Manager() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Render();

public:
	_uint Get_CellsSize() { return m_Cells.size(); }
	CCell* Get_Cell();
	CCell* Get_Cell(_uint iIndex);
	void Set_CilckedCellIndex(_uint iIndex) { m_iClickedCellIndex = iIndex; }
	void Set_CellType(CCell::CELLTYPE eType) { m_Cells[m_iClickedCellIndex]->Set_CellType(eType); }
	CCell::CELLTYPE Get_SellType() { return m_Cells[m_iClickedCellIndex]->Get_CellType(); }

public:
	HRESULT Add_ClickedSymbol(_float3 vClickPos, SYMBOL Symboltype);
	HRESULT Add_Cell(_float3* vPoss, _bool bCheckOverlap = false);
	void Click_Position(_vector vPosition);
	void Clear_ClickedPosition();
	void Update_ClickedPosition(_float3 position);
	void Clear_Cells();
	void Cancle_Cell();
	void Sort_CellByPosition(_float3 * vPoss);
	void Sort_CellByDot(_float3 * vPoss);

private:
	CCell* Find_PickingCell();
	_float3 Find_MinDistance(_vector vPosition);
	_bool Check_Sell(_float3 * vPoss);

private:
	_uint		m_iClickedCellIndex = 0;
	_float3		m_vClickedPos;
	_float		m_fMinDistance = MAX_NUM;

private:
	vector<class CCell*>			m_Cells;
	typedef vector<class CCell*>	CELLS;
	CNonAnim*						m_pClickedSymbol[SYMBOLEND] = {nullptr};
	vector<_float3>					m_vClickedPoints;

private:  /*For Render */
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pContext;
	class CShader*			m_pShader = nullptr;

public:
	virtual void Free() override;
};

END
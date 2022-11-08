#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCell;
class CShader;
END


BEGIN(Client)
class CNavigation_Manager final : public CBase
{
	DECLARE_SINGLETON(CNavigation_Manager)

public:
	CNavigation_Manager();
	virtual ~CNavigation_Manager() = default;

public:
	void Click_Position(_vector vPosition);
	void Clear_ClickedPosition();
	HRESULT Add_Cell(_float3* vPoss, _bool bCheckOverlap = false);
	void Clear_Cells();

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Render();

private:
	_uint		m_iClickedCellIndex = 0;
	_float3		m_vClickedPos;

	vector<class CCell*>			m_Cells;
	typedef vector<class CCell*>	CELLS;

	map<_float, _float3*> m_TempCells;
	vector<_float3> m_fvClickedPoss;

	_bool	m_bClickVertexModel = false;

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;
	class CShader*			m_pShader = nullptr;

public:
	virtual void Free() override;
};

END
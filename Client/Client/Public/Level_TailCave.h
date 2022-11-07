#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Engine)
class CCollider;
END


BEGIN(Client)

class CLevel_TailCave final : public CLevel
{
private:
	CLevel_TailCave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_TailCave() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Effect(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Monster(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Object(const _tchar* pLayerTag);


private:
	void Check_Solved_Puzzle(); 


private:
	_bool m_bDoorOpen = true;
	vector<_bool>					m_vecIsSolved = { false };
	vector<CCollider*>				m_pAABBCom = { nullptr };
	vector<CCollider*>				m_pOBBCom = { nullptr };
	vector<CCollider*>				m_pSPHERECom = { nullptr };

public:
	static CLevel_TailCave* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END


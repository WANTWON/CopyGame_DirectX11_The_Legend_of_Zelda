#include "stdafx.h"
#include "..\Public\Navigation_Manager.h"
#include "GameInstance.h"
#include "Cell.h"
#include "VIBuffer_Navigation.h"

IMPLEMENT_SINGLETON(CNavigation_Manager)


CNavigation_Manager::CNavigation_Manager()
{
}

HRESULT CNavigation_Manager::Initialize(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	if (nullptr == pDevice || nullptr == pContext)
		return E_FAIL;

	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../../Bin/ShaderFiles/Shader_Navigation.hlsl"), VTXPOS_DECLARATION::Elements, VTXPOS_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_Cells.reserve(0);

	return S_OK;
}


void CNavigation_Manager::Click_Position(_vector vPosition)
{
	//if (m_Cells.size() != 0)
	//{
	//	//CCell* pCell = Find_PickingCell();
	//	//if (nullptr == pCell)
	//		//return;

	//	int a = 0;
	//}
	//else
	//{

	_float3 vClickPosition;
	XMStoreFloat3(&vClickPosition, vPosition);;

	if (2 < m_fvClickedPoss.size())
		return;

	m_fvClickedPoss.push_back(vClickPosition);
	//}


	if (3 == m_fvClickedPoss.size())
	{
		_float3 vPoss[3];
		for (_int i = 0; i < 3; ++i)
			vPoss[i] = m_fvClickedPoss[i];

		Add_Cell(vPoss, true);
		Clear_ClickedPosition();
	}
}

void CNavigation_Manager::Clear_ClickedPosition()
{
	m_fvClickedPoss.clear();
}

HRESULT CNavigation_Manager::Add_Cell(_float3 * vPoss, _bool bCheckOverlap)
{
	
	// 추가한다.
	CCell*			pCell = CCell::Create(m_pDevice, m_pContext, vPoss, m_Cells.size());
	if (nullptr == pCell)
		return E_FAIL;
	m_Cells.push_back(pCell);

	return S_OK;
}

void CNavigation_Manager::Clear_Cells()
{
	for (auto& pCell : m_Cells)
		Safe_Release(pCell);
	m_Cells.clear();
}


HRESULT CNavigation_Manager::Render()
{
	if (m_Cells.size() == 0)
		return S_OK;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	_float4x4			WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());
	if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;


	_float3 vCamPos = *(_float3*)&pGameInstance->Get_CamPosition();

	RELEASE_INSTANCE(CGameInstance);


	for (auto& pCell : m_Cells)
	{
		if (nullptr != pCell)
		{

			m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
			m_pShader->Set_RawValue("g_vColor", &_float4(1.f, 1.f, 1.f, 1.f), sizeof(_float4));
			m_pShader->Begin(0);
			pCell->Render();
		}

	}
	return S_OK;
}

void CNavigation_Manager::Free()
{
	Clear_Cells();


	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pShader);
}

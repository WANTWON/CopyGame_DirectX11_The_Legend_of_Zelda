#include "stdafx.h"
#include "..\Public\Navigation_Manager.h"
#include "GameInstance.h"
#include "Cell.h"
#include "VIBuffer_Navigation.h"
#include "Picking.h"

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
	_float3 vClickPosition;

	//셀이 1개 이상 있으면 점을 찍을 때 가장 가까운 점이 있는지 확인.
	//찍은 점의 위치가 기존 셀의 점의 위치와 비슷하다면 해당 점을 찍게한다.
	vClickPosition = Find_MinDistance(vPosition);

	//만약 순회해서 찾은 셀의 점이 거리가 멀다면 그냥 기존에 찍은 점을 pushback하게 한다.
	if (m_fMinDistance > 2.f)
		XMStoreFloat3(&vClickPosition, vPosition);

	m_fMinDistance = MAX_NUM;
	if (2 < m_vClickedPoints.size())
		return;

	m_vClickedPoints.push_back(vClickPosition);

	//찍은 점의 개수가 3개가 된다면. 삼각형이 그려지니 Cell에 추가한다.
	if (3 == m_vClickedPoints.size())
	{
		_float3 vPoss[3];
		for (_int i = 0; i < 3; ++i)
			vPoss[i] = m_vClickedPoints[i];

		Add_Cell(vPoss, true);
		Clear_ClickedPosition();
	}
}

void CNavigation_Manager::Clear_ClickedPosition()
{
	m_vClickedPoints.clear();
	m_fMinDistance = MAX_NUM;
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

CCell * CNavigation_Manager::Find_PickingCell()
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 찍은 점에서 거리가 가장 가까운 셀을 구한다. 셀을 이웃하게 찍어야 하니까.
	XMVECTOR		vRayDir, vRayPos;

	vRayDir = pGameInstance->Get_RayDir();
	vRayDir = XMVector3Normalize(vRayDir);
	vRayPos = pGameInstance->Get_RayPos();
	vRayPos = XMVectorSetW(vRayPos, 1.f);

	_float fMinDistance = MAX_NUM;
	CCell* pMinCell = nullptr;

	for (_uint i = 0; i < m_Cells.size(); ++i)
	{

		CCell* pCell = m_Cells[i];
		if (nullptr == pCell)
			continue;

		_float3 vPoint[CCell::POINT_END];

		vPoint[CCell::POINT_A] = pCell->Get_PointValue(CCell::POINT_A);
		vPoint[CCell::POINT_B] = pCell->Get_PointValue(CCell::POINT_B);
		vPoint[CCell::POINT_C] = pCell->Get_PointValue(CCell::POINT_C);

		_float		fDist;

		_vector vTemp_1 = XMLoadFloat3(&vPoint[CCell::POINT_A]);
		vTemp_1 = XMVectorSetW(vTemp_1, 1.f);
		_vector vTemp_2 = XMLoadFloat3(&vPoint[CCell::POINT_B]);
		vTemp_2 = XMVectorSetW(vTemp_2, 1.f);
		_vector vTemp_3 = XMLoadFloat3(&vPoint[CCell::POINT_C]);
		vTemp_3 = XMVectorSetW(vTemp_3, 1.f);

		if (true == TriangleTests::Intersects(vRayPos, vRayDir, vTemp_1, vTemp_2, vTemp_3, fDist))
		{
			if (fMinDistance > fDist)
			{
				fMinDistance = fDist;
				pMinCell = m_Cells[i];
			}
		}

	}

	if (fMinDistance != MAX_NUM)
	{
		RELEASE_INSTANCE(CGameInstance);
		return pMinCell;
	}

	RELEASE_INSTANCE(CGameInstance);
	return nullptr;
}

_float3 CNavigation_Manager::Find_MinDistance(_vector vPosition)
{
	// 셀이 없다면 무조건 그냥 찍은 좌표로 준다.
	_float3 vClickPosition;
	XMStoreFloat3(&vClickPosition, vPosition);

	if (m_Cells.size() == 0)
		return vClickPosition;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	XMVECTOR		vRayDir, vRayPos;

	vRayDir = pGameInstance->Get_RayDir();
	vRayDir = XMVector3Normalize(vRayDir);
	vRayPos = pGameInstance->Get_RayPos();
	vRayPos = XMVectorSetW(vRayPos, 1.f);

	//셀을 순회한다.
	for (_uint i = 0; i < m_Cells.size(); ++i)
	{
		CCell* pCell = m_Cells[i];
		if (nullptr == pCell)
			continue;

		_vector vPoint[CCell::POINT_END];

		//셀마다 각 포인트 A,B,C를 받아온다
		vPoint[CCell::POINT_A] = XMLoadFloat3(&pCell->Get_PointValue(CCell::POINT_A));
		vPoint[CCell::POINT_A] = XMVectorSetW(vPoint[CCell::POINT_A], 1.f);
		vPoint[CCell::POINT_B] = XMLoadFloat3(&pCell->Get_PointValue(CCell::POINT_B));
		vPoint[CCell::POINT_B] = XMVectorSetW(vPoint[CCell::POINT_B], 1.f);
		vPoint[CCell::POINT_C] = XMLoadFloat3(&pCell->Get_PointValue(CCell::POINT_C));
		vPoint[CCell::POINT_C] = XMVectorSetW(vPoint[CCell::POINT_C], 1.f);


		//내가 피킹한 마우스 Postion과 셀과의 거리를 구한다.
		//그 중에서 가장 작은 값을 체크
		_float		fDist;

		_float fDis_A = XMVectorGetX(XMVector3Length(vPoint[CCell::POINT_A] - vPosition));
		_float fDis_B = XMVectorGetX(XMVector3Length(vPoint[CCell::POINT_B] - vPosition));
		_float fDis_C = XMVectorGetX(XMVector3Length(vPoint[CCell::POINT_C] - vPosition));
		_float fMinDistance = min(min(fDis_A, fDis_B), fDis_C);

		//만약 이전에 순회한 셀보다 거리가 크다면 continue,
		//더 작다면 해당 점을 click 한 점으로 Store해준다.
		if (m_fMinDistance < fMinDistance)
			continue;

		m_fMinDistance = fMinDistance;
		if (fDis_A == m_fMinDistance)
			XMStoreFloat3(&vClickPosition, vPoint[CCell::POINT_A]);
		else if (fDis_B == m_fMinDistance)
			XMStoreFloat3(&vClickPosition, vPoint[CCell::POINT_B]);
		else if (fDis_C == m_fMinDistance)
			XMStoreFloat3(&vClickPosition, vPoint[CCell::POINT_C]);

	}

	RELEASE_INSTANCE(CGameInstance);

	return vClickPosition;
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
			m_pShader->Set_RawValue("g_vColor", &_float4(1.f, 0.f, 0.f, 1.f), sizeof(_float4));
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

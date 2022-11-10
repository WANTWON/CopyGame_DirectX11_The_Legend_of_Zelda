#include "stdafx.h"
#include "..\Public\Data_Manager.h"

IMPLEMENT_SINGLETON(CData_Manager)

CData_Manager::CData_Manager()
{
}


void CData_Manager::Free()
{
	for (auto& iter : m_pModelTags)
		Safe_Delete(iter);

	m_pModelTags.clear();
}

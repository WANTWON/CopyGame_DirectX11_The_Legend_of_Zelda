#include "stdafx.h"
#include "..\Public\Collision_Manger.h"
#include "BaseObj.h"

IMPLEMENT_SINGLETON(CCollision_Manager);


CCollision_Manager::CCollision_Manager()
{
}

void CCollision_Manager::Add_CollisionGroup(COLLSIONGROUP CollisionGroup, CBaseObj * pGameObject)
{
	if (nullptr == pGameObject)
		return;
	m_GameObjects[CollisionGroup].push_back(pGameObject);
}

void CCollision_Manager::Out_CollisionGroup(COLLSIONGROUP CollisionGroup, CBaseObj * pGameObject)
{
	auto& iter = m_GameObjects[CollisionGroup].begin();
	while (iter != m_GameObjects[CollisionGroup].end())
	{
		if (*iter == pGameObject)
			iter = m_GameObjects[CollisionGroup].erase(iter);
		else
			++iter;
	}
}

void CCollision_Manager::Clear_CollisionGroup(COLLSIONGROUP CollisionGroup)
{
	m_GameObjects[CollisionGroup].clear();
}

_bool CCollision_Manager::CollisionwithGroup(COLLSIONGROUP CollisionGroup, CCollider* pCollider)
{
	for (auto& iter : m_GameObjects[CollisionGroup])
	{
		CCollider* pTargetCollider = iter->Get_Collider();
		if (iter == nullptr)
			continue;

		if (pCollider->Collision(pTargetCollider))
			return true;
	}

	return false;
}

void CCollision_Manager::CollisionwithBullet()
{
	//서로 상호작용
	//플레이어 불렛 - 몬스터 충돌하면 몬스터 -> TakeDamage : 플레이어 불렛 -> Dead
	//몬스터 불렛 - 플레이어 플레이어 ->TakeDamege : 불렛 Dead 
}

void CCollision_Manager::Update_Collider()
{
	for (_uint i = 0; i < COLLISION_END; ++i)
	{
		for (auto& iter : m_GameObjects[i])
			iter->Update_Collider();
	}




}

void CCollision_Manager::Free()
{
	for (_uint i = 0; i < COLLISION_END; ++i)
	{
		m_GameObjects[i].clear();
	}
}

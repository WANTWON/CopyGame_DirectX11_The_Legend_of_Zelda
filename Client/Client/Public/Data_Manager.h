#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CData_Manager : public CBase
{
	DECLARE_SINGLETON(CData_Manager)

public:
	CData_Manager();
	virtual ~CData_Manager() = default;
public:
	void Add_ModelTag(_tchar* pModeltag) { m_pModelTags.push_back(pModeltag); }


private:
	vector<_tchar*>		m_pModelTags;

public:
	virtual void Free() override;
};

END
#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)
class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)
public:
	enum UITYPE { UI_INVEN, UI_MAP, UI_OPTION, UI_END};

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	void Set_UI_Open() {m_bUIOpen = !m_bUIOpen;}
	_bool Get_UI_Open() { return m_bUIOpen; }

private:
	_bool m_bUIOpen = false;

public:
	void Free() override;
};

END
#include "stdafx.h"
#include "..\Public\Imgui_Manager.h"
#include "imgui.h"
#include "GameInstance.h"
#include "PickingMgr.h"
#include "BaseObj.h"

IMPLEMENT_SINGLETON(CImgui_Manager)

CImgui_Manager::CImgui_Manager()
	: m_pTerrain_Manager(CTerrain_Manager::Get_Instance())
{
	Safe_AddRef(m_pTerrain_Manager);
}


HRESULT CImgui_Manager::Initialize(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{

	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;


	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	return S_OK;
}

void CImgui_Manager::Tick(_float fTimeDelta)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTerrain_Manager::TERRAINDESC TerrainDesc = m_pTerrain_Manager->Get_TerrainDesc();

	if (pGameInstance->Key_Up(DIK_LEFT))
		TerrainDesc.TerrainDesc.m_iPositionX -= m_pTerrain_Manager->Get_MoveOffset();

	if (pGameInstance->Key_Up(DIK_RIGHT))
		TerrainDesc.TerrainDesc.m_iPositionX += m_pTerrain_Manager->Get_MoveOffset();

	if (pGameInstance->Key_Up(DIK_UP))
		TerrainDesc.TerrainDesc.m_iPositionZ += m_pTerrain_Manager->Get_MoveOffset();

	if (pGameInstance->Key_Up(DIK_DOWN))
		TerrainDesc.TerrainDesc.m_iPositionZ -= m_pTerrain_Manager->Get_MoveOffset();

	if (pGameInstance->Key_Up(DIK_SPACE))
	{
		LEVEL pLevelIndex = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

		if (FAILED(m_pTerrain_Manager->Create_Terrain(pLevelIndex, TEXT("Layer_Terrain"))))
			return;
	}

	m_pTerrain_Manager->Set_TerrainDesc(&TerrainDesc);
	RELEASE_INSTANCE(CGameInstance);

	ShowGui();




	ImGui::EndFrame();

}

void CImgui_Manager::Render()
{
	// Rendering


	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());



}

void CImgui_Manager::ShowGui()
{
	ImGui::Begin(u8"Editor", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar
		| ImGuiWindowFlags_AlwaysVerticalScrollbar);


	//SettingMenu();
	//메뉴바
	if (ImGui::BeginMenuBar())
	{
		// 메뉴
		if (ImGui::BeginMenu("File"))
		{

			if (ImGui::MenuItem("save"))
			{
				int a = 0;
			}

			ImGui::Separator();
			if (ImGui::MenuItem("open"))
			{
				int a = 0;
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Debug"))
		{
			ImGui::MenuItem("Show Mouse Pos", NULL, &m_bShowSimpleMousePos);
			ImGui::Separator();
			ImGui::MenuItem("Show Picked Object", NULL, &m_bShowPickedObject);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}


	const char* Level[] = { "Level_GamePlay", "Level_Boss" };
	static int iCurrentLevel = 0;
	ImGui::Combo("##0", &iCurrentLevel, Level, IM_ARRAYSIZE(Level));

	ImGui::NewLine();


	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Terrain Tool"))
		{
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Picking for Object"); ImGui::SameLine();
			ImGui::RadioButton("##Picking for Object", &m_PickingType, PICKING_OBJECT); ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Picking for Shaping Terrain"); ImGui::SameLine();
			ImGui::RadioButton("##Picking for Shaping Terrain", &m_PickingType, PICKING_TERRAIN);

			Terrain_Map();
			Set_Terrain_Shape();
			Object_Map();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Camera Tool"))
		{
			ImGui::Text("This is the Camera Tool tab!\nblah blah blah blah blah");
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Tool,,Tool"))
		{
			ImGui::Text("This is the Tool,,Tool Tool tab!\nblah blah blah blah blah");
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	if (m_bShowSimpleMousePos)      ShowSimpleMousePos(&m_bShowSimpleMousePos);
	if (m_bShowPickedObject)      ShowPickedObjLayOut(&m_bShowPickedObject);

	ImGui::End();

}


void CImgui_Manager::Terrain_Map()
{

	_bool bCreateTerrain;

	ImGui::GetIO().NavActive = false;
	ImGui::GetIO().WantCaptureMouse = true;

	if (!ImGui::CollapsingHeader("Terrain_Map"))
		return;

	CTerrain_Manager::TERRAINDESC TerrainDesc = m_pTerrain_Manager->Get_TerrainDesc();

	static _int iNumVertice[2] = { TerrainDesc.TerrainDesc.m_iVerticeNumX, TerrainDesc.TerrainDesc.m_iVerticeNumZ };
	ImGui::Text("NumVerticeX / NumVerticeZ");
	ImGui::SameLine(0, 10);
	ImGui::InputInt2("##1", iNumVertice, 10);
	TerrainDesc.TerrainDesc.m_iVerticeNumX = iNumVertice[0];
	TerrainDesc.TerrainDesc.m_iVerticeNumZ = iNumVertice[1];

	ImGui::Text("Position X");
	ImGui::SameLine();
	ImGui::DragInt("##PositionX", &TerrainDesc.TerrainDesc.m_iPositionX);

	ImGui::Text("Position Z");
	ImGui::SameLine();
	ImGui::DragInt("##PositionZ", &TerrainDesc.TerrainDesc.m_iPositionZ);

	ImGui::Text("Position Y");
	ImGui::SameLine();
	ImGui::DragFloat("##PositionY", &TerrainDesc.TerrainDesc.m_fHeight, 1.f, -10, 10);

	static _int iOffset = m_pTerrain_Manager->Get_MoveOffset();
	ImGui::Text("Move Offset");
	ImGui::SameLine();
	ImGui::InputInt("##MoveOffset", &iOffset);
	m_pTerrain_Manager->Set_MoveOffset(iOffset);

	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::ImColor(0.f, 0.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::ImColor(0.f, 1.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::ImColor(1.f, 0.f, 0.f));
	bCreateTerrain = ImGui::Button("Create Terrain");
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::Checkbox("WireFrame", &TerrainDesc.m_bShowWireFrame);
	m_pTerrain_Manager->Set_bWireFrame(TerrainDesc.m_bShowWireFrame);
	ImGui::SameLine();
	ImGui::Checkbox("Show", &m_bTerrain_Show);
	m_pTerrain_Manager->Set_TerrainShow(m_bTerrain_Show);
	ImGui::NewLine();

	m_pTerrain_Manager->Set_TerrainDesc(&TerrainDesc);

	if (bCreateTerrain)
	{
		LEVEL pLevelIndex = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

		if (FAILED(m_pTerrain_Manager->Create_Terrain(pLevelIndex, TEXT("Layer_Terrain"))))
			return;

		bCreateTerrain = false;
	}


}


void CImgui_Manager::Object_Map()
{
	_bool ret;

	ImGui::GetIO().NavActive = false;
	ImGui::GetIO().WantCaptureMouse = true;

	if (!ImGui::CollapsingHeader("Object_Map"))
		return;

	static _float Pos[3] = { TempPos2.x , TempPos2.y, TempPos2.z };
	ImGui::Text("set Scale");
	ImGui::SameLine();
	ImGui::InputFloat3("##1", Pos);

	static _float Pos2[3] = { TempPos2.x , TempPos2.y, TempPos2.z };
	ImGui::Text("set Rotation AXIS");
	ImGui::SameLine();
	ImGui::InputFloat3("##2", Pos2);
	//Pos2.x = m_TerrainInfo.vPos.x;
	//Pos2.y = m_TerrainInfo.vPos.y;
	//Pos2.z = m_TerrainInfo.vPos.z;

	static _float Pos3[3] = { TempPos2.x , TempPos2.y };
	ImGui::Text("Rotaion Angle / dist");
	ImGui::SameLine();
	ImGui::InputFloat2("##3", Pos3);

	//	Pos3.y = 1.f;

	ImGui::Text("vPos X : %f", TempPos2.x);
	ImGui::SameLine();
	ImGui::Text("vPos Y : %f", TempPos2.x);
	ImGui::SameLine();
	ImGui::Text("vPos Z : %f", TempPos2.x);
	ImGui::Text("fAngle : %f", TempPos2.x);
	ImGui::SameLine();
	ImGui::Text("Dist : %f", Pos3[1]);
	ImGui::NewLine();

	const char* ObjectID[] = { "OBJ_BACKGROUND", "OBJ_MONSTER", "OBJ_BLOCK", "OBJ_INTERATIVE", "OBJ_UNINTERATIVE", "OBJ_END" };
	static int iObjectID = 5;
	ImGui::Combo("Object_ID", &iObjectID, ObjectID, IM_ARRAYSIZE(ObjectID));
	m_eObjID = (OBJID)iObjectID;


	switch (m_eObjID)
	{

	case Client::OBJ_BACKGROUND:
	{
		const char* ObjectList[] = { "Terrain1", "Terrain2", "Terrain3", "Terrain4" };
		static int ObjectCurrentList = 0; // Here we store our selection data as an index.
		ImGui::Combo("ObjectList", &ObjectCurrentList, ObjectList, IM_ARRAYSIZE(ObjectList));
		break;
	}
	case Client::OBJ_MONSTER:
	{
		const char* ObjectList[] = { "Pig", "Mpbline", "Spider", "Bearger", "Boarwarrior", "Boss" };
		static int ObjectCurrentList = 0; // Here we store our selection data as an index.
		ImGui::Combo("ObjectList", &ObjectCurrentList, ObjectList, IM_ARRAYSIZE(ObjectList));
		break;
	}
	case Client::OBJ_BLOCK:
	{
		const char* ObjectList[] = { "Block1", "Block2", "Block3" };
		static int ObjectCurrentList = 0; // Here we store our selection data as an index.
		ImGui::Combo("ObjectList", &ObjectCurrentList, ObjectList, IM_ARRAYSIZE(ObjectList));
		break;
	}
	case Client::OBJ_INTERATIVE:
	{
		const char* ObjectList[] = { "Grass", "Tree", "Pot", "Tent", "Food", "Flower" };
		static int ObjectCurrentList = 0; // Here we store our selection data as an index.
		ImGui::Combo("ObjectList", &ObjectCurrentList, ObjectList, IM_ARRAYSIZE(ObjectList));
		break;
	}
	case Client::OBJ_UNINTERATIVE:
	{
		const char* ObjectList[] = { "Wall", "Rock", "Deco1", "Deco2", "Deco3" };
		static int ObjectCurrentList = 0; // Here we store our selection data as an index.
		ImGui::Combo("ObjectList", &ObjectCurrentList, ObjectList, IM_ARRAYSIZE(ObjectList));
		break;
	}
	default:
		break;
	}


	//ImGui::Text("OK");
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::ImColor(0.f, 0.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::ImColor(0.f, 1.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::ImColor(1.f, 0.f, 0.f));
	ret = ImGui::Button("Create_Object");
	ImGui::PopStyleColor(3);
	ImGui::NewLine();

}

void CImgui_Manager::Set_Terrain_Shape()
{
	ImGui::GetIO().NavActive = false;
	ImGui::GetIO().WantCaptureMouse = true;

	if (!ImGui::CollapsingHeader("Shape Setting (Height, Sharp, Range)"))
		return;

	ImGui::Text("Height");
	ImGui::SameLine();
	ImGui::DragFloat("##fHeight", &m_TerrainShapeDesc.fHeight);

	ImGui::Text("Radius");
	ImGui::SameLine();
	ImGui::DragFloat("##fRadius", &m_TerrainShapeDesc.fRadius);

	ImGui::Text("Sharp");
	ImGui::SameLine();
	ImGui::DragFloat("##fSharp", &m_TerrainShapeDesc.fSharp);


	m_pTerrain_Manager->Set_TerrainShapeDesc(&m_TerrainShapeDesc);

}

void CImgui_Manager::ShowSimpleMousePos(bool* p_open)
{
	static int location = 0;
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	if (location >= 0)
	{
		const float PAD = 10.0f;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
		ImVec2 work_size = viewport->WorkSize;
		ImVec2 window_pos, window_pos_pivot;
		window_pos.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
		window_pos.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
		window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
		window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		window_flags |= ImGuiWindowFlags_NoMove;
	}
	else if (location == -2)
	{
		// Center window
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		window_flags |= ImGuiWindowFlags_NoMove;
	}
	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	if (ImGui::Begin("Mouse Debug tab", p_open, window_flags))
	{
		ImGui::Text("Mouse Debug");
		ImGui::Separator();
		if (ImGui::IsMousePosValid())
		{
			_float3 vPickingPos = m_pTerrain_Manager->Get_PickingPos();
			ImGui::Text("Mouse UI: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
			ImGui::Text("Mouse World: (%.1f,%.1f,%.1f)", vPickingPos.x, vPickingPos.y, vPickingPos.z);
		}
		else
			ImGui::Text("Mouse Position: <invalid>");
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Custom", NULL, location == -1)) location = -1;
			if (ImGui::MenuItem("Center", NULL, location == -2)) location = -2;
			if (ImGui::MenuItem("Top-left", NULL, location == 0)) location = 0;
			if (ImGui::MenuItem("Top-right", NULL, location == 1)) location = 1;
			if (ImGui::MenuItem("Bottom-left", NULL, location == 2)) location = 2;
			if (ImGui::MenuItem("Bottom-right", NULL, location == 3)) location = 3;
			if (p_open && ImGui::MenuItem("Close")) *p_open = false;
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void CImgui_Manager::ShowPickedObjLayOut(bool * p_open)
{
	CPickingMgr* pPickingMgr = GET_INSTANCE(CPickingMgr);
	CGameObject* pPickedObj = pPickingMgr->Get_PickedObj();


	ImGui::SetNextWindowSize(ImVec2(200, 300), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Picked Object Info", p_open, ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Close")) *p_open = false;
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// Right

		ImGui::BeginGroup();
		ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us

		if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Description"))
			{

				const char* ObjectID[] = { "OBJ_BACKGROUND", "OBJ_MONSTER", "OBJ_BLOCK", "OBJ_INTERATIVE", "OBJ_UNINTERATIVE", "OBJ_END" };
				static int iObjectID = 5;
				
				if (pPickedObj != nullptr)
				{
					iObjectID = dynamic_cast<CBaseObj*>(pPickedObj)->Get_ObjectID();
					DirectX::XMStoreFloat3(&m_vPickedObjPos, dynamic_cast<CBaseObj*>(pPickedObj)->Get_Position());
					m_vPickedObjScale = dynamic_cast<CBaseObj*>(pPickedObj)->Get_Scale();

				}
				else
				{
					iObjectID = 5;
					m_vPickedObjPos = _float3(0.f, 0.f, 0.f);
					m_vPickedObjScale = _float3(1.f, 1.f, 1.f);
				}

				ImGui::Text("OBJECT_ID : ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "%s", ObjectID[iObjectID]);

				ImGui::NewLine();

				ImGui::Text("OBJECT_TYPE : ");
				ImGui::SameLine();
				switch (m_eObjID)
				{
				case Client::OBJ_BACKGROUND:
				{
					const char* ObjectList[] = { "Terrain1", "Terrain2", "Terrain3", "Terrain4" };
					static int ObjectCurrentList = m_iObjectList; // Here we store our selection data as an index.
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%s", ObjectList[ObjectCurrentList]);
					break;
				}
				case Client::OBJ_MONSTER:
				{
					const char* ObjectList[] = { "Pig", "Mpbline", "Spider", "Bearger", "Boarwarrior", "Boss" };
					static int ObjectCurrentList = m_iObjectList; // Here we store our selection data as an index.
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%s", ObjectList[ObjectCurrentList]);
					break;
				}
				case Client::OBJ_BLOCK:
				{
					const char* ObjectList[] = { "Block1", "Block2", "Block3" };
					static int ObjectCurrentList = m_iObjectList; // Here we store our selection data as an index.
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%s", ObjectList[ObjectCurrentList]);
					break;
				}
				case Client::OBJ_INTERATIVE:
				{
					const char* ObjectList[] = { "Grass", "Tree", "Pot", "Tent", "Food", "Flower" };
					static int ObjectCurrentList = m_iObjectList; // Here we store our selection data as an index.
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%s", ObjectList[ObjectCurrentList]);
					break;
				}
				case Client::OBJ_UNINTERATIVE:
				{
					const char* ObjectList[] = { "Wall", "Rock", "Deco1", "Deco2", "Deco3" };
					static int ObjectCurrentList = m_iObjectList; // Here we store our selection data as an index.
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%s", ObjectList[ObjectCurrentList]);
					break;
				}
				case Client::OBJ_END:
				{
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "NONE");
					break;
				}
				default:
					break;
				}

				ImGui::NewLine();
				ImGui::Text("Position (");
				ImGui::SameLine();
				ImGui::Text("%f, %f , %f )", m_vPickedObjPos.x, m_vPickedObjPos.y, m_vPickedObjPos.z);

				ImGui::NewLine();
				ImGui::Text("Scale (");
				ImGui::SameLine();
				ImGui::Text("%f, %f , %f )", m_vPickedObjScale.x, m_vPickedObjScale.y, m_vPickedObjScale.z);

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Setting"))
			{
				if (pPickedObj != nullptr)
				{
					DirectX::XMStoreFloat3(&m_vPickedObjPos, dynamic_cast<CBaseObj*>(pPickedObj)->Get_Position());
					m_vPickedObjScale = dynamic_cast<CBaseObj*>(pPickedObj)->Get_Scale();

				}
				else
				{
					m_vPickedObjPos = _float3(0.f, 0.f, 0.f);
					m_vPickedObjScale = _float3(1.f, 1.f, 1.f);
				}


				ImGui::BulletText("Position");
				ImGui::Text("Position X");
				ImGui::SameLine();
				ImGui::DragFloat("##PositionX", &m_vPickedObjPos.x);
	
				ImGui::Text("Position Z");
				ImGui::SameLine();
				ImGui::DragFloat("##PositionZ", &m_vPickedObjPos.z);

				ImGui::Text("Position Y");
				ImGui::SameLine();
				ImGui::DragFloat("##PositionY", &m_vPickedObjPos.y, 1.f, -10, 10);

				ImGui::NewLine();

				ImGui::BulletText("Scale");

				static _float Pos[3] = { m_vPickedObjScale.x, m_vPickedObjScale.y,  m_vPickedObjScale.z };
				Pos[0] = m_vPickedObjScale.x;
				Pos[1] = m_vPickedObjScale.y;
				Pos[2] = m_vPickedObjScale.z;

				ImGui::Text("Scale");
				ImGui::SameLine();
				ImGui::InputFloat3("##SettingScale", Pos);
				m_vPickedObjScale = _float3(Pos[0], Pos[1], Pos[2]);

				if (pPickedObj != nullptr)
				{
					_vector vSettingPosition = DirectX::XMLoadFloat3(&m_vPickedObjPos);
					vSettingPosition = XMVectorSetW(vSettingPosition, 1.f);
					dynamic_cast<CBaseObj*>(pPickedObj)->Set_State(CTransform::STATE_POSITION, vSettingPosition);
					dynamic_cast<CBaseObj*>(pPickedObj)->Set_Scale(m_vPickedObjScale);

				}

				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::EndChild();
		if (ImGui::Button("Reset")) {}
		ImGui::SameLine();
		if (ImGui::Button("Save")) {}
		ImGui::EndGroup();
	}

	RELEASE_INSTANCE(CPickingMgr);

	ImGui::End();
}

void CImgui_Manager::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//CleanupDeviceD3D();
	//::DestroyWindow(hwnd);
	//::UnregisterClass(wc.lpszClassName, wc.hInstance);

	Safe_Release(m_pTerrain_Manager);
	CTerrain_Manager::Get_Instance()->Destroy_Instance();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

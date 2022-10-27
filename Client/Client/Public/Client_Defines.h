#pragma once

namespace Client
{
	const unsigned int g_iWinSizeX = 1280;
	const unsigned int g_iWinSizeY = 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };
	enum OBJID { OBJ_BACKGROUND, OBJ_MONSTER, OBJ_BLOCK, OBJ_INTERATIVE, OBJ_UNINTERATIVE, OBJ_END };
	enum SHADER_ID { SHADER_DEFAULT, SHADER_WIREFRAME };
	enum SHADERUI_ID { UI_ALPHATEST, UI_ALPHABLEND, UI_PICKED };

	enum DIR { MINUS, NONE, PLUS };
}

#include <process.h>

// Client_Defines.h

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;

using namespace Client;
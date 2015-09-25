/*
 * Editor.h
 *
 *  Created on: Sep 2, 2015
 *      Author: mathi
 */

#ifndef APPLICATION_EDITOR_EDITOR_H_
#define APPLICATION_EDITOR_EDITOR_H_

#include "../Application.h"

// Lua version 5.3.1
#include "Lua/lua.h"
#include "Lua/lapi.h"
#include "Lua/lauxlib.h"
#include "Lua/lualib.h"

lua_State *Lua_State;
struct Vector4f Lua_Window;
_Bool App_UsingDisplay, App_UseFullScreen, Lua_hasResize;
_Bool Lua_giveWidth, Lua_giveHeight;
_Bool Lua_requestClose;

int Lua_Close(lua_State *L);

void Editor_Init();
void Editor_Render();
void Editor_Close();

#include "Editor/Code_Manager.h"
#include "Console/Console.h"
#include "Library/LuaLibrary.h"

#endif /* APPLICATION_EDITOR_EDITOR_H_ */

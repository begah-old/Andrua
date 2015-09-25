#ifndef LUALIBRARY_H_INCLUDED
#define LUALIBRARY_H_INCLUDED

#include "../Editor.h"

/* File that act's as lua's stdout and stderr */
FILE *LuaLibrary_Log;

int LuaLibrary_OpenKeyboard(lua_State *L);
int LuaLibrary_CloseKeyboard(lua_State *L);

void LuaLibrary_Particles_Load();
void LuaLibrary_Particles_Render();
void LuaLibrary_Particles_Close();

void LuaLibrary_Image_Load();
void LuaLibrary_Image_Render();
void LuaLibrary_Image_Close();

void LuaLibrary_Gui_Load();
void LuaLibrary_Gui_Render();
void LuaLibrary_Gui_Close();

void LuaLibrary_Animation_Load();
void LuaLibrary_Animation_Render();
void LuaLibrary_Animation_Close();

void LuaLibrary_Load(FILE *F);
void LuaLibrary_Render();
void LuaLibrary_Close();

#endif // LUALIBRARY_H_INCLUDED

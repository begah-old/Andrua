/*
 * LuaLibrary.c
 *
 *  Created on: Sep 2, 2015
 *      Author: mathi
 */

#include "LuaLibrary.h"

/* Gives current FPS */
static int Lua_getFPS(lua_State *L)
{
	lua_pushnumber(L, Game_FPS);
	return 1;
}

/* Give Executable Path */
static int Lua_getPath(lua_State *L)
{
	lua_pushstring(Lua_State, Executable_Path);
	return 1;
}

/* Open OnScreen Keyboard */
int LuaLibrary_OpenKeyboard(lua_State *L)
{
	if(!OnScreen_Keyboard)
	{
		Engine_requestKeyboard(0, 0, Game_Width, Game_Height / 20.0f * 9.0f);
	}
	return 0;
}

int LuaLibrary_CloseKeyboard(lua_State *L)
{
	if(OnScreen_Keyboard)
	{
		Engine_requestCloseKeyboard();
	}
	return 0;
}

/* Draw colored rectangled */
static int Lua_DrawRectangle2(lua_State *L)
{
	if(!App_UsingDisplay)
	{
		fprintf(LuaLibrary_Log, "renderer.rectangle : need to call have useDisplay to true to get access to rendering functions\n");
		Lua_requestClose = true;
	}
	if(!lua_isnumber(L, -9))
	{
		fprintf(LuaLibrary_Log, "renderer.rectangle : need number for argument 1\n");
		return 0;
	}
	if(!lua_isnumber(L, -8))
	{
		fprintf(LuaLibrary_Log, "renderer.rectangle : need number for argument 2\n");
		return 0;
	}
	if(!lua_isnumber(L, -7))
	{
		fprintf(LuaLibrary_Log, "renderer.rectangle : need number for argument 3\n");
		return 0;
	}
	if(!lua_isnumber(L, -6))
	{
		fprintf(LuaLibrary_Log, "renderer.rectangle : need number for argument 4\n");
		return 0;
	}
	if(!lua_isnumber(L, -5))
	{
		fprintf(LuaLibrary_Log, "renderer.rectangle : need number for argument 5\n");
		return 0;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(LuaLibrary_Log, "renderer.rectangle : need number for argument 6\n");
		return 0;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(LuaLibrary_Log, "renderer.rectangle : need number for argument 7\n");
		return 0;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(LuaLibrary_Log, "renderer.rectangle : need number for argument 8\n");
		return 0;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(LuaLibrary_Log, "renderer.rectangle : need number for argument 9\n");
		return 0;
	}

	double x = lua_tonumber(L, -9) + Lua_Window.x, y = lua_tonumber(L, -8) + Lua_Window.y;
	int z = lua_tonumber(L, -7);
	printf("%i\n", z);
	double width = lua_tonumber(L, -6), height = lua_tonumber(L, -5);

	if(x + width < Lua_Window.x || x > Lua_Window.z + Lua_Window.x || y + height < Lua_Window.y || y > Lua_Window.w + Lua_Window.y)
		return 0;

	if(x < Lua_Window.x)
	{
		double delta = Lua_Window.x - x;
		x = Lua_Window.x;
		width -= delta;
	} else if(x + width > Lua_Window.x + Lua_Window.z)
	{
		double delta = (x + width) - (Lua_Window.x + Lua_Window.z);
		width -= delta;
	}

	if(y < Lua_Window.y)
	{
		double delta = Lua_Window.y - y;
		y = Lua_Window.y;
		height -= delta;
	} else if(y + height > Lua_Window.y + Lua_Window.w)
	{
		double delta = (y + height) - (Lua_Window.y + Lua_Window.w);
		height -= delta;
	}

	struct Vector4f Color = { lua_tonumber(L, -4), lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1) };

	Default_Shader.pushQuad(Quad_Create(x, y, x, y + height, x + width, y + height, x + width, y), Color, z);
	return 0;
}

static int Lua_DrawRectangle(lua_State *L)
{
	if(!App_UsingDisplay)
	{
		fprintf(LuaLibrary_Log, "renderer.rectangle : need to call have useDisplay to true to get access to rendering functions\n");
		Lua_requestClose = true;
	}
	if(lua_isnumber(L, -9))
    {
        return Lua_DrawRectangle2(L);
    }
	if(!lua_isnumber(L, -8))
	{
		fprintf(LuaLibrary_Log, "renderer.rectangle : need number for argument 1\n");
		return 0;
	}
	if(!lua_isnumber(L, -7))
	{
		fprintf(LuaLibrary_Log, "renderer.rectangle : need number for argument 2\n");
		return 0;
	}
	if(!lua_isnumber(L, -6))
	{
		fprintf(LuaLibrary_Log, "renderer.rectangle : need number for argument 3\n");
		return 0;
	}
	if(!lua_isnumber(L, -5))
	{
		fprintf(LuaLibrary_Log, "renderer.rectangle : need number for argument 4\n");
		return 0;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(LuaLibrary_Log, "renderer.rectangle : need number for argument 5\n");
		return 0;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(LuaLibrary_Log, "renderer.rectangle : need number for argument 6\n");
		return 0;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(LuaLibrary_Log, "renderer.rectangle : need number for argument 7\n");
		return 0;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(LuaLibrary_Log, "renderer.rectangle : need number for argument 8\n");
		return 0;
	}

	double x = lua_tonumber(L, -8) + Lua_Window.x, y = lua_tonumber(L, -7) + Lua_Window.y;
	double width = lua_tonumber(L, -6), height = lua_tonumber(L, -5);

	if(x + width < Lua_Window.x || x > Lua_Window.z + Lua_Window.x || y + height < Lua_Window.y || y > Lua_Window.w + Lua_Window.y)
		return 0;

	if(x < Lua_Window.x)
	{
		double delta = Lua_Window.x - x;
		x = Lua_Window.x;
		width -= delta;
	} else if(x + width > Lua_Window.x + Lua_Window.z)
	{
		double delta = (x + width) - (Lua_Window.x + Lua_Window.z);
		width -= delta;
	}

	if(y < Lua_Window.y)
	{
		double delta = Lua_Window.y - y;
		y = Lua_Window.y;
		height -= delta;
	} else if(y + height > Lua_Window.y + Lua_Window.w)
	{
		double delta = (y + height) - (Lua_Window.y + Lua_Window.w);
		height -= delta;
	}

	struct Vector4f Color = { lua_tonumber(L, -4), lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1) };

	Default_Shader.pushQuad(Quad_Create(x, y, x, y + height, x + width, y + height, x + width, y), Color, 0);
	return 0;
}

/* Draw fixed text */
static int Lua_fixedFontRender(lua_State *L)
{
	if(!App_UsingDisplay)
	{
		fprintf(LuaLibrary_Log, "renderer.fixedText : need to call have useDisplay to true to get access to rendering functions\n");
		Lua_requestClose = true;
	}
	if(!lua_isstring(L, -9))
	{
		fprintf(LuaLibrary_Log, "renderer.fixedText : need string for argument 1\n");
		return 0;
	}
	if(!lua_isnumber(L, -8))
	{
		fprintf(LuaLibrary_Log, "renderer.fixedText : need number for argument 2\n");
		return 0;
	}
	if(!lua_isnumber(L, -7))
	{
		fprintf(LuaLibrary_Log, "renderer.fixedText : need number for argument 3\n");
		return 0;
	}
	if(!lua_isnumber(L, -6))
	{
		fprintf(LuaLibrary_Log, "renderer.fixedText : need number for argument 4\n");
		return 0;
	}
	if(!lua_isnumber(L, -5))
	{
		fprintf(LuaLibrary_Log, "renderer.fixedText : need number for argument 5\n");
		return 0;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(LuaLibrary_Log, "renderer.fixedText : need number for argument 6\n");
		return 0;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(LuaLibrary_Log, "renderer.fixedText : need number for argument 7\n");
		return 0;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(LuaLibrary_Log, "renderer.fixedText : need number for argument 8\n");
		return 0;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(LuaLibrary_Log, "renderer.fixedText : need number for argument 9\n");
		return 0;
	}

	double x = lua_tonumber(L, -8) + Lua_Window.x, y = lua_tonumber(L, -7) + Lua_Window.y;
	double width = lua_tonumber(L, -6), height = lua_tonumber(L, -5);

	if(x + width < Lua_Window.x || x > Lua_Window.z + Lua_Window.x || y + height < Lua_Window.y || y > Lua_Window.w + Lua_Window.y)
		return 0;

	if(x < Lua_Window.x)
	{
		double delta = Lua_Window.x - x;
		x = Lua_Window.x;
		width -= delta;
	} else if(x + width > Lua_Window.x + Lua_Window.z)
	{
		double delta = (x + width) - (Lua_Window.x + Lua_Window.z);
		width -= delta;
	}

	if(y < Lua_Window.y)
	{
		double delta = Lua_Window.y - y;
		y = Lua_Window.y;
		height -= delta;
	} else if(y + height > Lua_Window.y + Lua_Window.w)
	{
		double delta = (y + height) - (Lua_Window.y + Lua_Window.w);
		height -= delta;
	}

	struct Vector4f Color = { lua_tonumber(L, -4), lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1) };

	Font_FixedRender(DefaultFontManager, lua_tostring(L, -9), x, y, height, width, 1.0f, Color);

	return 0;
}

void LuaLibrary_Load(FILE *F)
{
    LuaLibrary_Log = F;

    static const luaL_Reg Engine_Functions[] = {
		{"getFPS", Lua_getFPS},
		{"getPath", Lua_getPath},
		{"openKeyboard", LuaLibrary_OpenKeyboard},
		{"closeKeyboard", LuaLibrary_CloseKeyboard},
		{"close", Lua_Close},
    	{NULL, NULL}
    };

    static const luaL_Reg Renderer_Functions[] = {
    		{"rectangle", Lua_DrawRectangle},
			{"fixedText", Lua_fixedFontRender},
			{NULL, NULL}
    };

    lua_newtable(Lua_State);
    luaL_setfuncs(Lua_State, Engine_Functions, 0);
    lua_setglobal(Lua_State, "engine");

    lua_newtable(Lua_State);
    luaL_setfuncs(Lua_State, Renderer_Functions, 0);
    lua_setglobal(Lua_State, "renderer");

    LuaLibrary_Gui_Load();
    LuaLibrary_Image_Load();
    LuaLibrary_Animation_Load();
    LuaLibrary_Particles_Load();
}

void LuaLibrary_Render()
{
    LuaLibrary_Particles_Render();
}

void LuaLibrary_Close()
{
    log_info("Closing Lua Library");

    LuaLibrary_Gui_Close();
    LuaLibrary_Image_Close();
    LuaLibrary_Animation_Close();
	LuaLibrary_Particles_Close();

	log_info("Done closing Lua Library");
}

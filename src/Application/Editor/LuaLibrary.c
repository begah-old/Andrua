/*
 * LuaLibrary.c
 *
 *  Created on: Sep 2, 2015
 *      Author: mathi
 */

#include "Editor.h"

/* File that act's as lua's stdout and stderr */
FILE *Log, *Input;

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

// Texture Management
double Texture_NextID = 0.0; // Keep track of available ID's
struct Lua_Texture { double ID; GLuint TextID; }; // Structure to match a texture to and ID
struct vector_t *Lua_Texture_List = NULL; // List of textures

// Loads an image and add it to the lua list
static int Lua_loadImage(lua_State *L)
{
	if(!App_UsingDisplay)
	{
		return 0;
	}
	if(!lua_isstring(L, -1))
	{
		fprintf(Log, "Error loading image, argument needs to be a string\n");
		Lua_Close(NULL);
		lua_pushnumber(L, -1.0);
		return 1;
	}

	// If list isn't created, create it
	if(!Lua_Texture_List) Lua_Texture_List = vector_new(sizeof(struct Lua_Texture));

	GLuint Image = Image_LoadExternal(lua_tostring(L, -1));
	struct Lua_Texture tt = { Texture_NextID++, Image };
	vector_push_back( Lua_Texture_List,  &tt);

	// Give ID back
	lua_pushnumber(L, tt.ID);

	return 1;
}

// Draw image with a specific ID
static int Lua_drawImage(lua_State *L)
{
	if(!Lua_Texture_List) return 0;

	if(!lua_isnumber(L, -5))
	{
		fprintf(Log, "image.rectangle : need number for argument 1\n");
		return 0;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(Log, "image.rectangle : need number for argument 2\n");
		return 0;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(Log, "image.rectangle : need number for argument 3\n");
		return 0;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(Log, "image.rectangle : need number for argument 4\n");
		return 0;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(Log, "image.rectangle : need number for argument 5\n");
		return 0;
	}

	double TextureID = lua_tonumber(L, -1);
	GLuint Texture = 0;

	// Check if TextureID is in range
	if(TextureID < 0.0 || TextureID > Texture_NextID)
	{
		fprintf(Log, "image.rectangle : invalid texture id\n");
		return 0;
	}

	struct Lua_Texture *LT = Lua_Texture_List->items;

	// Search for corresponding texture
	for(int i = 0; i < Lua_Texture_List->size; i++)
	{
		if(LT[i].ID == TextureID)
		{
			Texture = LT[i].TextID;
			break;
		}
	}

	// Make sure image is in Lua's given view
	double x = lua_tonumber(L, -5) + Lua_Window.x, y = lua_tonumber(L, -4) + Lua_Window.y;
	double width = lua_tonumber(L, -3), height = lua_tonumber(L, -2);

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

	Image_Shader.pushQuad(Quad_Create(x, y, x, y + height, x + width, y + height, x + width, y), Quad_Create(0, 0, 0, 1, 1, 1, 1, 0), Texture, Vector4_Create(0.0, 0.0, 0.0, 0.0));

	return 0;
}

static int Lua_freeImage(lua_State *L)
{
	if(!Lua_Texture_List) return 0;

	if(!lua_isnumber(L, -1))
	{
		fprintf(Log, "Error freeing image, argument needs to be a number\n");
		Lua_Close(NULL);
		return 0;
	}

	double TextureID = lua_tonumber(L, -1);

	// Check if TextureID is in range
	if(TextureID < 0.0 || TextureID > Texture_NextID)
	{
		fprintf(Log, "image.free : invalid texture id\n");
		return 0;
	}

	struct Lua_Texture *LT = Lua_Texture_List->items;

	for(int i = 0; i < Lua_Texture_List->size; i++)
	{
		if(LT[i].ID == TextureID)
		{
			Image_Free(LT[i].TextID);
			vector_erase(Lua_Texture_List, i);
			return 0;
		}
	}

	return 0;
}

// Gui Button Management
double Gui_Button_NextID = 0.0;
struct Lua_Button { double ID; struct Gui_Button *B; };
struct vector_t *Lua_Button_List = NULL;

static int Lua_newButton(lua_State *L)
{
	if(!App_UsingDisplay)
	{
		return 0;
	}
	if(!lua_isstring(L, -5))
	{
		fprintf(Log, "Error creating button, argument 1 needs to be a string\n");
		Lua_Close(NULL);
		lua_pushnumber(L, -1.0);
		return 1;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(Log, "Error creating button, argument 2 needs to be a number\n");
		Lua_Close(NULL);
		lua_pushnumber(L, -1.0);
		return 1;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(Log, "Error creating button, argument 3 needs to be a number\n");
		Lua_Close(NULL);
		lua_pushnumber(L, -1.0);
		return 1;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(Log, "Error creating button, argument 4 needs to be a number\n");
		Lua_Close(NULL);
		lua_pushnumber(L, -1.0);
		return 1;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(Log, "Error creating button, argument 5 needs to be a number\n");
		Lua_Close(NULL);
		lua_pushnumber(L, -1.0);
		return 1;
	}

	if(!Lua_Button_List)
	{
		Lua_Button_List = vector_new(sizeof(struct Gui_Button));
		Gui_Button_NextID = 0.0;
	}

	double x = lua_tonumber(L, -4), y = lua_tonumber(L, -3), width = lua_tonumber(L, -2), height = lua_tonumber(L, -1);
	const char *str = lua_tostring(L, -5);

	struct Lua_Button lg = { Gui_Button_NextID++, NULL };

	lg.B = Gui_Button_Create(x + Lua_Window.x, y + Lua_Window.y, width, height, str, 0, 0, width, height, 0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
	lg.B->CenterText = true;
	vector_push_back(Lua_Button_List, &lg);

	lua_pushnumber(Lua_State, lg.ID);
	return 1;
}

static int Lua_colorButton(lua_State *L)
{
	if(!Lua_Button_List)
	{
		return 0;
	}

	if(!lua_isnumber(L, -9))
	{
		fprintf(Log, "Error coloring button, argument 1 needs to be a number");
		Lua_Close(NULL);
		return 0;
	}
	if(!lua_isnumber(L, -8))
	{
		fprintf(Log, "Error coloring button, argument 2 needs to be a number");
		Lua_Close(NULL);
		return 0;
	}
	if(!lua_isnumber(L, -7))
	{
		fprintf(Log, "Error coloring button, argument 3 needs to be a number");
		Lua_Close(NULL);
		return 0;
	}
	if(!lua_isnumber(L, -6))
	{
		fprintf(Log, "Error coloring button, argument 4 needs to be a number");
		Lua_Close(NULL);
		return 0;
	}
	if(!lua_isnumber(L, -5))
	{
		fprintf(Log, "Error coloring button, argument 5 needs to be a number");
		Lua_Close(NULL);
		return 0;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(Log, "Error coloring button, argument 6 needs to be a number");
		Lua_Close(NULL);
		return 0;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(Log, "Error coloring button, argument 7 needs to be a number");
		Lua_Close(NULL);
		return 0;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(Log, "Error coloring button, argument 8 needs to be a number");
		Lua_Close(NULL);
		return 0;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(Log, "Error coloring button, argument 9 needs to be a number");
		Lua_Close(NULL);
		return 0;
	}

	double ID = lua_tonumber(L, -9);

	if(ID < 0 || ID >= Gui_Button_NextID)
	{
		fprintf(Log, "Error coloring button, argument 1 needs to be a valid button");
		Lua_Close(NULL);
		return 0;
	}

	struct Lua_Button *lb = Lua_Button_List->items;
	struct Gui_Button *bu = NULL;
	for(int i = 0; i < Lua_Button_List->size; i++)
		if(lb[i].ID == ID) {
			bu = lb[i].B;
			break;
		}
	if(!bu)
	{
		fprintf(Log, "Error coloring button, didn't find button");
		Lua_Close(NULL);
		return 0;
	}

	double r = lua_tonumber(L, -8), g = lua_tonumber(L, -7), b = lua_tonumber(L, -6), a = lua_tonumber(L, -5), rr = lua_tonumber(L, -4), gg = lua_tonumber(L, -3), bb = lua_tonumber(L, -2), aa = lua_tonumber(L, -1);
	bu->Color = Vector4_Create(r, g, b, a);
	bu->HoverColor = Vector4_Create(rr, gg, bb, aa);

	return 0;
}

static int Lua_colorButtonText(lua_State *L)
{
	if(!App_UsingDisplay)
	{
		return 0;
	}
	if(!Lua_Button_List)
	{
		return 0;
	}

	if(!lua_isnumber(L, -5))
	{
		fprintf(Log, "Error creating button, argument 1 needs to be a number\n");
		Lua_Close(NULL);
		return 0;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(Log, "Error creating button, argument 2 needs to be a number\n");
		Lua_Close(NULL);
		return 0;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(Log, "Error creating button, argument 3 needs to be a number\n");
		Lua_Close(NULL);
		return 0;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(Log, "Error creating button, argument 4 needs to be a number\n");
		Lua_Close(NULL);
		return 0;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(Log, "Error creating button, argument 5 needs to be a number\n");
		Lua_Close(NULL);
		return 0;
	}

	if(!Lua_Button_List)
	{
		Lua_Button_List = vector_new(sizeof(struct Gui_Button));
		Gui_Button_NextID = 0.0;
	}

	double ID = lua_tonumber(L, -5);

	if(ID < 0 || ID >= Gui_Button_NextID)
	{
		fprintf(Log, "Error coloring button, argument 1 needs to be a valid button");
		Lua_Close(NULL);
		return 0;
	}

	struct Lua_Button *lb = Lua_Button_List->items;
	struct Gui_Button *bu = NULL;
	for(int i = 0; i < Lua_Button_List->size; i++)
		if(lb[i].ID == ID) {
			bu = lb[i].B;
			break;
		}
	if(!bu)
	{
		fprintf(Log, "Error coloring button, didn't find button");
		Lua_Close(NULL);
		return 0;
	}

	bu->Font_Color = Vector4_Create(lua_tonumber(L, -4), lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1));
	return 0;
}

static int Lua_renderButton(lua_State *L)
{
	if(!Lua_Button_List)
	{
		lua_pushboolean(Lua_State, false);
		return 1;
	}

	if(!lua_isnumber(L, -1))
	{
		fprintf(Log, "Error rendering button, argument needs to be a number");
		Lua_Close(NULL);
		lua_pushboolean(Lua_State, false);
		return 1;
	}

	double ID = lua_tonumber(L, -1);

	if(ID < 0 || ID >= Gui_Button_NextID)
	{
		fprintf(Log, "Error rendering button, argument needs to be a valid button");
		Lua_Close(NULL);
		lua_pushboolean(Lua_State, false);
		return 1;
	}

	struct Lua_Button *lb = Lua_Button_List->items;
	struct Gui_Button *b = NULL;
	for(int i = 0; i < Lua_Button_List->size; i++)
		if(lb[i].ID == ID) {
			b = lb[i].B;
			break;
		}
	if(!b)
	{
		fprintf(Log, "Error rendering button, didn't find button");
		Lua_Close(NULL);
		lua_pushboolean(Lua_State, false);
		return 1;
	}

	lua_pushboolean(Lua_State, Gui_Button_Render(b));
	return 1;
}

static int Lua_freeButton(lua_State *L)
{
	if(!Lua_Texture_List) return 0;

	if(!lua_isnumber(L, -1))
	{
		fprintf(Log, "Error freeing button, argument needs to be a number\n");
		Lua_Close(NULL);
		return 0;
	}

	double ID = lua_tonumber(L, -1);

	struct Lua_Button *lb = Lua_Button_List->items;

	for(int i = 0; i < Lua_Button_List->size; i++)
	{
		if(lb[i].ID == ID)
		{
			Gui_Button_Free(&lb[i].B);
			vector_erase(Lua_Button_List, i);
			return 0;
		}
	}

	return 0;
}

/* Draw colored rectangled */
static int Lua_DrawRectangle(lua_State *L)
{
	if(!App_UsingDisplay)
	{
		fprintf(Log, "renderer.rectangle : need to call have useDisplay to true to get access to rendering functions\n");
		Lua_Close(NULL);
	}
	if(!lua_isnumber(L, -8))
	{
		fprintf(Log, "renderer.rectangle : need number for argument 1\n");
		return 0;
	}
	if(!lua_isnumber(L, -7))
	{
		fprintf(Log, "renderer.rectangle : need number for argument 2\n");
		return 0;
	}
	if(!lua_isnumber(L, -6))
	{
		fprintf(Log, "renderer.rectangle : need number for argument 3\n");
		return 0;
	}
	if(!lua_isnumber(L, -5))
	{
		fprintf(Log, "renderer.rectangle : need number for argument 4\n");
		return 0;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(Log, "renderer.rectangle : need number for argument 5\n");
		return 0;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(Log, "renderer.rectangle : need number for argument 6\n");
		return 0;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(Log, "renderer.rectangle : need number for argument 7\n");
		return 0;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(Log, "renderer.rectangle : need number for argument 8\n");
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

	Default_Shader.pushQuad(Quad_Create(x, y, x, y + height, x + width, y + height, x + width, y), Color);
	return 0;
}

/* Draw fixed text */
static int Lua_fixedFontRender(lua_State *L)
{
	if(!App_UsingDisplay)
	{
		fprintf(Log, "renderer.rectangle : need to call have useDisplay to true to get access to rendering functions\n");
		Lua_Close(NULL);
	}
	if(!lua_isstring(L, -9))
	{
		fprintf(Log, "renderer.rectangle : need string for argument 1\n");
		return 0;
	}
	if(!lua_isnumber(L, -8))
	{
		fprintf(Log, "renderer.rectangle : need number for argument 2\n");
		return 0;
	}
	if(!lua_isnumber(L, -7))
	{
		fprintf(Log, "renderer.rectangle : need number for argument 3\n");
		return 0;
	}
	if(!lua_isnumber(L, -6))
	{
		fprintf(Log, "renderer.rectangle : need number for argument 4\n");
		return 0;
	}
	if(!lua_isnumber(L, -5))
	{
		fprintf(Log, "renderer.rectangle : need number for argument 5\n");
		return 0;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(Log, "renderer.rectangle : need number for argument 6\n");
		return 0;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(Log, "renderer.rectangle : need number for argument 7\n");
		return 0;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(Log, "renderer.rectangle : need number for argument 8\n");
		return 0;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(Log, "renderer.rectangle : need number for argument 9\n");
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

void Lua_LoadLibrary(FILE *F)
{
    static const luaL_Reg Engine_Functions[] = {
		{"getFPS", Lua_getFPS},
		{"getPath", Lua_getPath},
		{"close", Lua_Close},
    	{NULL, NULL}
    };

    static const luaL_Reg Renderer_Functions[] = {
    		{"rectangle", Lua_DrawRectangle},
			{"fixedText", Lua_fixedFontRender},
			{NULL, NULL}
    };

    static const luaL_Reg Image_Functions[] = {
    		{"load", Lua_loadImage},
			{"drawRectangle", Lua_drawImage},
			{"free", Lua_freeImage},
			{NULL, NULL}
    };

    static const luaL_Reg Button_Functions[] = {
    		{"new", Lua_newButton},
			{"color", Lua_colorButton},
			{"colorText", Lua_colorButtonText},
			{"render", Lua_renderButton},
			{"free", Lua_freeButton},
			{NULL, NULL}
    };

    lua_newtable(Lua_State);
    luaL_setfuncs(Lua_State, Engine_Functions, 0);
    lua_setglobal(Lua_State, "engine");

    lua_newtable(Lua_State);
    luaL_setfuncs(Lua_State, Renderer_Functions, 0);
    lua_setglobal(Lua_State, "renderer");

    lua_newtable(Lua_State);
    luaL_setfuncs(Lua_State, Image_Functions, 0);
    lua_setglobal(Lua_State, "image");

    lua_newtable(Lua_State);
    luaL_setfuncs(Lua_State, Button_Functions, 0);
    lua_setglobal(Lua_State, "button");
}

void Lua_closeLibrary()
{
	if (Lua_Texture_List) {
		struct Lua_Texture *LT = Lua_Texture_List->items;

		for (int i = 0; i < Lua_Texture_List->size; i++)
			Image_Free(LT[i].TextID);

		vector_delete(Lua_Texture_List);
		Lua_Texture_List = NULL;
		Texture_NextID = 0.0;
	}
	if (Lua_Button_List) {
		struct Lua_Button *lb = Lua_Button_List->items;

		for (int i = 0; i < Lua_Button_List->size; i++)
			Gui_Button_Free(&lb[i].B);

		vector_delete(Lua_Button_List);
		Lua_Button_List = NULL;
		Gui_Button_NextID = 0.0;
	}
}
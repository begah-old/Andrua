#include "LuaLibrary.h"

// Texture Management
double Texture_NextID = 0.0; // Keep track of available ID's
struct Lua_Texture { double ID; struct Image *TextID; }; // Structure to match a texture to and ID
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
		fprintf(LuaLibrary_Log, "Error image.load, argument needs to be a string\n");
		Lua_requestClose = true;
		lua_pushnumber(L, -1.0);
		return 1;
	}

	// If list isn't created, create it
	if(!Lua_Texture_List) Lua_Texture_List = vector_new(sizeof(struct Lua_Texture));

	struct Image *Image = Image_LoadExternal(lua_tostring(L, -1));
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
		fprintf(LuaLibrary_Log, "image.rectangle : need number for argument 1\n");
		return 0;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(LuaLibrary_Log, "image.rectangle : need number for argument 2\n");
		return 0;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(LuaLibrary_Log, "image.rectangle : need number for argument 3\n");
		return 0;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(LuaLibrary_Log, "image.rectangle : need number for argument 4\n");
		return 0;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(LuaLibrary_Log, "image.rectangle : need number for argument 5\n");
		return 0;
	}

	double TextureID = lua_tonumber(L, -1);
	struct Image *Texture = NULL;

	// Check if TextureID is in range
	if(TextureID < 0.0 || TextureID > Texture_NextID)
	{
		fprintf(LuaLibrary_Log, "image.rectangle : invalid texture id\n");
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

	Image_Shader.pushQuad(Quad_Create(x, y, x, y + height, x + width, y + height, x + width, y), Quad_Create(Texture->x, Texture->y, Texture->x, Texture->y2, Texture->x2, Texture->y2, Texture->x2, Texture->y), Texture->Image,
                       Vector4_Create(0.0, 0.0, 0.0, 0.0), 0);

	return 0;
}

static int Lua_freeImage(lua_State *L)
{
	if(!Lua_Texture_List) return 0;

	if(!lua_isnumber(L, -1))
	{
		fprintf(LuaLibrary_Log, "Error image.free, argument needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}

	double TextureID = lua_tonumber(L, -1);

	// Check if TextureID is in range
	if(TextureID < 0.0 || TextureID > Texture_NextID)
	{
		fprintf(LuaLibrary_Log, "image.free : invalid texture id\n");
		return 0;
	}

	struct Lua_Texture *LT = Lua_Texture_List->items;

	for(int i = 0; i < Lua_Texture_List->size; i++)
	{
		if(LT[i].ID == TextureID)
		{
			Image_FreeSimple(LT[i].TextID);
			vector_erase(Lua_Texture_List, i);
			return 0;
		}
	}

	return 0;
}

void LuaLibrary_Image_Load()
{
    static const luaL_Reg Image_Functions[] = {
    		{"load", Lua_loadImage},
			{"drawRectangle", Lua_drawImage},
			{"free", Lua_freeImage},
			{NULL, NULL}
    };

    lua_newtable(Lua_State);
    luaL_setfuncs(Lua_State, Image_Functions, 0);
    lua_setglobal(Lua_State, "image");
}

void LuaLibrary_Image_Render()
{

}

void LuaLibrary_Image_Close()
{
    if (Lua_Texture_List) {
		struct Lua_Texture *LT = Lua_Texture_List->items;

		for (int i = 0; i < Lua_Texture_List->size; i++)
			Image_Free(LT[i].TextID);

		vector_delete(Lua_Texture_List);
		Lua_Texture_List = NULL;
		Texture_NextID = 0.0;
	}
	log_info("Free'd all textures");
}

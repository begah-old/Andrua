#include "LuaLibrary.h"

// Animation Management
double Animation_NextID = 0.0;
struct Lua_Animation { double ID; struct Animation *Animation; };
struct vector_t *Lua_Animation_List = NULL;

static int Lua_loadAnimation(lua_State *State)
{
    if(!Lua_Animation_List)
    {
        Lua_Animation_List = vector_new(sizeof(struct Lua_Animation));
    }

    if(!lua_isstring(Lua_State, -1))
    {
        fprintf(LuaLibrary_Log, "animation.load : Argument need to be a string");
        Lua_requestClose = true;
		lua_pushnumber(Lua_State, -1.0);
		return 1;
    }

    const char *Name = lua_tostring(Lua_State, -1);

    struct Lua_Animation Anim = {Animation_NextID++, Animation_LoadExternal(Name)};
    Anim.Animation->Time_perFrame = 500;
    Anim.Animation->x = Lua_Window.x;
    Anim.Animation->y = Lua_Window.y;

    vector_push_back(Lua_Animation_List, &Anim);

    lua_pushnumber(Lua_State, Anim.ID);

    return 1;
}

static int Lua_setSizeAnimation(lua_State *State)
{
    if(!Lua_Animation_List)
    {
        fprintf(LuaLibrary_Log, "animation.setSize : No animation created");
        Lua_requestClose = true;
        return 0;
    }

    if(!lua_isnumber(Lua_State, -3))
    {
        fprintf(LuaLibrary_Log, "animation.setSize : Argument 1 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    } if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(LuaLibrary_Log, "animation.setSize : Argument 2 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    } if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(LuaLibrary_Log, "animation.setSize : Argument 3 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    }

    double ID = lua_tonumber(Lua_State, -3);

    if(ID < 0 || ID >= Animation_NextID)
    {
        fprintf(LuaLibrary_Log, "animation.setSize : Argument 1 is a invalid animation ID");
        Lua_requestClose = true;
        return 0;
    }

    struct Lua_Animation *LA = Lua_Animation_List->items;
    struct Animation *Animation = NULL;

	// Search for corresponding texture
	for(int i = 0; i < Lua_Animation_List->size; i++)
	{
		if(LA[i].ID == ID)
		{
			Animation = LA[i].Animation;
			break;
		}
	}

	if(!Animation)
    {
        fprintf(LuaLibrary_Log, "animation.setSize : No animation matching that ID");
        Lua_requestClose = true;
        return 0;
    }

    double w = lua_tonumber(Lua_State, -2), h = lua_tonumber(Lua_State, -1);
    Animation_SetSize(Animation, w, h);
    return 0;
}

static int Lua_setPosAnimation(lua_State *State)
{
    if(!Lua_Animation_List)
    {
        fprintf(LuaLibrary_Log, "animation.setPos : No animation created");
        Lua_requestClose = true;
        return 0;
    }

    if(!lua_isnumber(Lua_State, -3))
    {
        fprintf(LuaLibrary_Log, "animation.setPos : Argument 1 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    } if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(LuaLibrary_Log, "animation.setPos : Argument 2 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    } if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(LuaLibrary_Log, "animation.setPos : Argument 3 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    }

    double ID = lua_tonumber(Lua_State, -3);

    if(ID < 0 || ID >= Animation_NextID)
    {
        fprintf(LuaLibrary_Log, "animation.setPos : Argument 1 is a invalid animation ID");
        Lua_requestClose = true;
        return 0;
    }

    struct Lua_Animation *LA = Lua_Animation_List->items;
    struct Animation *Animation = NULL;

	// Search for corresponding texture
	for(int i = 0; i < Lua_Animation_List->size; i++)
	{
		if(LA[i].ID == ID)
		{
			Animation = LA[i].Animation;
			break;
		}
	}

	if(!Animation)
    {
        fprintf(LuaLibrary_Log, "animation.setPos : No animation matching that ID");
        Lua_requestClose = true;
        return 0;
    }

    double x = lua_tonumber(Lua_State, -2) + Lua_Window.x, y = lua_tonumber(Lua_State, -1) + Lua_Window.y;
    Animation_SetPosition(Animation, x, y);
    return 0;
}

static int Lua_setDepthAnimation(lua_State *State)
{
    if(!Lua_Animation_List)
    {
        fprintf(LuaLibrary_Log, "animation.setDepth : No animation created");
        Lua_requestClose = true;
        return 0;
    }

    if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(LuaLibrary_Log, "animation.setDepth : Argument 1 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    } if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(LuaLibrary_Log, "animation.setDepth : Argument 2 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    }

    double ID = lua_tonumber(Lua_State, -2);

    if(ID < 0 || ID >= Animation_NextID)
    {
        fprintf(LuaLibrary_Log, "animation.setDepth : Argument 1 is a invalid animation ID");
        Lua_requestClose = true;
        return 0;
    }

    struct Lua_Animation *LA = Lua_Animation_List->items;
    struct Animation *Animation = NULL;

	// Search for corresponding texture
	for(int i = 0; i < Lua_Animation_List->size; i++)
	{
		if(LA[i].ID == ID)
		{
			Animation = LA[i].Animation;
			break;
		}
	}

	if(!Animation)
    {
        fprintf(LuaLibrary_Log, "animation.setDepth : No animation matching that ID");
        Lua_requestClose = true;
        return 0;
    }

    Animation->z = lua_tonumber(Lua_State, -1);
    return 0;
}

static int Lua_setAngleAnimation(lua_State *State)
{
    if(!Lua_Animation_List)
    {
        fprintf(LuaLibrary_Log, "animation.setAngle : No animation created");
        Lua_requestClose = true;
        return 0;
    }

    if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(LuaLibrary_Log, "animation.setAngle : Argument 1 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    } if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(LuaLibrary_Log, "animation.setAngle : Argument 2 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    }

    double ID = lua_tonumber(Lua_State, -2);

    if(ID < 0 || ID >= Animation_NextID)
    {
        fprintf(LuaLibrary_Log, "animation.setAngle : Argument 1 is a invalid animation ID");
        Lua_requestClose = true;
        return 0;
    }

    struct Lua_Animation *LA = Lua_Animation_List->items;
    struct Animation *Animation = NULL;

	// Search for corresponding texture
	for(int i = 0; i < Lua_Animation_List->size; i++)
	{
		if(LA[i].ID == ID)
		{
			Animation = LA[i].Animation;
			break;
		}
	}

	if(!Animation)
    {
        fprintf(LuaLibrary_Log, "animation.setAngle : No animation matching that ID");
        Lua_requestClose = true;
        return 0;
    }

    double Angle = lua_tonumber(Lua_State, -1);
    Animation_SetAngle(Animation, Angle);
    return 0;
}

static int Lua_setFrameTimeAnimation(lua_State *State)
{
    if(!Lua_Animation_List)
    {
        fprintf(LuaLibrary_Log, "animation.timePerFrame : No animation created");
        Lua_requestClose = true;
        return 0;
    }

    if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(LuaLibrary_Log, "animation.timePerFrame : Argument 1 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    } if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(LuaLibrary_Log, "animation.timePerFrame : Argument 2 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    }

    double ID = lua_tonumber(Lua_State, -2);

    if(ID < 0 || ID >= Animation_NextID)
    {
        fprintf(LuaLibrary_Log, "animation.timePerFrame : Argument 1 is a invalid animation ID");
        Lua_requestClose = true;
        return 0;
    }

    struct Lua_Animation *LA = Lua_Animation_List->items;
    struct Animation *Animation = NULL;

	// Search for corresponding texture
	for(int i = 0; i < Lua_Animation_List->size; i++)
	{
		if(LA[i].ID == ID)
		{
			Animation = LA[i].Animation;
			break;
		}
	}

	if(!Animation)
    {
        fprintf(LuaLibrary_Log, "animation.timePerFrame : No animation matching that ID");
        Lua_requestClose = true;
        return 0;
    }

    double Time = lua_tonumber(Lua_State, -1);
    Animation->Time_perFrame = (long)Time;
    return 0;
}

static int Lua_setReverseOnFinish(lua_State *State)
{
    if(!Lua_Animation_List)
    {
        fprintf(LuaLibrary_Log, "animation.reverseOnFinish : No animation created");
        Lua_requestClose = true;
        return 0;
    }

    if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(LuaLibrary_Log, "animation.reverseOnFinish : Argument 1 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    }

    double ID = lua_tonumber(Lua_State, -1);

    if(ID < 0 || ID >= Animation_NextID)
    {
        fprintf(LuaLibrary_Log, "animation.reverseOnFinish : Argument 1 is a invalid animation ID");
        Lua_requestClose = true;
        return 0;
    }

    struct Lua_Animation *LA = Lua_Animation_List->items;
    struct Animation *Animation = NULL;

	// Search for corresponding texture
	for(int i = 0; i < Lua_Animation_List->size; i++)
	{
		if(LA[i].ID == ID)
		{
			Animation = LA[i].Animation;
			break;
		}
	}

	if(!Animation)
    {
        fprintf(LuaLibrary_Log, "animation.reverseOnFinish : No animation matching that ID");
        Lua_requestClose = true;
        return 0;
    }

    Animation_toggleReverseOnFinish(Animation);

    return 0;
}

static int Lua_renderAnimation(lua_State *State)
{
        if(!Lua_Animation_List)
    {
        fprintf(LuaLibrary_Log, "animation.render : No animation created");
        Lua_requestClose = true;
        return 0;
    }

    if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(LuaLibrary_Log, "animation.render : Argument 1 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    }

    double ID = lua_tonumber(Lua_State, -1);

    if(ID < 0 || ID >= Animation_NextID)
    {
        fprintf(LuaLibrary_Log, "animation.render : Argument 1 is a invalid animation ID");
        Lua_requestClose = true;
        return 0;
    }

    struct Lua_Animation *LA = Lua_Animation_List->items;
    struct Animation *Animation = NULL;

	// Search for corresponding texture
	for(int i = 0; i < Lua_Animation_List->size; i++)
	{
		if(LA[i].ID == ID)
		{
			Animation = LA[i].Animation;
			break;
		}
	}

	if(!Animation)
    {
        fprintf(LuaLibrary_Log, "animation.render : No animation matching that ID");
        Lua_requestClose = true;
        return 0;
    }

    Animation_Render(Animation);
    return 0;
}

static int Lua_freeAnimation(lua_State *State)
{
    if(!Lua_Animation_List)
    {
        fprintf(LuaLibrary_Log, "animation.free : No animation created");
        Lua_requestClose = true;
        return 0;
    }

    if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(LuaLibrary_Log, "animation.free : Argument 1 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    }

    double ID = lua_tonumber(Lua_State, -1);

    if(ID < 0 || ID >= Animation_NextID)
    {
        fprintf(LuaLibrary_Log, "animation.free : Argument 1 is a invalid animation ID");
        Lua_requestClose = true;
        return 0;
    }

    struct Lua_Animation *LA = Lua_Animation_List->items;
    int Index = -1;

	// Search for corresponding texture
	for(int i = 0; i < Lua_Animation_List->size; i++)
	{
		if(LA[i].ID == ID)
		{
			Index = i;
			break;
		}
	}

	if(Index == -1)
    {
        fprintf(LuaLibrary_Log, "animation.free : No animation matching that ID");
        Lua_requestClose = true;
        return 0;
    }

    Animation_Free(LA[Index].Animation);
    vector_erase(Lua_Animation_List, Index);

    if(!Lua_Animation_List->size)
    {
        Animation_NextID = 0.0;
        vector_delete(Lua_Animation_List);
        Lua_Animation_List = NULL;
    }
    return 0;
}

void LuaLibrary_Animation_Load()
{
    static const luaL_Reg Animation_Functions[] = {
        {"load", Lua_loadAnimation},
        {"setPos", Lua_setPosAnimation},
        {"setSize", Lua_setSizeAnimation},
        {"setAngle", Lua_setAngleAnimation},
        {"setDepth", Lua_setDepthAnimation},
        {"timePerFrame", Lua_setFrameTimeAnimation},
        {"reverseOnFinish", Lua_setReverseOnFinish},
        {"render", Lua_renderAnimation},
        {"free", Lua_freeAnimation},
        {NULL, NULL}
    };

    lua_newtable(Lua_State);
    luaL_setfuncs(Lua_State, Animation_Functions, 0);
    lua_setglobal(Lua_State, "animation");
}

void LuaLibrary_Animation_Render()
{

}

void LuaLibrary_Animation_Close()
{
    if (Lua_Animation_List) {
        struct Lua_Animation *la = Lua_Animation_List->items;

        for(int i = 0; i < Lua_Animation_List->size; i++)
            Animation_Free(la[i].Animation);

        vector_delete(Lua_Animation_List);
        Lua_Animation_List = NULL;
        Animation_NextID = 0.0;
	}
	log_info("Free'd all animations");
}

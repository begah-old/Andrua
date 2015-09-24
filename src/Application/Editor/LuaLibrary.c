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

/* Open OnScreen Keyboard */
static int Lua_OpenKeyboard(lua_State *L)
{
	if(!OnScreen_Keyboard)
	{
		Engine_requestKeyboard(0, 0, Game_Width, Game_Height / 20.0f * 9.0f);
	}
	return 0;
}

static int Lua_CloseKeyboard(lua_State *L)
{
	if(OnScreen_Keyboard)
	{
		Engine_requestCloseKeyboard();
	}
	return 0;
}

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
		fprintf(Log, "Error image.load, argument needs to be a string\n");
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
	struct Image *Texture = NULL;

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

	Image_Shader.pushQuad(Quad_Create(x, y, x, y + height, x + width, y + height, x + width, y), Quad_Create(Texture->x, Texture->y, Texture->x, Texture->y2, Texture->x2, Texture->y2, Texture->x2, Texture->y), Texture->Image, Vector4_Create(0.0, 0.0, 0.0, 0.0));

	return 0;
}

static int Lua_freeImage(lua_State *L)
{
	if(!Lua_Texture_List) return 0;

	if(!lua_isnumber(L, -1))
	{
		fprintf(Log, "Error image.free, argument needs to be a number\n");
		Lua_requestClose = true;
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
			Image_FreeSimple(LT[i].TextID);
			vector_erase(Lua_Texture_List, i);
			return 0;
		}
	}

	return 0;
}

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
        fprintf(Log, "animation.load : Argument need to be a string");
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
        fprintf(Log, "animation.setSize : No animation created");
        Lua_requestClose = true;
        return 0;
    }

    if(!lua_isnumber(Lua_State, -3))
    {
        fprintf(Log, "animation.setSize : Argument 1 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    } if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(Log, "animation.setSize : Argument 2 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    } if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(Log, "animation.setSize : Argument 3 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    }

    double ID = lua_tonumber(Lua_State, -3);

    if(ID < 0 || ID >= Animation_NextID)
    {
        fprintf(Log, "animation.setSize : Argument 1 is a invalid animation ID");
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
        fprintf(Log, "animation.setSize : No animation matching that ID");
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
        fprintf(Log, "animation.setPos : No animation created");
        Lua_requestClose = true;
        return 0;
    }

    if(!lua_isnumber(Lua_State, -3))
    {
        fprintf(Log, "animation.setPos : Argument 1 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    } if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(Log, "animation.setPos : Argument 2 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    } if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(Log, "animation.setPos : Argument 3 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    }

    double ID = lua_tonumber(Lua_State, -3);

    if(ID < 0 || ID >= Animation_NextID)
    {
        fprintf(Log, "animation.setPos : Argument 1 is a invalid animation ID");
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
        fprintf(Log, "animation.setPos : No animation matching that ID");
        Lua_requestClose = true;
        return 0;
    }

    double x = lua_tonumber(Lua_State, -2) + Lua_Window.x, y = lua_tonumber(Lua_State, -1) + Lua_Window.y;
    Animation_SetPosition(Animation, x, y);
    return 0;
}

static int Lua_setAngleAnimation(lua_State *State)
{
    if(!Lua_Animation_List)
    {
        fprintf(Log, "animation.setAngle : No animation created");
        Lua_requestClose = true;
        return 0;
    }

    if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(Log, "animation.setAngle : Argument 1 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    } if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(Log, "animation.setAngle : Argument 2 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    }

    double ID = lua_tonumber(Lua_State, -2);

    if(ID < 0 || ID >= Animation_NextID)
    {
        fprintf(Log, "animation.setAngle : Argument 1 is a invalid animation ID");
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
        fprintf(Log, "animation.setAngle : No animation matching that ID");
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
        fprintf(Log, "animation.timePerFrame : No animation created");
        Lua_requestClose = true;
        return 0;
    }

    if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(Log, "animation.timePerFrame : Argument 1 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    } if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(Log, "animation.timePerFrame : Argument 2 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    }

    double ID = lua_tonumber(Lua_State, -2);

    if(ID < 0 || ID >= Animation_NextID)
    {
        fprintf(Log, "animation.timePerFrame : Argument 1 is a invalid animation ID");
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
        fprintf(Log, "animation.timePerFrame : No animation matching that ID");
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
        fprintf(Log, "animation.reverseOnFinish : No animation created");
        Lua_requestClose = true;
        return 0;
    }

    if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(Log, "animation.reverseOnFinish : Argument 1 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    }

    double ID = lua_tonumber(Lua_State, -1);

    if(ID < 0 || ID >= Animation_NextID)
    {
        fprintf(Log, "animation.reverseOnFinish : Argument 1 is a invalid animation ID");
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
        fprintf(Log, "animation.reverseOnFinish : No animation matching that ID");
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
        fprintf(Log, "animation.render : No animation created");
        Lua_requestClose = true;
        return 0;
    }

    if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(Log, "animation.render : Argument 1 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    }

    double ID = lua_tonumber(Lua_State, -1);

    if(ID < 0 || ID >= Animation_NextID)
    {
        fprintf(Log, "animation.render : Argument 1 is a invalid animation ID");
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
        fprintf(Log, "animation.render : No animation matching that ID");
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
        fprintf(Log, "animation.free : No animation created");
        Lua_requestClose = true;
        return 0;
    }

    if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(Log, "animation.free : Argument 1 needs to be a number (Animation ID)");
        Lua_requestClose = true;
        return 0;
    }

    double ID = lua_tonumber(Lua_State, -1);

    if(ID < 0 || ID >= Animation_NextID)
    {
        fprintf(Log, "animation.free : Argument 1 is a invalid animation ID");
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
        fprintf(Log, "animation.free : No animation matching that ID");
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

// Particle Systems Management
struct Lua_ParcticleS { int ID; _Bool DeleteOnFinish; };
struct Particle_System *Lua_ParticleSystem = NULL;
struct vector_t *Lua_ParcticleS_List = NULL;

static int Lua_particleSnew(lua_State *L)
{
    if(!lua_isnumber(Lua_State, -3))
    {
        fprintf(Log, "particles.new : No number for argument 1");
        Lua_requestClose = true;
        lua_pushnumber(Lua_State, -1);
        return 1;
    } else if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(Log, "particles.new : No number for argument 2");
        Lua_requestClose = true;
        lua_pushnumber(Lua_State, -1);
        return 1;
    } else if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(Log, "particles.new : No number for argument 3");
        Lua_requestClose = true;
        lua_pushnumber(Lua_State, -1);
        return 1;
    }

    if(!Lua_ParcticleS_List)
    {
        Lua_ParcticleS_List = vector_new(sizeof(struct Lua_ParcticleS));
        Lua_ParticleSystem = Particle_System_New();
    }

    int numParticle = lua_tonumber(Lua_State, -3);
    int x = lua_tonumber(Lua_State, -2), y = lua_tonumber(Lua_State, -1);

    struct Lua_ParcticleS PP = { Particle_Emitter_New(Lua_ParticleSystem, x, y, numParticle), false};
    vector_push_back(Lua_ParcticleS_List, &PP);

    lua_pushnumber(Lua_State, PP.ID);
    return 1;
}

static int Lua_particleToggleEmitting(lua_State *L)
{
    if(!Lua_ParcticleS_List)
        return 0;

    if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(Log, "particle.toggleEmitting needs ID as argument");
        Lua_requestClose = true;
        return 0;
    }

    int ID = lua_tonumber(Lua_State, -1);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(Log, "particle.toggleEmitting : argument 1 is an invalid ID");
        Lua_requestClose = true;
        return 0;
    }

    int IDX = -1;
    struct Particle_Emitter *PS = Lua_ParticleSystem->Emitters;;;
    for(int i = 0; i < Lua_ParticleSystem->Emitters_Count; i++)
    {
        if(PS[i].ID == ID)
        {
            IDX = i;
            break;
        }
    }

    if(IDX == -1)
    {
        fprintf(Log, "particle.toggleEmitting : cannot find particle emitter with that id");
        Lua_requestClose = true;
        return 0;
    }

    Lua_ParticleSystem->Emitters[IDX].Continue_Emitting = !Lua_ParticleSystem->Emitters[IDX].Continue_Emitting;
    return 0;
}

static int Lua_particleEmit(lua_State *L)
{
    if(!Lua_ParcticleS_List)
        return 0;

    if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(Log, "particle.emit needs ID as argument");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(Log, "particle.emit needs number as argument 2");
        Lua_requestClose = true;
        return 0;
    }

    int ID = lua_tonumber(Lua_State, -2);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(Log, "particle.emit : argument 1 is an invalid ID");
        Lua_requestClose = true;
        return 0;
    }

    int IDX = -1;
    struct Particle_Emitter *PS = Lua_ParticleSystem->Emitters;
    for(int i = 0; i < Lua_ParticleSystem->Emitters_Count; i++)
    {
        if(PS[i].ID == ID)
        {
            IDX = i;
            break;
        }
    }

    if(IDX == -1)
    {
        fprintf(Log, "particle.emit : cannot find particle emitter with that id");
        Lua_requestClose = true;
        return 0;
    }

    Particle_EmitParticles(Lua_ParticleSystem->Emitters + IDX, lua_tonumber(Lua_State, -1));
    return 0;
}

static int Lua_particleSetDeleteOnFinish(lua_State *L)
{
    if(!Lua_ParcticleS_List)
        return 0;

    if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(Log, "particle.deleteOnFinish needs ID as argument");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isboolean(Lua_State, -1))
    {
        fprintf(Log, "particle.emit needs boolean as argument 2");
        Lua_requestClose = true;
        return 0;
    }

    int ID = lua_tonumber(Lua_State, -2);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(Log, "particle.emit : argument 1 is an invalid ID");
        Lua_requestClose = true;
        return 0;
    }

    int IDX = -1;
    struct Lua_ParcticleS *PS = Lua_ParcticleS_List->items;
    for(int i = 0; i < Lua_ParcticleS_List->size; i++)
    {
        if(PS[i].ID == ID)
        {
            IDX = i;
            break;
        }
    }

    if(IDX == -1)
    {
        fprintf(Log, "particle.emit : cannot find particle emitter with that id");
        Lua_requestClose = true;
        return 0;
    }

    PS[IDX].DeleteOnFinish = lua_toboolean(Lua_State, -1);
    return 0;
}

static int Lua_particleScolor(lua_State *L)
{
    if(!Lua_ParcticleS_List)
        return 0;

    if(!lua_isnumber(Lua_State, -9))
    {
        fprintf(Log, "particle.setColor : argument 1 (ID) needs to be a number");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -8))
    {
        fprintf(Log, "particle.setColor : argument 2 needs to be a number");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -7))
    {
        fprintf(Log, "particle.setColor : argument 3 needs to be a number");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -6))
    {
        fprintf(Log, "particle.setColor : argument 4 needs to be a number");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -5))
    {
        fprintf(Log, "particle.setColor : argument 5 needs to be a number");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -4))
    {
        fprintf(Log, "particle.setColor : argument 6 needs to be a number");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -3))
    {
        fprintf(Log, "particle.setColor : argument 7 needs to be a number");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(Log, "particle.setColor : argument 8 needs to be a number");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(Log, "particle.setColor : argument 9 needs to be a number");
        Lua_requestClose = true;
        return 0;
    }

    int ID = lua_tonumber(Lua_State, -9);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(Log, "particle.setColor : argument 1 is an invalid ID");
        Lua_requestClose = true;
        return 0;
    }

    int IDX = -1;
    struct Particle_Emitter *PS = Lua_ParticleSystem->Emitters;
    for(int i = 0; i < Lua_ParticleSystem->Emitters_Count; i++)
    {
        if(PS[i].ID == ID)
        {
            IDX = i;
            break;
        }
    }

    if(IDX == -1)
    {
        fprintf(Log, "particle.setColor : cannot find particle emitter with that id");
        Lua_requestClose = true;
        return 0;
    }

    Lua_ParticleSystem->Emitters[IDX].Color_Start = Vector4_Create(lua_tonumber(Lua_State, -8), lua_tonumber(Lua_State, -7), lua_tonumber(Lua_State, -6), lua_tonumber(Lua_State, -5));
    Lua_ParticleSystem->Emitters[IDX].Color_End = Vector4_Create(lua_tonumber(Lua_State, -4), lua_tonumber(Lua_State, -3), lua_tonumber(Lua_State, -2), lua_tonumber(Lua_State, -1));

    return 0;
}

static int Lua_particleSgravityType(lua_State *L)
{
    if(!Lua_ParcticleS_List)
        return 0;

    if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(Log, "particle.gravityType needs ID as argument");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isstring(Lua_State, -1))
    {
        fprintf(Log, "particle.gravityType needs String as argument 2");
        Lua_requestClose = true;
        return 0;
    }

    int ID = lua_tonumber(Lua_State, -2);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(Log, "particle.setColor : argument 1 is an invalid ID");
        Lua_requestClose = true;
        return 0;
    }

    int IDX = -1;
    struct Particle_Emitter *PS = Lua_ParticleSystem->Emitters;
    for(int i = 0; i < Lua_ParticleSystem->Emitters_Count; i++)
    {
        if(PS[i].ID == ID)
        {
            IDX = i;
            break;
        }
    }

    if(IDX == -1)
    {
        fprintf(Log, "particle.setColor : cannot find particle emitter with that id");
        Lua_requestClose = true;
        return 0;
    }

    const char *str = lua_tostring(Lua_State, -1);

    if(!strcmp(str, "down"))
        Lua_ParticleSystem->Emitters[IDX].Gravity_Type = GRAVITY_TYPE_DOWN;
    else if(!strcmp(str, "up"))
        Lua_ParticleSystem->Emitters[IDX].Gravity_Type = GRAVITY_TYPE_UP;
    else if(!strcmp(str, "right"))
        Lua_ParticleSystem->Emitters[IDX].Gravity_Type = GRAVITY_TYPE_RIGHT;
    else if(!strcmp(str, "left"))
        Lua_ParticleSystem->Emitters[IDX].Gravity_Type = GRAVITY_TYPE_LEFT;
    else if(!strcmp(str, "other"))
        Lua_ParticleSystem->Emitters[IDX].Gravity_Type = GRAVITY_TYPE_OTHER;
    else
    {
        fprintf(Log, "particle.setColor : cannot find type math with argument 2");
        Lua_requestClose = true;
    }

    return 0;
}

static int Lua_particleSparticles(lua_State *L)
{
    if(!Lua_ParcticleS_List)
        return 0;

    if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(Log, "particle.maxParticles needs ID as argument");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(Log, "particle.maxParticles needs number as argument 2");
        Lua_requestClose = true;
        return 0;
    }

    int ID = lua_tonumber(Lua_State, -2);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(Log, "particle.maxParticles : argument 1 is an invalid ID");
        Lua_requestClose = true;
        return 0;
    }

    int IDX = -1;
    struct Particle_Emitter *PS = Lua_ParticleSystem->Emitters;
    for(int i = 0; i < Lua_ParticleSystem->Emitters_Count; i++)
    {
        if(PS[i].ID == ID)
        {
            IDX = i;
            break;
        }
    }

    if(IDX == -1)
    {
        fprintf(Log, "particle.maxParticles : cannot find particle emitter with that id");
        Lua_requestClose = true;
        return 0;
    }

    Lua_ParticleSystem->Emitters[IDX].Particle_Count = lua_tonumber(Lua_State, -1);

    return 0;
}

static int Lua_particleSlife(lua_State *L)
{
    if(!Lua_ParcticleS_List)
        return 0;

    if(!lua_isnumber(Lua_State, -3))
    {
        fprintf(Log, "particle.setLife needs ID as argument 1");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(Log, "particle.setLife needs number as argument 2");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(Log, "particle.setLife needs number as argument 3");
        Lua_requestClose = true;
        return 0;
    }

    int ID = lua_tonumber(Lua_State, -3);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(Log, "particle.setLife : argument 1 is an invalid ID");
        Lua_requestClose = true;
        return 0;
    }

    int IDX = -1;
    struct Particle_Emitter *PS = Lua_ParticleSystem->Emitters;
    for(int i = 0; i < Lua_ParticleSystem->Emitters_Count; i++)
    {
        if(PS[i].ID == ID)
        {
            IDX = i;
            break;
        }
    }

    if(IDX == -1)
    {
        fprintf(Log, "particle.setLife : cannot find particle emitter with that id");
        Lua_requestClose = true;
        return 0;
    }

    Lua_ParticleSystem->Emitters[IDX].Min_Life = lua_tonumber(Lua_State, -2);
    Lua_ParticleSystem->Emitters[IDX].Max_Life = lua_tonumber(Lua_State, -1);

    return 0;
}

static int Lua_particleSspeed(lua_State *L)
{
    if(!Lua_ParcticleS_List)
        return 0;

    if(!lua_isnumber(Lua_State, -3))
    {
        fprintf(Log, "particle.setSpeed needs ID as argument 1");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(Log, "particle.setSpeed needs number as argument 2");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(Log, "particle.setSpeed needs number as argument 3");
        Lua_requestClose = true;
        return 0;
    }

    int ID = lua_tonumber(Lua_State, -3);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(Log, "particle.setSpeed : argument 1 is an invalid ID");
        Lua_requestClose = true;
        return 0;
    }

    int IDX = -1;
    struct Particle_Emitter *PS = Lua_ParticleSystem->Emitters;
    for(int i = 0; i < Lua_ParticleSystem->Emitters_Count; i++)
    {
        if(PS[i].ID == ID)
        {
            IDX = i;
            break;
        }
    }

    if(IDX == -1)
    {
        fprintf(Log, "particle.setSpeed : cannot find particle emitter with that id");
        Lua_requestClose = true;
        return 0;
    }

    Lua_ParticleSystem->Emitters[IDX].Min_Speed = lua_tonumber(Lua_State, -2);
    Lua_ParticleSystem->Emitters[IDX].Max_Speed = lua_tonumber(Lua_State, -1);

    return 0;
}

static int Lua_particleSgravity(lua_State *L)
{
    if(!Lua_ParcticleS_List)
        return 0;

    if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(Log, "particle.setGravity needs ID as argument");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(Log, "particle.setGravity needs number as argument 2");
        Lua_requestClose = true;
        return 0;
    }

    int ID = lua_tonumber(Lua_State, -2);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(Log, "particle.setGravity : argument 1 is an invalid ID");
        Lua_requestClose = true;
        return 0;
    }

    int IDX = -1;
    struct Particle_Emitter *PS = Lua_ParticleSystem->Emitters;
    for(int i = 0; i < Lua_ParticleSystem->Emitters_Count; i++)
    {
        if(PS[i].ID == ID)
        {
            IDX = i;
            break;
        }
    }

    if(IDX == -1)
    {
        fprintf(Log, "particle.setGravity : cannot find particle emitter with that id");
        Lua_requestClose = true;
        return 0;
    }

    double value = lua_tonumber(Lua_State, -1);

    Lua_ParticleSystem->Emitters[IDX].Gravity = value;

    return 0;
}

static int Lua_particleSposition(lua_State *L)
{
    if(!Lua_ParcticleS_List)
        return 0;

    if(!lua_isnumber(Lua_State, -3))
    {
        fprintf(Log, "particle.setPosition needs ID as argument 1");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(Log, "particle.setPosition needs number as argument 2");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(Log, "particle.setPosition needs number as argument 3");
        Lua_requestClose = true;
        return 0;
    }

    int ID = lua_tonumber(Lua_State, -3);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(Log, "particle.setPosition : argument 1 is an invalid ID");
        Lua_requestClose = true;
        return 0;
    }

    int IDX = -1;
    struct Particle_Emitter *PS = Lua_ParticleSystem->Emitters;
    for(int i = 0; i < Lua_ParticleSystem->Emitters_Count; i++)
    {
        if(PS[i].ID == ID)
        {
            IDX = i;
            break;
        }
    }

    if(IDX == -1)
    {
        fprintf(Log, "particle.setPosition : cannot find particle emitter with that id");
        Lua_requestClose = true;
        return 0;
    }

    PS[IDX].x = lua_tonumber(Lua_State, -2);
    PS[IDX].y = lua_tonumber(Lua_State, -1);

    return 0;
}

static int Lua_particleSgravityCenter(lua_State *L)
{
    if(!Lua_ParcticleS_List)
        return 0;

    if(!lua_isnumber(Lua_State, -3))
    {
        fprintf(Log, "particle.gravityCenter needs ID as argument 1");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(Log, "particle.gravityCenter needs number as argument 2");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(Log, "particle.gravityCenter needs number as argument 3");
        Lua_requestClose = true;
        return 0;
    }

    int ID = lua_tonumber(Lua_State, -3);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(Log, "particle.gravityCenter : argument 1 is an invalid ID");
        Lua_requestClose = true;
        return 0;
    }

    int IDX = -1;
    struct Particle_Emitter *PS = Lua_ParticleSystem->Emitters;
    for(int i = 0; i < Lua_ParticleSystem->Emitters_Count; i++)
    {
        if(PS[i].ID == ID)
        {
            IDX = i;
            break;
        }
    }

    if(IDX == -1)
    {
        fprintf(Log, "particle.gravityCenter : cannot find particle emitter with that id");
        Lua_requestClose = true;
        return 0;
    }

    PS[IDX].Gravity_Center = Vector2_Create(lua_tonumber(Lua_State, -2), lua_tonumber(Lua_State, -1));

    return 0;
}

static int Lua_particleGetCount(lua_State *L)
{
    if(!Lua_ParcticleS_List)
    {
        lua_pushnumber(Lua_State, 0);
        return 1;
    }

    if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(Log, "particle.getCount needs ID as argument");
        Lua_requestClose = true;
        lua_pushnumber(Lua_State, 0);
        return 1;
    }

    int ID = lua_tonumber(Lua_State, -1);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(Log, "particle.getCount : argument 1 is an invalid ID");
        Lua_requestClose = true;
        lua_pushnumber(Lua_State, 0);
        return 1;
    }

    int IDX = -1;
    struct Particle_Emitter *PS = Lua_ParticleSystem->Emitters;
    for(int i = 0; i < Lua_ParticleSystem->Emitters_Count; i++)
    {
        if(PS[i].ID == ID)
        {
            IDX = i;
            break;
        }
    }

    if(IDX == -1)
    {
        fprintf(Log, "particle.getCount : cannot find particle emitter with that id");
        Lua_requestClose = true;
        lua_pushnumber(Lua_State, 0);
        return 1;
    }

    lua_pushnumber(Lua_State, PS[IDX].Particle_Count);
    return 1;
}

static int Lua_particleFree(lua_State *L)
{
    if(!Lua_ParcticleS_List)
        return 0;

    if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(Log, "particle.free needs ID as argument");
        Lua_requestClose = true;
        return 0;
    }

    int ID = lua_tonumber(Lua_State, -1);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(Log, "particle.free : argument is an invalid ID");
        Lua_requestClose = true;
        return 0;
    }

    int IDX = -1;
    struct Particle_Emitter *PS = Lua_ParticleSystem->Emitters;
    for(int i = 0; i < Lua_ParticleSystem->Emitters_Count; i++)
    {
        if(PS[i].ID == ID)
        {
            IDX = i;
            break;
        }
    }

    if(IDX == -1)
    {
        fprintf(Log, "particle.free : cannot find particle emitter with that id");
        Lua_requestClose = true;
        return 0;
    }

    Particle_Emitter_Free(Lua_ParticleSystem, IDX);

    struct Lua_ParcticleS *SS = Lua_ParcticleS_List->items;
    int IDX2 = 0;
    for(int i = 0; i < Lua_ParcticleS_List->size; i++)
    {
        if(SS[IDX2].ID == ID)
        {
            IDX2 = i;
            break;
        }
    }
    vector_erase(Lua_ParcticleS_List, IDX2);
    if(!Lua_ParcticleS_List->size)
    {
        vector_delete(Lua_ParcticleS_List);
        Particle_System_Free(Lua_ParticleSystem);
        Lua_ParticleSystem = NULL;
        Lua_ParcticleS_List = NULL;
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
		Lua_requestClose = true;
		lua_pushnumber(L, -1.0);
		return 1;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(Log, "Error creating button, argument 2 needs to be a number\n");
		Lua_requestClose = true;
		lua_pushnumber(L, -1.0);
		return 1;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(Log, "Error creating button, argument 3 needs to be a number\n");
		Lua_requestClose = true;
		lua_pushnumber(L, -1.0);
		return 1;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(Log, "Error creating button, argument 4 needs to be a number\n");
		Lua_requestClose = true;
		lua_pushnumber(L, -1.0);
		return 1;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(Log, "Error creating button, argument 5 needs to be a number\n");
		Lua_requestClose = true;
		lua_pushnumber(L, -1.0);
		return 1;
	}

	if(!Lua_Button_List)
	{
		Lua_Button_List = vector_new(sizeof(struct Lua_Button));
		Gui_Button_NextID = 0.0;
	}

	double x = lua_tonumber(L, -4), y = lua_tonumber(L, -3), width = lua_tonumber(L, -2), height = lua_tonumber(L, -1);
	const char *str = lua_tostring(L, -5);

	struct Lua_Button lg = { Gui_Button_NextID, NULL };

	lg.B = Gui_Button_Create(x + Lua_Window.x, y + Lua_Window.y, width, height, str, 0, 0, width, height, 0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
	lg.B->CenterText = true;
	vector_push_back(Lua_Button_List, &lg);

	lua_pushnumber(Lua_State, lg.ID);
	Gui_Button_NextID++;
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
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -8))
	{
		fprintf(Log, "Error coloring button, argument 2 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -7))
	{
		fprintf(Log, "Error coloring button, argument 3 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -6))
	{
		fprintf(Log, "Error coloring button, argument 4 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -5))
	{
		fprintf(Log, "Error coloring button, argument 5 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(Log, "Error coloring button, argument 6 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(Log, "Error coloring button, argument 7 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(Log, "Error coloring button, argument 8 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(Log, "Error coloring button, argument 9 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}

	double ID = lua_tonumber(L, -9);

	if(ID < 0 || ID >= Gui_Button_NextID)
	{
		fprintf(Log, "Error coloring button, argument 1 needs to be a valid button");
		Lua_requestClose = true;
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
		Lua_requestClose = true;
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
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(Log, "Error creating button, argument 2 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(Log, "Error creating button, argument 3 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(Log, "Error creating button, argument 4 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(Log, "Error creating button, argument 5 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}

	double ID = lua_tonumber(L, -5);

	if(ID < 0 || ID >= Gui_Button_NextID)
	{
		fprintf(Log, "Error coloring button, argument 1 needs to be a valid button");
		Lua_requestClose = true;
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
		Lua_requestClose = true;
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
		lua_pushboolean(Lua_State, false);
		return 1;
	}

	double ID = lua_tonumber(L, -1);

	if(ID < 0 || ID >= Gui_Button_NextID)
	{
		fprintf(Log, "Error rendering button, argument needs to be a valid button");
		lua_pushboolean(Lua_State, false);
		return 1;
	}

	struct Lua_Button *lb = Lua_Button_List->items;
	struct Gui_Button *b = NULL;
	for(int i = 0; i < Lua_Button_List->size; i++)
	{
		if(lb[i].ID == ID) {
			b = lb[i].B;
			break;
		}
	}
	if(!b)
	{
		fprintf(Log, "Error rendering button, didn't find button");
		lua_pushboolean(Lua_State, false);
		return 1;
	}

	lua_pushboolean(Lua_State, Gui_Button_Render(b));
	return 1;
}

static int Lua_resizeButton(lua_State *L)
{
	if(!Lua_Button_List)
	{
		return 0;
	}

	if(!lua_isnumber(L, -5))
	{
		fprintf(Log, "Error resizing button, argument 1 needs to be a number");
		return 0;
	} 	if(!lua_isnumber(L, -4))
	{
		fprintf(Log, "Error resizing button, argument 2 needs to be a number");
		return 0;
	} 	if(!lua_isnumber(L, -3))
	{
		fprintf(Log, "Error resizing button, argument 3 needs to be a number");
		return 0;
	} 	if(!lua_isnumber(L, -2))
	{
		fprintf(Log, "Error resizing button, argument 4 needs to be a number");
		return 0;
	} 	if(!lua_isnumber(L, -0))
	{
		fprintf(Log, "Error resizing button, argument 5 needs to be a number");
		return 0;
	}

	double ID = lua_tonumber(L, -5);

	if(ID < 0 || ID >= Gui_Button_NextID)
	{
		fprintf(Log, "Error resizing button, argument needs to be a valid button");
		return 0;
	}

	struct Lua_Button *lb = Lua_Button_List->items;
	struct Gui_Button *b = NULL;
	for(int i = 0; i < Lua_Button_List->size; i++)
	{
		if(lb[i].ID == ID) {
			b = lb[i].B;
			break;
		}
	}
	if(!b)
	{
		fprintf(Log, "Error resizing button, didn't find button");
		return 0;
	}

	double X = lua_tonumber(L, -4) + Lua_Window.x, Y = lua_tonumber(L, -3) + Lua_Window.y, W = lua_tonumber(L, -2), H = lua_tonumber(L, -1);
	Gui_Button_Resize(b, X, Y, W, H);
	b->TextWidth = W;
	b->TextHeight = H;
	return 0;
}

static int Lua_freeButton(lua_State *L)
{
	if(!Lua_Texture_List) return 0;

	if(!lua_isnumber(L, -1))
	{
		fprintf(Log, "Error freeing button, argument needs to be a number\n");
		Lua_requestClose = true;
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

	if(!Lua_Button_List->size)
    {
        vector_delete(Lua_Button_List);
        Particle_System_Free(Lua_ParticleSystem);
        Lua_ParticleSystem = NULL;
        Lua_Button_List = NULL;
    }

	return 0;
}

// Gui TextBox Management
double Gui_TextBox_NextID = 0.0;
struct Lua_TextBox { double ID; struct Gui_TextBox *B; _Bool ShowKeyboard; };
struct vector_t *Lua_TextBox_List = NULL;

static int Lua_newTextBox(lua_State *L)
{
	if(!App_UsingDisplay)
	{
		return 0;
	}
	if(!lua_isstring(L, -5))
	{
		fprintf(Log, "Error creating text box, argument 1 needs to be a string\n");
		Lua_requestClose = true;
		lua_pushnumber(L, -1.0);
		return 1;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(Log, "Error creating text box, argument 2 needs to be a number\n");
		Lua_requestClose = true;
		lua_pushnumber(L, -1.0);
		return 1;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(Log, "Error creating text box, argument 3 needs to be a number\n");
		Lua_requestClose = true;
		lua_pushnumber(L, -1.0);
		return 1;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(Log, "Error creating text box, argument 4 needs to be a number\n");
		Lua_requestClose = true;
		lua_pushnumber(L, -1.0);
		return 1;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(Log, "Error creating text box, argument 5 needs to be a number\n");
		Lua_requestClose = true;
		lua_pushnumber(L, -1.0);
		return 1;
	}

	if(!Lua_TextBox_List)
	{
		Lua_TextBox_List = vector_new(sizeof(struct Lua_TextBox));
		Gui_TextBox_NextID = 0.0;
	}

	double x = lua_tonumber(L, -4), y = lua_tonumber(L, -3), width = lua_tonumber(L, -2), height = lua_tonumber(L, -1);
	const char *str = lua_tostring(L, -5);

	struct Lua_TextBox lg = { Gui_TextBox_NextID, NULL , true};

	lg.B = Gui_TextBox_Create(x + Lua_Window.x, y + Lua_Window.y, width, height, str, 10, 0, 0, height, width, 0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
	lg.B->Quad_takeMouse = Quad_Create(Lua_Window.x, Lua_Window.y, Lua_Window.x, Lua_Window.y + Lua_Window.w, Lua_Window.x + Lua_Window.z, Lua_Window.y + Lua_Window.w, Lua_Window.x + Lua_Window.z, Lua_Window.y);
	vector_push_back(Lua_TextBox_List, &lg);

	lua_pushnumber(Lua_State, lg.ID);
	Gui_TextBox_NextID++;
	return 1;
}

static int Lua_LettersTextBox(lua_State *L)
{
	if(!Lua_TextBox_List)
		return 0;

	if(!lua_isnumber(L, -2))
	{
		fprintf(Log, "Error textbox.letters in text box, argument 1 needs to be a number");
		return 0;
	} 	if(!lua_isboolean(L, -1))
	{
		fprintf(Log, "Error textbox.letters in text box, argument 2 needs to be a boolean");
		return 0;
	}

	double ID = lua_tonumber(L, -2);

	if(ID < 0 || ID >= Gui_TextBox_NextID)
	{
		fprintf(Log, "Error textbox.letters, argument needs to be a valid button");
		return 0;
	}

	struct Lua_TextBox *lb = Lua_TextBox_List->items;
	struct Gui_TextBox *b = NULL;
	for(int i = 0; i < Lua_TextBox_List->size; i++)
	{
		if(lb[i].ID == ID) {
			b = lb[i].B;
			break;
		}
	}
	if(!b)
	{
		fprintf(Log, "Error textbox.letters, didn't find button");
		return 0;
	}

	b->AcceptLetters = lua_toboolean(L, -1);
	return 0;
}

static int Lua_NumbersTextBox(lua_State *L)
{
	if(!Lua_TextBox_List)
		return 0;

	if(!lua_isnumber(L, -2))
	{
		fprintf(Log, "Error textbox.numbers in text box, argument 1 needs to be a number");
		return 0;
	} 	if(!lua_isboolean(L, -1))
	{
		fprintf(Log, "Error textbox.numbers in text box, argument 2 needs to be a boolean");
		return 0;
	}

	double ID = lua_tonumber(L, -2);

	if(ID < 0 || ID >= Gui_TextBox_NextID)
	{
		fprintf(Log, "Error textbox.numbers, argument needs to be a valid button");
		return 0;
	}

	struct Lua_TextBox *lb = Lua_TextBox_List->items;
	struct Gui_TextBox *b = NULL;
	for(int i = 0; i < Lua_TextBox_List->size; i++)
	{
		if(lb[i].ID == ID) {
			b = lb[i].B;
			break;
		}
	}
	if(!b)
	{
		fprintf(Log, "Error textbox.numbers, didn't find button");
		return 0;
	}

	b->AcceptNumbers = lua_toboolean(L, -1);
	return 0;
}

static int Lua_DotTextBox(lua_State *L)
{
	if(!Lua_TextBox_List)
		return 0;

	if(!lua_isnumber(L, -2))
	{
		fprintf(Log, "Error textbox.dot in text box, argument 1 needs to be a number");
		return 0;
	} 	if(!lua_isboolean(L, -1))
	{
		fprintf(Log, "Error textbox.dot in text box, argument 2 needs to be a boolean");
		return 0;
	}

	double ID = lua_tonumber(L, -2);

	if(ID < 0 || ID >= Gui_TextBox_NextID)
	{
		fprintf(Log, "Error textbox.dot, argument needs to be a valid button");
		return 0;
	}

	struct Lua_TextBox *lb = Lua_TextBox_List->items;
	struct Gui_TextBox *b = NULL;
	for(int i = 0; i < Lua_TextBox_List->size; i++)
	{
		if(lb[i].ID == ID) {
			b = lb[i].B;
			break;
		}
	}
	if(!b)
	{
		fprintf(Log, "Error textbox.dot, didn't find button");
		return 0;
	}

	b->AcceptDot = lua_toboolean(L, -1);
	return 0;
}

static int Lua_SymbolsTextBox(lua_State *L)
{
	if(!Lua_TextBox_List)
		return 0;

	if(!lua_isnumber(L, -2))
	{
		fprintf(Log, "Error textbox.symbols in text box, argument 1 needs to be a number");
		return 0;
	} 	if(!lua_isboolean(L, -1))
	{
		fprintf(Log, "Error textbox.symbols in text box, argument 2 needs to be a boolean");
		return 0;
	}

	double ID = lua_tonumber(L, -2);

	if(ID < 0 || ID >= Gui_TextBox_NextID)
	{
		fprintf(Log, "Error textbox.symbols, argument needs to be a valid button");
		return 0;
	}

	struct Lua_TextBox *lb = Lua_TextBox_List->items;
	struct Gui_TextBox *b = NULL;
	for(int i = 0; i < Lua_TextBox_List->size; i++)
	{
		if(lb[i].ID == ID) {
			b = lb[i].B;
			break;
		}
	}
	if(!b)
	{
		fprintf(Log, "Error textbox.symbols, didn't find button");
		return 0;
	}

	b->AcceptDot = lua_toboolean(L, -1);
	return 0;
}

static int Lua_colorTextBox(lua_State *L)
{
	if(!Lua_TextBox_List)
	{
		return 0;
	}

	if(!lua_isnumber(L, -9))
	{
		fprintf(Log, "Error coloring text box, argument 1 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -8))
	{
		fprintf(Log, "Error coloring text box, argument 2 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -7))
	{
		fprintf(Log, "Error coloring text box, argument 3 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -6))
	{
		fprintf(Log, "Error coloring text box, argument 4 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -5))
	{
		fprintf(Log, "Error coloring text box, argument 5 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(Log, "Error coloring text box, argument 6 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(Log, "Error coloring text box, argument 7 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(Log, "Error coloring text box, argument 8 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(Log, "Error coloring text box, argument 9 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}

	double ID = lua_tonumber(L, -9);

	if(ID < 0 || ID >= Gui_TextBox_NextID)
	{
		fprintf(Log, "Error coloring text box, argument 1 needs to be a valid button");
		Lua_requestClose = true;
		return 0;
	}

	struct Lua_TextBox *lb = Lua_TextBox_List->items;
	struct Gui_TextBox *bu = NULL;
	for(int i = 0; i < Lua_TextBox_List->size; i++)
		if(lb[i].ID == ID) {
			bu = lb[i].B;
			break;
		}
	if(!bu)
	{
		fprintf(Log, "Error coloring text box, didn't find button");
		Lua_requestClose = true;
		return 0;
	}

	double r = lua_tonumber(L, -8), g = lua_tonumber(L, -7), b = lua_tonumber(L, -6), a = lua_tonumber(L, -5), rr = lua_tonumber(L, -4), gg = lua_tonumber(L, -3), bb = lua_tonumber(L, -2), aa = lua_tonumber(L, -1);
	bu->Color = Vector4_Create(r, g, b, a);
	bu->HoverColor = Vector4_Create(rr, gg, bb, aa);

	return 0;
}

static int Lua_colorTextBoxText(lua_State *L)
{
	if(!App_UsingDisplay)
	{
		return 0;
	}
	if(!Lua_TextBox_List)
	{
		return 0;
	}

	if(!lua_isnumber(L, -5))
	{
		fprintf(Log, "Error textbox.textColor, argument 1 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(Log, "Error textbox.textColor, argument 2 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(Log, "Error textbox.textColor, argument 3 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(Log, "Error textbox.textColor, argument 4 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(Log, "Error textbox.textColor, argument 5 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}

	double ID = lua_tonumber(L, -5);

	if(ID < 0 || ID >= Gui_TextBox_NextID)
	{
		fprintf(Log, "Error textbox.textColor, argument 1 needs to be a valid button");
		Lua_requestClose = true;
		return 0;
	}

	struct Lua_TextBox *lb = Lua_TextBox_List->items;
	struct Gui_TextBox *bu = NULL;
	for(int i = 0; i < Lua_TextBox_List->size; i++)
		if(lb[i].ID == ID) {
			bu = lb[i].B;
			break;
		}
	if(!bu)
	{
		fprintf(Log, "Error textbox.textColor, didn't find button");
		Lua_requestClose = true;
		return 0;
	}

	bu->Font_Color = Vector4_Create(lua_tonumber(L, -4), lua_tonumber(L, -3), lua_tonumber(L, -2), lua_tonumber(L, -1));
	return 0;
}

static int Lua_resizeTextBox(lua_State *L)
{
	if(!App_UsingDisplay)
	{
		return 0;
	}
	if(!Lua_TextBox_List)
	{
		return 0;
	}

	if(!lua_isnumber(L, -5))
	{
		fprintf(Log, "Error textbox.resize, argument 1 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(Log, "Error textbox.resize, argument 2 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(Log, "Error textbox.resize, argument 3 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(Log, "Error textbox.resize, argument 4 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(Log, "Error textbox.resize, argument 5 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}

	double ID = lua_tonumber(L, -5);

	if(ID < 0 || ID >= Gui_TextBox_NextID)
	{
		fprintf(Log, "Error textbox.resize, argument 1 needs to be a valid button");
		Lua_requestClose = true;
		return 0;
	}

	struct Lua_TextBox *lb = Lua_TextBox_List->items;
	struct Gui_TextBox *bu = NULL;
	for(int i = 0; i < Lua_TextBox_List->size; i++)
		if(lb[i].ID == ID) {
			bu = lb[i].B;
			break;
		}
	if(!bu)
	{
		fprintf(Log, "Error textbox.resize, didn't find button");
		Lua_requestClose = true;
		return 0;
	}

	double X = lua_tonumber(L, -4) + Lua_Window.x, Y = lua_tonumber(L, -3) + Lua_Window.y, W = lua_tonumber(L, -2), H = lua_tonumber(L, -1);
	Gui_TextBox_Resize(bu, X, Y, W, H);
	bu->TextHeight = H;
	bu->TextMaxWidth = W;
	bu->Quad_takeMouse = Quad_Create(Lua_Window.x, Lua_Window.y, Lua_Window.x, Lua_Window.y + Lua_Window.w, Lua_Window.x + Lua_Window.z, Lua_Window.y + Lua_Window.w, Lua_Window.x + Lua_Window.z, Lua_Window.y);
	return 0;
}


static int Lua_renderTextBox(lua_State *L)
{
	if(!Lua_TextBox_List)
	{
		lua_pushboolean(Lua_State, false);
		return 1;
	}

	if(!lua_isnumber(L, -1))
	{
		fprintf(Log, "Error rendering text box, argument needs to be a number");
		lua_pushboolean(Lua_State, false);
		return 1;
	}

	double ID = lua_tonumber(L, -1);

	if(ID < 0 || ID >= Gui_TextBox_NextID)
	{
		fprintf(Log, "Error rendering text box, argument needs to be a valid button");
		lua_pushboolean(Lua_State, false);
		return 1;
	}

	struct Lua_TextBox *lb = Lua_TextBox_List->items;
	struct Gui_TextBox *b = NULL;
	for(int i = 0; i < Lua_TextBox_List->size; i++)
	{
		if(lb[i].ID == ID) {
			b = lb[i].B;
			break;
		}
	}
	if(!b)
	{
		fprintf(Log, "Error rendering text box, didn't find button");
		lua_pushboolean(Lua_State, false);
		return 1;
	}

	if(lb->ShowKeyboard) {
		if(Mouse.justReleased && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), Quad_Create(b->X, b->Y, b->X, b->Y + b->Height, b->X + b->Width, b->Y + b->Height, b->X + b->Width, b->Y)))
			Lua_OpenKeyboard(NULL);
		else if(Mouse.justReleased && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), b->Quad_takeMouse))
			Lua_CloseKeyboard(NULL);
	}

	Gui_TextBox_Render(b);
	return 1;
}

static int Lua_getTextTextBox(lua_State *L)
{
	if(!Lua_TextBox_List)
	{
		lua_pushboolean(Lua_State, false);
		return 1;
	}

	if(!lua_isnumber(L, -1))
	{
		fprintf(Log, "Error textbox.text in text box, argument needs to be a number");
		lua_pushboolean(Lua_State, false);
		return 1;
	}

	double ID = lua_tonumber(L, -1);

	if(ID < 0 || ID >= Gui_TextBox_NextID)
	{
		fprintf(Log, "Error textbox.text, argument needs to be a valid button");
		lua_pushboolean(Lua_State, false);
		return 1;
	}

	struct Lua_TextBox *lb = Lua_TextBox_List->items;
	struct Gui_TextBox *b = NULL;
	for(int i = 0; i < Lua_TextBox_List->size; i++)
	{
		if(lb[i].ID == ID) {
			b = lb[i].B;
			break;
		}
	}
	if(!b)
	{
		fprintf(Log, "Error textbox.text, didn't find button");
		lua_pushboolean(Lua_State, false);
		return 1;
	}

	lua_pushstring(Lua_State, b->Value);
	return 1;
}

static int Lua_setTextTextBox(lua_State *L)
{
	if(!Lua_TextBox_List)
	{
		lua_pushstring(Lua_State, "0");
		return 1;
	}

	if(!lua_isnumber(L, -2))
	{
		fprintf(Log, "Error textbox.set, argument 1 needs to be a number");
		lua_pushstring(Lua_State, "0");
		return 1;
	}
	if(!lua_isstring(L, -1))
	{
		fprintf(Log, "Error textbox.set, argument 2 needs to be a string");
		lua_pushstring(Lua_State, "0");
		return 1;
	}

	double ID = lua_tonumber(L, -2);

	if(ID < 0 || ID >= Gui_TextBox_NextID)
	{
		fprintf(Log, "Error textbox.set, argument 1 needs to be a valid button");
		lua_pushstring(Lua_State, "0");
		return 1;
	}

	struct Lua_TextBox *lb = Lua_TextBox_List->items;
	struct Gui_TextBox *b = NULL;
	for(int i = 0; i < Lua_TextBox_List->size; i++)
	{
		if(lb[i].ID == ID) {
			b = lb[i].B;
			break;
		}
	}
	if(!b)
	{
		fprintf(Log, "Error textbox.set, didn't find button");
		lua_pushstring(Lua_State, "0");
		return 1;
	}

	const char *Value = lua_tostring(L, -1);
	memcpy(b->Value, Value, sizeof(char) * String_length(Value));

	return 1;
}

static int Lua_freeTextBox(lua_State *L)
{
	if(!Lua_Texture_List) return 0;

	if(!lua_isnumber(L, -1))
	{
		fprintf(Log, "Error freeing button, argument needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}

	double ID = lua_tonumber(L, -1);

	struct Lua_TextBox *lb = Lua_TextBox_List->items;

	for(int i = 0; i < Lua_TextBox_List->size; i++)
	{
		if(lb[i].ID == ID)
		{
			Gui_TextBox_Free_Simple(lb[i].B);
			vector_erase(Lua_TextBox_List, i);
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
		Lua_requestClose = true;
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
		fprintf(Log, "renderer.fixedText : need to call have useDisplay to true to get access to rendering functions\n");
		Lua_requestClose = true;
	}
	if(!lua_isstring(L, -9))
	{
		fprintf(Log, "renderer.fixedText : need string for argument 1\n");
		return 0;
	}
	if(!lua_isnumber(L, -8))
	{
		fprintf(Log, "renderer.fixedText : need number for argument 2\n");
		return 0;
	}
	if(!lua_isnumber(L, -7))
	{
		fprintf(Log, "renderer.fixedText : need number for argument 3\n");
		return 0;
	}
	if(!lua_isnumber(L, -6))
	{
		fprintf(Log, "renderer.fixedText : need number for argument 4\n");
		return 0;
	}
	if(!lua_isnumber(L, -5))
	{
		fprintf(Log, "renderer.fixedText : need number for argument 5\n");
		return 0;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(Log, "renderer.fixedText : need number for argument 6\n");
		return 0;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(Log, "renderer.fixedText : need number for argument 7\n");
		return 0;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(Log, "renderer.fixedText : need number for argument 8\n");
		return 0;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(Log, "renderer.fixedText : need number for argument 9\n");
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
		{"openKeyboard", Lua_OpenKeyboard},
		{"closeKeyboard", Lua_CloseKeyboard},
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
			{"resize", Lua_resizeButton},
			{"free", Lua_freeButton},
			{NULL, NULL}
    };

    static const luaL_Reg TextBox_Functions[] = {
    		{"new", Lua_newTextBox},
			{"color", Lua_colorTextBox},
			{"colorText", Lua_colorTextBoxText},
			{"render", Lua_renderTextBox},
			{"resize", Lua_resizeTextBox},
			{"text", Lua_getTextTextBox},
			{"set", Lua_setTextTextBox},
			{"free", Lua_freeTextBox},
			{"letters", Lua_LettersTextBox},
			{"numbers", Lua_NumbersTextBox},
			{"dot", Lua_DotTextBox},
			{"symbols", Lua_SymbolsTextBox},
			{NULL, NULL}
    };

    static const luaL_Reg Animation_Functions[] = {
        {"load", Lua_loadAnimation},
        {"setPos", Lua_setPosAnimation},
        {"setSize", Lua_setSizeAnimation},
        {"setAngle", Lua_setAngleAnimation},
        {"timePerFrame", Lua_setFrameTimeAnimation},
        {"reverseOnFinish", Lua_setReverseOnFinish},
        {"render", Lua_renderAnimation},
        {"free", Lua_freeAnimation},
        {NULL, NULL}
    };

    static const luaL_Reg Particle_Functions[] = {
        {"new", Lua_particleSnew},
        {"maxParticles", Lua_particleSparticles},
        {"setColor", Lua_particleScolor},
        {"getCount", Lua_particleGetCount},
        {"setPosition", Lua_particleSposition},
        {"setSpeed", Lua_particleSspeed},
        {"deleteOnFinish", Lua_particleSetDeleteOnFinish},
        {"toggleEmitting", Lua_particleToggleEmitting},
        {"emit", Lua_particleEmit},
        {"gravityCenter", Lua_particleSgravityCenter},
        {"setGravity", Lua_particleSgravity},
        {"gravityType", Lua_particleSgravityType},
        {"setLife", Lua_particleSlife},
        {"free", Lua_particleFree},
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

    lua_newtable(Lua_State);
    luaL_setfuncs(Lua_State, TextBox_Functions, 0);
    lua_setglobal(Lua_State, "textbox");

    lua_newtable(Lua_State);
    luaL_setfuncs(Lua_State, Animation_Functions, 0);
    lua_setglobal(Lua_State, "animation");

    lua_newtable(Lua_State);
    luaL_setfuncs(Lua_State, Particle_Functions, 0);
    lua_setglobal(Lua_State, "particle");
}

void Lua_closeLibrary()
{
    log_info("Closing Lua Library");
	if (Lua_Texture_List) {
		struct Lua_Texture *LT = Lua_Texture_List->items;

		for (int i = 0; i < Lua_Texture_List->size; i++)
			Image_Free(LT[i].TextID);

		vector_delete(Lua_Texture_List);
		Lua_Texture_List = NULL;
		Texture_NextID = 0.0;
	}
	log_info("Free'd all textures");
	if (Lua_Button_List) {
		struct Lua_Button *lb = Lua_Button_List->items;

		for (int i = 0; i < Lua_Button_List->size; i++)
			Gui_Button_Free(&(lb + i)->B);

		vector_delete(Lua_Button_List);
		Lua_Button_List = NULL;
		Gui_Button_NextID = 0.0;
	}
	log_info("Free'd all buttons");
	if (Lua_Animation_List) {
        struct Lua_Animation *la = Lua_Animation_List->items;

        for(int i = 0; i < Lua_Animation_List->size; i++)
            Animation_Free(la[i].Animation);

        vector_delete(Lua_Animation_List);
        Lua_Animation_List = NULL;
        Animation_NextID = 0.0;
	}
	log_info("Free'd all animations");

	if(Lua_ParcticleS_List)
    {
        vector_delete(Lua_ParcticleS_List);
        Particle_System_Free(Lua_ParticleSystem);
        Lua_ParticleSystem = NULL;
        Lua_ParcticleS_List = NULL;
    }
    log_info("Free'd all particle systems");

	log_info("Done closing Lua Library");
}

void LuaLibrary_Render()
{
    if(Lua_ParticleSystem)
    {
        Particle_System_Render(Lua_ParticleSystem);

        struct Lua_ParcticleS *PS = Lua_ParcticleS_List->items;
        int j = 0;
        while(j < Lua_ParcticleS_List->size)
        {
            //printf("%i:%s %s\n",j, PS[j].DeleteOnFinish ? "true":"false", PS[j].Emitter->Finished ? "true":"false");
            struct Particle_Emitter *PA = Particle_getEmitter(Lua_ParticleSystem, PS[j].ID);
            if(PS[j].DeleteOnFinish && PA->Finished)
            {
                int IDX2 = -1;
                for(int i = 0; i < Lua_ParticleSystem->Emitters_Count; i++)
                {
                    if(Lua_ParticleSystem->Emitters[i].Gravity == PA->Gravity && Lua_ParticleSystem->Emitters[i].Gravity_Center.x == PA->Gravity_Center.x && Lua_ParticleSystem->Emitters[i].Gravity_Center.y == PA->Gravity_Center.y)
                    {
                        IDX2 = i;
                        break;
                    }
                }

                Particle_Emitter_Free(Lua_ParticleSystem, IDX2);

                vector_erase(Lua_ParcticleS_List, j);
                if(!Lua_ParcticleS_List->size)
                {
                    vector_delete(Lua_ParcticleS_List);
                    Particle_System_Free(Lua_ParticleSystem);
                    Lua_ParticleSystem = NULL;
                    Lua_ParcticleS_List = NULL;
                    break;
                } else
                    PS = Lua_ParcticleS_List->items;
                j--;
            }
            j++;
        }
    }

}

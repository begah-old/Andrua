#include "LuaLibrary.h"

// Particle Systems Management
struct Lua_ParcticleS { int ID; _Bool DeleteOnFinish; };
struct Particle_System *Lua_ParticleSystem = NULL;
struct vector_t *Lua_ParcticleS_List = NULL;

static int Lua_particleSnew(lua_State *L)
{
    if(!lua_isnumber(Lua_State, -3))
    {
        fprintf(LuaLibrary_Log, "particles.new : No number for argument 1");
        Lua_requestClose = true;
        lua_pushnumber(Lua_State, -1);
        return 1;
    } else if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(LuaLibrary_Log, "particles.new : No number for argument 2");
        Lua_requestClose = true;
        lua_pushnumber(Lua_State, -1);
        return 1;
    } else if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(LuaLibrary_Log, "particles.new : No number for argument 3");
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
        fprintf(LuaLibrary_Log, "particle.toggleEmitting needs ID as argument");
        Lua_requestClose = true;
        return 0;
    }

    int ID = lua_tonumber(Lua_State, -1);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(LuaLibrary_Log, "particle.toggleEmitting : argument 1 is an invalid ID");
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
        fprintf(LuaLibrary_Log, "particle.toggleEmitting : cannot find particle emitter with that id");
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
        fprintf(LuaLibrary_Log, "particle.emit needs ID as argument");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(LuaLibrary_Log, "particle.emit needs number as argument 2");
        Lua_requestClose = true;
        return 0;
    }

    int ID = lua_tonumber(Lua_State, -2);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(LuaLibrary_Log, "particle.emit : argument 1 is an invalid ID");
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
        fprintf(LuaLibrary_Log, "particle.emit : cannot find particle emitter with that id");
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
        fprintf(LuaLibrary_Log, "particle.deleteOnFinish needs ID as argument");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isboolean(Lua_State, -1))
    {
        fprintf(LuaLibrary_Log, "particle.emit needs boolean as argument 2");
        Lua_requestClose = true;
        return 0;
    }

    int ID = lua_tonumber(Lua_State, -2);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(LuaLibrary_Log, "particle.emit : argument 1 is an invalid ID");
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
        fprintf(LuaLibrary_Log, "particle.emit : cannot find particle emitter with that id");
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
        fprintf(LuaLibrary_Log, "particle.setColor : argument 1 (ID) needs to be a number");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -8))
    {
        fprintf(LuaLibrary_Log, "particle.setColor : argument 2 needs to be a number");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -7))
    {
        fprintf(LuaLibrary_Log, "particle.setColor : argument 3 needs to be a number");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -6))
    {
        fprintf(LuaLibrary_Log, "particle.setColor : argument 4 needs to be a number");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -5))
    {
        fprintf(LuaLibrary_Log, "particle.setColor : argument 5 needs to be a number");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -4))
    {
        fprintf(LuaLibrary_Log, "particle.setColor : argument 6 needs to be a number");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -3))
    {
        fprintf(LuaLibrary_Log, "particle.setColor : argument 7 needs to be a number");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(LuaLibrary_Log, "particle.setColor : argument 8 needs to be a number");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(LuaLibrary_Log, "particle.setColor : argument 9 needs to be a number");
        Lua_requestClose = true;
        return 0;
    }

    int ID = lua_tonumber(Lua_State, -9);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(LuaLibrary_Log, "particle.setColor : argument 1 is an invalid ID");
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
        fprintf(LuaLibrary_Log, "particle.setColor : cannot find particle emitter with that id");
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
        fprintf(LuaLibrary_Log, "particle.gravityType needs ID as argument");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isstring(Lua_State, -1))
    {
        fprintf(LuaLibrary_Log, "particle.gravityType needs String as argument 2");
        Lua_requestClose = true;
        return 0;
    }

    int ID = lua_tonumber(Lua_State, -2);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(LuaLibrary_Log, "particle.setColor : argument 1 is an invalid ID");
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
        fprintf(LuaLibrary_Log, "particle.setColor : cannot find particle emitter with that id");
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
        fprintf(LuaLibrary_Log, "particle.setColor : cannot find type math with argument 2");
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
        fprintf(LuaLibrary_Log, "particle.maxParticles needs ID as argument");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(LuaLibrary_Log, "particle.maxParticles needs number as argument 2");
        Lua_requestClose = true;
        return 0;
    }

    int ID = lua_tonumber(Lua_State, -2);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(LuaLibrary_Log, "particle.maxParticles : argument 1 is an invalid ID");
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
        fprintf(LuaLibrary_Log, "particle.maxParticles : cannot find particle emitter with that id");
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
        fprintf(LuaLibrary_Log, "particle.setLife needs ID as argument 1");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(LuaLibrary_Log, "particle.setLife needs number as argument 2");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(LuaLibrary_Log, "particle.setLife needs number as argument 3");
        Lua_requestClose = true;
        return 0;
    }

    int ID = lua_tonumber(Lua_State, -3);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(LuaLibrary_Log, "particle.setLife : argument 1 is an invalid ID");
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
        fprintf(LuaLibrary_Log, "particle.setLife : cannot find particle emitter with that id");
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
        fprintf(LuaLibrary_Log, "particle.setSpeed needs ID as argument 1");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(LuaLibrary_Log, "particle.setSpeed needs number as argument 2");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(LuaLibrary_Log, "particle.setSpeed needs number as argument 3");
        Lua_requestClose = true;
        return 0;
    }

    int ID = lua_tonumber(Lua_State, -3);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(LuaLibrary_Log, "particle.setSpeed : argument 1 is an invalid ID");
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
        fprintf(LuaLibrary_Log, "particle.setSpeed : cannot find particle emitter with that id");
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
        fprintf(LuaLibrary_Log, "particle.setGravity needs ID as argument");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(LuaLibrary_Log, "particle.setGravity needs number as argument 2");
        Lua_requestClose = true;
        return 0;
    }

    int ID = lua_tonumber(Lua_State, -2);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(LuaLibrary_Log, "particle.setGravity : argument 1 is an invalid ID");
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
        fprintf(LuaLibrary_Log, "particle.setGravity : cannot find particle emitter with that id");
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
        fprintf(LuaLibrary_Log, "particle.setPosition needs ID as argument 1");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(LuaLibrary_Log, "particle.setPosition needs number as argument 2");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(LuaLibrary_Log, "particle.setPosition needs number as argument 3");
        Lua_requestClose = true;
        return 0;
    }

    int ID = lua_tonumber(Lua_State, -3);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(LuaLibrary_Log, "particle.setPosition : argument 1 is an invalid ID");
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
        fprintf(LuaLibrary_Log, "particle.setPosition : cannot find particle emitter with that id");
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
        fprintf(LuaLibrary_Log, "particle.gravityCenter needs ID as argument 1");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -2))
    {
        fprintf(LuaLibrary_Log, "particle.gravityCenter needs number as argument 2");
        Lua_requestClose = true;
        return 0;
    } else if(!lua_isnumber(Lua_State, -1))
    {
        fprintf(LuaLibrary_Log, "particle.gravityCenter needs number as argument 3");
        Lua_requestClose = true;
        return 0;
    }

    int ID = lua_tonumber(Lua_State, -3);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(LuaLibrary_Log, "particle.gravityCenter : argument 1 is an invalid ID");
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
        fprintf(LuaLibrary_Log, "particle.gravityCenter : cannot find particle emitter with that id");
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
        fprintf(LuaLibrary_Log, "particle.getCount needs ID as argument");
        Lua_requestClose = true;
        lua_pushnumber(Lua_State, 0);
        return 1;
    }

    int ID = lua_tonumber(Lua_State, -1);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(LuaLibrary_Log, "particle.getCount : argument 1 is an invalid ID");
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
        fprintf(LuaLibrary_Log, "particle.getCount : cannot find particle emitter with that id");
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
        fprintf(LuaLibrary_Log, "particle.free needs ID as argument");
        Lua_requestClose = true;
        return 0;
    }

    int ID = lua_tonumber(Lua_State, -1);

    if(ID < 0 || ID >= Lua_ParticleSystem->ID_Count)
    {
        fprintf(LuaLibrary_Log, "particle.free : argument is an invalid ID");
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
        fprintf(LuaLibrary_Log, "particle.free : cannot find particle emitter with that id");
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

void LuaLibrary_Particles_Load()
{
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
    luaL_setfuncs(Lua_State, Particle_Functions, 0);
    lua_setglobal(Lua_State, "particle");
}

void LuaLibrary_Particles_Render()
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

void LuaLibrary_Particles_Close()
{
    if(Lua_ParcticleS_List)
    {
        vector_delete(Lua_ParcticleS_List);
        Particle_System_Free(Lua_ParticleSystem);
        Lua_ParticleSystem = NULL;
        Lua_ParcticleS_List = NULL;
    }
    log_info("Free'd all particle systems");
}

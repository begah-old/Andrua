#include "Util.h"

struct Particle_System *Particle_System_New()
{
    struct Particle_System *System = malloc(sizeof(struct Particle_System));

    System->Emitters = NULL;
    System->Emitters_Count = 0;
    System->ID_Count = 0;

    System->Circle = Image_Load("circle.png");

    return System;
};

static float RandoM(float Min, float Max)
{
    float d = (rand()/ (float)RAND_MAX) - 0.5f;
    d *= (Max-Min);
    if(d < 0)
        return d - Min;
    else
        return d + Min;
}

static void Particle_Set(struct Particle_Emitter *Emitter, int IDX)
{
    if(!Emitter->Continue_Emitting)
    {
        Emitter->Particles[IDX].life = 0;
        return;
    }

    Emitter->Particles[IDX].Location = Vector2_Create((rand()/ (float)RAND_MAX) * Emitter->Spawn_Rectangle.z + Emitter->Spawn_Rectangle.x,
                                                      (rand()/ (float)RAND_MAX) * Emitter->Spawn_Rectangle.w + Emitter->Spawn_Rectangle.y);

    Emitter->Particles[IDX].width = 10;
    Emitter->Particles[IDX].height = 10;

    Emitter->Particles[IDX].Velocity =  Vector2_Create(RandoM(Emitter->Min_Speed, Emitter->Max_Speed), RandoM(Emitter->Min_Speed, Emitter->Max_Speed));

    Emitter->Particles[IDX].Acceleration = Vector2_Create(0, 0);

    Emitter->Particles[IDX].life = Emitter->Min_Life + (Emitter->Max_Life - Emitter->Min_Life) * (rand() / (float)RAND_MAX);
}

static void Particle_New(struct Particle_Emitter *Emitter)
{
    if(Emitter->Particle_Count >= Emitter->Particle_Max) return;

    Particle_Set(Emitter, Emitter->Particle_Count);

    Emitter->Particle_Count++;
}

int Particle_Emitter_New(struct Particle_System *System, float oX, float oY, long int Max_Particle)
{
    System->Emitters = realloc(System->Emitters, sizeof(struct Particle_Emitter) * (System->Emitters_Count + 1));

    System->Emitters[System->Emitters_Count].z = 0;

    System->Emitters[System->Emitters_Count].Particle_Max = Max_Particle;

    System->Emitters[System->Emitters_Count].Particle_Count = 0;
    System->Emitters[System->Emitters_Count].Particles = malloc(sizeof(struct Particle) * System->Emitters[System->Emitters_Count].Particle_Max);

    System->Emitters[System->Emitters_Count].Min_Life = 3000;
    System->Emitters[System->Emitters_Count].Max_Life = 5000;

    System->Emitters[System->Emitters_Count].Color_Start = COLOR_BLUE;
    System->Emitters[System->Emitters_Count].Color_End = COLOR_RED;

    System->Emitters[System->Emitters_Count].Gravity = Vector2_Create(0.0f, -1.0f);
    System->Emitters[System->Emitters_Count].Gravity_Type = GRAVITY_TYPE_NORMAL;
    System->Emitters[System->Emitters_Count].Gravity_Center = Vector2_Create(0.0f, 0.0f);

    System->Emitters[System->Emitters_Count].Min_Speed = 0.01f;
    System->Emitters[System->Emitters_Count].Max_Speed = 2.0f;

    System->Emitters[System->Emitters_Count].Gravity_MaxDistance = 1000.0f;

    System->Emitters[System->Emitters_Count].Spawn_Rectangle = Vector4_Create(oX, oY, 0, 0);

    System->Emitters[System->Emitters_Count].Continue_Emitting = true;
    System->Emitters[System->Emitters_Count].Finished = false;

    System->Emitters[System->Emitters_Count].ID = System->ID_Count++;

    System->Emitters_Count++;

    return System->Emitters[System->Emitters_Count - 1].ID;
}

static void Particle_UpdateGravity_Simple(struct Particle_Emitter *Emitter, struct Particle *P)
{
    if(Emitter->Gravity_Type == GRAVITY_TYPE_NORMAL)
    {
        P->Acceleration = Vector2_Create(Emitter->Gravity.x, Emitter->Gravity.y);
    } else {
        float distX = Emitter->Gravity_Center.x - P->Location.x;
        float distY = Emitter->Gravity_Center.y - P->Location.y;

        if(distX > Emitter->Gravity_MaxDistance || distY > Emitter->Gravity_MaxDistance)
            return;
        if(distX >= 0 && distX < 10.0f) distX = 10.0f;
        if(distY >= 0 && distY < 10.0f) distY = 10.0f;
        if(distX < 0 && distX > -10.0f) distX = -10.0f;
        if(distY < 0 && distY > -10.0f) distY = -10.0f;

        struct Vector2f Norm = Vector2_Normalize(Vector2_Create(distX, distY));

        P->Acceleration = Vector2_Create(Norm.x * (1.0f - distX / Emitter->Gravity_MaxDistance) * Emitter->Gravity.x,
                                         Norm.y * (1.0f - distY / Emitter->Gravity_MaxDistance) * Emitter->Gravity.y);
    }
}

void Particle_EmitParticles(struct Particle_Emitter *Emitter, int Particles)
{
    if(!Emitter || Emitter->Particle_Count == Emitter->Particle_Max || !Emitter->Continue_Emitting)
        return;

    if(Emitter->Particle_Count + Particles > Emitter->Particle_Max) Particles = Emitter->Particle_Max - Emitter->Particle_Count;

    for(int i = 0; i < Particles; i++)
        Particle_New(Emitter);
}

static void Particle_Emitter_Render(struct Particle_System *System, struct Particle_Emitter *Emitter)
{
    if(Emitter->Continue_Emitting && Emitter->Particle_Count != Emitter->Particle_Max)
        Particle_New(Emitter);

    _Bool Finished = true;
    for(int i = 0; i < Emitter->Particle_Count; i++)
    {
        if(!Emitter->Particles[i].life) continue;
        Finished = false;
        Particle_UpdateGravity_Simple(Emitter, Emitter->Particles + i);

        Emitter->Particles[i].Velocity.x += Emitter->Particles[i].Acceleration.x;
        Emitter->Particles[i].Velocity.y += Emitter->Particles[i].Acceleration.y;

        if(Emitter->Particles[i].Velocity.x > Emitter->Max_Speed) Emitter->Particles[i].Velocity.x = Emitter->Max_Speed;
        else if(Emitter->Particles[i].Velocity.x < -Emitter->Max_Speed) Emitter->Particles[i].Velocity.x = -Emitter->Max_Speed;

        if(Emitter->Particles[i].Velocity.y > Emitter->Max_Speed) Emitter->Particles[i].Velocity.y = Emitter->Max_Speed;
        else if(Emitter->Particles[i].Velocity.y < -Emitter->Max_Speed) Emitter->Particles[i].Velocity.y = -Emitter->Max_Speed;

        Emitter->Particles[i].Location.x += Emitter->Particles[i].Velocity.x;
        Emitter->Particles[i].Location.y += Emitter->Particles[i].Velocity.y;

        Emitter->Particles[i].life -= Frame_Time_Passed;

        if(Emitter->Particles[i].life <= 0)
            Particle_Set(Emitter, i);

        //printf("%i %f %f\n", i, Emitter->Particles[i].x, Emitter->Particles[i].y);
        //printf("%f %f %f %f %f : %i %i %i\n", Emitter->x, Emitter->y, Emitter->Gravity_Center.x, Emitter->Gravity_Center.y, Emitter->Gravity, Emitter->Min_Life, Emitter->Max_Life, Emitter->Gravity_Type);

        float x = Emitter->Particles[i].Location.x - Emitter->Particles[i].width / 2.0f, y = Emitter->Particles[i].Location.y - Emitter->Particles[i].height / 2.0f,
            x2 = x + Emitter->Particles[i].width, y2 = y + Emitter->Particles[i].height;

        float ratio = Emitter->Particles[i].life / (float)Emitter->Max_Life, ratio2 = 1 - ratio;

        Image_Shader.pushQuad(Quad_Create(x,y,x,y2,x2,y2,x2,y), Quad_Create(System->Circle->x, System->Circle->y2, System->Circle->x, System->Circle->y, System->Circle->x2, System->Circle->y, System->Circle->x2, System->Circle->y2),
                              System->Circle->Image, Vector4_Create(ratio * Emitter->Color_Start.x + ratio2 * Emitter->Color_End.x, ratio * Emitter->Color_Start.y + ratio2 * Emitter->Color_End.y,
                                                                                 ratio * Emitter->Color_Start.z + ratio2 * Emitter->Color_End.z,(ratio * Emitter->Color_Start.w + ratio2 * Emitter->Color_End.w) - 1.0f), Emitter->z);
    }
    Emitter->Finished = Finished;
}

void Particle_System_Render(struct Particle_System *System)
{
    for(int i = 0; i < System->Emitters_Count; i++)
    {
        Particle_Emitter_Render(System, System->Emitters + i);
    }
}

void Particle_Emitter_Free(struct Particle_System *System, int IDX)
{
    if(IDX < 0 || IDX >= System->Emitters_Count)
        return;

    free(System->Emitters[IDX].Particles);
    if(IDX + 1 != System->Emitters_Count)
        memmove(System->Emitters + IDX, System->Emitters + IDX + 1, sizeof(struct Particle_Emitter) * (System->Emitters_Count - IDX));
    else
        System->ID_Count--;
    System->Emitters_Count--;
    if(!System->Emitters_Count)
    {
        free(System->Emitters);
        System->Emitters = NULL;
    }
    else
        System->Emitters = realloc(System->Emitters, sizeof(struct Particle_Emitter) * (System->Emitters_Count));
}

void Particle_System_Free(struct Particle_System *System)
{
    for(int i = 0; i < System->Emitters_Count; i++)
        free(System->Emitters[i].Particles);
    if(System->Emitters)
        free(System->Emitters);

    Image_Free(System->Circle);
    free(System);
}

struct Particle_Emitter *Particle_getEmitter(struct Particle_System *System, int ID)
{
    for(int i = 0; i < System->ID_Count; i++)
        if(System->Emitters[i].ID == ID)
            return System->Emitters + i;
    return NULL;
};

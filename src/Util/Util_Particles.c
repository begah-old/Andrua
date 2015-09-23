#include "Util.h"

struct Particle_System *Particle_System_New()
{
    struct Particle_System *System = malloc(sizeof(struct Particle_System));

    System->Emitters = NULL;
    System->Emitters_Count = 0;

    return System;
};

static void Particle_Set(struct Particle_Emitter *Emitter, int IDX)
{
    if(IDX >= Emitter->Particle_Max) return;

    Emitter->Particles[IDX].x = Emitter->x;
    Emitter->Particles[IDX].y = Emitter->y;

    Emitter->Particles[IDX].width = 10;
    Emitter->Particles[IDX].height = 10;

    Emitter->Particles[IDX].angle = 0;

    Emitter->Particles[IDX].xspeed = rand()/ (float)RAND_MAX * 2 - 1;
    Emitter->Particles[IDX].yspeed = rand() / (float)RAND_MAX * 2 - 1;

    Emitter->Particles[IDX].xacceleration = Emitter->Particles[IDX].yacceleration = 0;

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

    System->Emitters[System->Emitters_Count].x = oX;
    System->Emitters[System->Emitters_Count].y = oY;

    System->Emitters[System->Emitters_Count].Particle_Max = Max_Particle;

    System->Emitters[System->Emitters_Count].Particle_Count = 0;
    System->Emitters[System->Emitters_Count].Particles = malloc(sizeof(struct Particle) * System->Emitters[System->Emitters_Count].Particle_Max);

    System->Emitters[System->Emitters_Count].Min_Life = 3000;
    System->Emitters[System->Emitters_Count].Max_Life = 5000;

    System->Emitters[System->Emitters_Count].Color_Start = COLOR_BLUE;
    System->Emitters[System->Emitters_Count].Color_End = COLOR_RED;

    System->Emitters[System->Emitters_Count].Gravity = 0.0f;
    System->Emitters[System->Emitters_Count].Gravity_Type = GRAVITY_TYPE_DOWN;
    System->Emitters[System->Emitters_Count].Gravity_Center = Vector2_Create(0.0f, 0.0f);

    System->Emitters_Count++;

    return System->Emitters_Count - 1;
}

static void Particle_UpdateGravity_Simple(struct Particle_Emitter *Emitter, struct Particle *P)
{
    if(Emitter->Gravity_Type == GRAVITY_TYPE_DOWN)
        P->yacceleration -= Emitter->Gravity;
    else if(Emitter->Gravity_Type == GRAVITY_TYPE_UP)
        P->yacceleration += Emitter->Gravity;

    else if(Emitter->Gravity_Type == GRAVITY_TYPE_LEFT)
        P->xacceleration -= Emitter->Gravity;
    else if(Emitter->Gravity_Type == GRAVITY_TYPE_RIGHT)
        P->xacceleration += Emitter->Gravity;

    else {
        double x = P->x + (P->width / 2.0), y = P->y + (P->height / 2.0);
        if(x < Emitter->Gravity_Center.x)
            P->xacceleration = Emitter->Gravity;
        else
            P->xacceleration = -Emitter->Gravity;

        if(y < Emitter->Gravity_Center.y)
            P->yacceleration = Emitter->Gravity;
        else
            P->yacceleration = -Emitter->Gravity;
    }
}

static void Particle_Emitter_Render(struct Particle_Emitter *Emitter)
{
    if(Emitter->Particle_Count != Emitter->Particle_Max)
        Particle_New(Emitter);

    for(int i = 0; i < Emitter->Particle_Count; i++)
    {
        Particle_UpdateGravity_Simple(Emitter, Emitter->Particles + i);

        if(Emitter->Particles[i].xacceleration > 0.5) Emitter->Particles[i].xacceleration = 0.5;
        else if(Emitter->Particles[i].xacceleration < -0.5) Emitter->Particles[i].xacceleration = -0.5;

        if(Emitter->Particles[i].yacceleration > 0.5) Emitter->Particles[i].yacceleration = 0.5;
        else if(Emitter->Particles[i].yacceleration < -0.5) Emitter->Particles[i].yacceleration = -0.5;

        Emitter->Particles[i].xspeed += Emitter->Particles[i].xacceleration;
        Emitter->Particles[i].yspeed += Emitter->Particles[i].yacceleration;

        if(Emitter->Particles[i].xspeed > 2) Emitter->Particles[i].xspeed = 2;
        else if(Emitter->Particles[i].xspeed < -2) Emitter->Particles[i].xspeed = -2;

        if(Emitter->Particles[i].yspeed > 2) Emitter->Particles[i].yspeed = 2;
        else if(Emitter->Particles[i].yspeed < -2) Emitter->Particles[i].yspeed = -2;

        Emitter->Particles[i].x += Emitter->Particles[i].xspeed;
        Emitter->Particles[i].y += Emitter->Particles[i].yspeed;

        Emitter->Particles[i].life -= Frame_Time_Passed;

        if(Emitter->Particles[i].life <= 0)
            Particle_Set(Emitter, i);

        //printf("%i %f %f\n", i, Emitter->Particles[i].x, Emitter->Particles[i].y);
        //printf("%f %f %f %f %f : %i %i %i\n", Emitter->x, Emitter->y, Emitter->Gravity_Center.x, Emitter->Gravity_Center.y, Emitter->Gravity, Emitter->Min_Life, Emitter->Max_Life, Emitter->Gravity_Type);

        float x = Emitter->Particles[i].x - Emitter->Particles[i].width / 2.0f, y = Emitter->Particles[i].y - Emitter->Particles[i].height / 2.0f,
            x2 = x + Emitter->Particles[i].width, y2 = y + Emitter->Particles[i].height;

        float ratio = Emitter->Particles[i].life / (float)Emitter->Max_Life, ratio2 = 1 - ratio;

        Default_Shader.pushQuad(Quad_Create(x,y,x,y2,x2,y2,x2,y), Vector4_Create(ratio * Emitter->Color_Start.x + ratio2 * Emitter->Color_End.x, ratio * Emitter->Color_Start.y + ratio2 * Emitter->Color_End.y,
                                                                                 ratio * Emitter->Color_Start.z + ratio2 * Emitter->Color_End.z, ratio * Emitter->Color_Start.w + ratio2 * Emitter->Color_End.w));
    }
}

void Particle_System_Render(struct Particle_System *System)
{
    for(int i = 0; i < System->Emitters_Count; i++)
    {
        Particle_Emitter_Render(System->Emitters + i);
    }
}

void Particle_Emitter_Free(struct Particle_System *System, int IDX)
{
    if(IDX < 0 || IDX >= System->Emitters_Count)
        return;

    free(System->Emitters[IDX].Particles);
    if(IDX + 1 != System->Emitters_Count)
        memmove(System->Emitters + IDX, System->Emitters + IDX + 1, sizeof(struct Particle_Emitter) * (System->Emitters_Count - IDX));
    System->Emitters_Count--;
    if(!System->Emitters_Count)
        free(System->Emitters);
    else
        System->Emitters = realloc(System->Emitters, sizeof(struct Particle_Emitter) * (System->Emitters_Count));
}

void Particle_System_Free(struct Particle_System *System)
{
    for(int i = 0; i < System->Emitters_Count; i++)
        free(System->Emitters[i].Particles);
    if(System->Emitters)
        free(System->Emitters);
    free(System);
}

//
// Created by root on 8/14/15.
//

#include "Util.h"

/*#ifndef ANDROID
void Engine_requestKeyboard()
{
    // Nothing, on desktop keyboars is physical
}
void Engine_requestCloseKeyboard()
{
    // Nothing, on desktop keyboars is physical
}
#else*/

#define TEXTURE_WIDTH 1024.0f
#define TEXTURE_HEIGHT 64.0f

struct Keyboard_Key
{
    struct Quad quad;
    struct Vector4f primary_Data, secondary_Data;
    struct Vector4f Color, Color_Hover;
    char Primary_Page1, Secondary_Page1;
    char Primary_Page2, Secondary_Page2;
};

struct OnScreen_Keyboard_Struct
{
	struct Keyboard_Key *Keyboard_Keys;

	GLuint Keyboard_Pack;
	int Page_On;

	float X, Y, Width, Height;

	struct Keyboard_Key Key_Maj, Key_ChangePage, Key_Space, Key_Return, Key_Delete, Key_Close, Key_Tab;
	struct Keyboard_Key Cursor_Up, Cursor_Down, Cursor_Left, Cursor_Right;
	_Bool Key_Max_pressed;

	_Bool ScrollUp;
	_Bool ScrollDown;
};

#define KEY_HEIGHT ((OnScreen_Keyboard->Height /* Take a part of the screen */ ) / 4.0f)
#define FIRST_LINE_Y ((Key_Height * 1.0f + Y) - Height)
#define SECOND_LINE_Y ((Key_Height * 2.0f + Y) - Height)
#define THIRD_LINE_Y ((Key_Height * 3.0f + Y) - Height)
#define SCROLL_PER_TICK (OnScreen_Keyboard->Height / 60)

struct Keyboard_Key Keyboard_Key_Create(float x, float y, float width, float height, char Primary1,char Secondary1, char Primary2, char Secondary2)
{
    struct Keyboard_Key Key = { Quad_Create(x, y, x, y + height, x + width, y + height, x + width, y), Vector4_Create(x + width / 4.0f, y, width / 2.0f, height / 4.0f * 3.0f), Vector4_Create(x + width / 4.0f * 3.0f, y + height / 4.0f * 2.75f, width / 2.0f, height / 4.0f * 1.0f), Vector4_Create(0.9f, 0.9f, 0.9f, 1.0f), Vector4_Create(0.7f, 0.7f, 0.7f, 1.0f), Primary1, Secondary1, Primary2, Secondary2 };
    return Key;
}

void Engine_requestKeyboard(float X, float Y, float Width, float Height)
{
	if(OnScreen_Keyboard) free(OnScreen_Keyboard);

	OnScreen_Keyboard = malloc(sizeof(struct OnScreen_Keyboard_Struct));

	OnScreen_Keyboard->X = Width / 10.0f; OnScreen_Keyboard->Y = Y;
	OnScreen_Keyboard->Width = Width / 10.0f * 9.0f; OnScreen_Keyboard->Height = Height;

	OnScreen_Keyboard->Keyboard_Pack = Image_Load("Keyboard/KeyboardPack.png");

    char *Primary_L1 = "qwertyuiop", *Secondary_L1 = "1234567890";
    char *Primary_L11 = "1234567890", *Secondary_L11 = "!@#$%^&*()";

    char *Primary_L2 = "asdfghjkl", *Secondary_L2 = "+*/_{}|\".";
    char *Primary_L22 = "+-*/{}\\\"\'";

    char *Primary_L3 = "zxcvbnm", *Secondary_L3 = "<>:|\\&*";
    char *Primary_L33 = ",=<>;:`";

    OnScreen_Keyboard->Page_On = 0;

    int Length1 = String_length(Primary_L1), Length2 = String_length(Primary_L2), Length3 = String_length(Primary_L3);

    OnScreen_Keyboard->Keyboard_Keys = malloc(sizeof(struct Keyboard_Key) * (Length1 + Length2 + Length3 + 1));

    float Key_Width = (OnScreen_Keyboard->Width / (float)(Length1 + 2));
    float Key_Height = KEY_HEIGHT;

    float Track_X = Key_Width + OnScreen_Keyboard->X;

    for(int i = 0; i < Length1; i++)
    {
    	OnScreen_Keyboard->Keyboard_Keys[i] = Keyboard_Key_Create(Track_X, THIRD_LINE_Y, Key_Width, Key_Height, Primary_L1[i], Secondary_L1[i], Primary_L11[i], Secondary_L11[i]);
        Track_X += Key_Width;
    }

    int NumToFitInSides = Length2 - Length1 - 2;
    if(NumToFitInSides < 0) NumToFitInSides *= -1;
    Track_X = (NumToFitInSides / 2.0f) * Key_Width  + OnScreen_Keyboard->X;

    OnScreen_Keyboard->Cursor_Left = Keyboard_Key_Create(X, FIRST_LINE_Y, Key_Width, Key_Height, -1, -1, -1, -1);
    OnScreen_Keyboard->Cursor_Right = Keyboard_Key_Create(X + Key_Width, FIRST_LINE_Y, Key_Width, Key_Height, -1, -1, -1, -1);
    OnScreen_Keyboard->Cursor_Up = Keyboard_Key_Create(X + Key_Width * 0.5f, SECOND_LINE_Y, Key_Width, Key_Height, -1, -1, -1, -1);
    OnScreen_Keyboard->Cursor_Down = Keyboard_Key_Create(X + Key_Width * 0.5f, Y - Height, Key_Width, Key_Height, -1, -1, -1, -1);

    OnScreen_Keyboard->Key_Tab = Keyboard_Key_Create(Track_X - Key_Width, SECOND_LINE_Y, Key_Width, Key_Height, ' ', ' ', ' ', ' ');

    for(int i = 0; i < Length2; i++)
    {
    	OnScreen_Keyboard->Keyboard_Keys[Length1 + i] = Keyboard_Key_Create(Track_X, SECOND_LINE_Y, Key_Width, Key_Height, Primary_L2[i], Secondary_L2[i], Primary_L22[i], -1);
        Track_X += Key_Width;
    }

    OnScreen_Keyboard->Key_Delete = Keyboard_Key_Create(Track_X, SECOND_LINE_Y, Key_Width, Key_Height, -1, -1, -1, -1);

    NumToFitInSides = Length3 - Length1 - 2;
    if(NumToFitInSides < 0) NumToFitInSides *= -1;
    Track_X = (NumToFitInSides / 2.0f) * Key_Width  + OnScreen_Keyboard->X;

    OnScreen_Keyboard->Key_Maj = Keyboard_Key_Create(Track_X - Key_Width, Key_Height * 0.0f - Height + Y, Key_Width, Key_Height, ' ', ' ',  -5, -5);
    OnScreen_Keyboard->Key_Max_pressed = false;

    for(int i = 0; i < Length3; i++)
    {
    	OnScreen_Keyboard->Keyboard_Keys[Length1 + Length2 + i] = Keyboard_Key_Create(Track_X, FIRST_LINE_Y, Key_Width, Key_Height, Primary_L3[i], Secondary_L3[i], Primary_L33[i], -1);
        Track_X += Key_Width;
    }

    OnScreen_Keyboard->Key_Return = Keyboard_Key_Create(Track_X, Key_Height - Height * 1.0f + Y, Key_Width * 2, Key_Height, ' ', ' ', -5, -5);

    OnScreen_Keyboard->Key_ChangePage = Keyboard_Key_Create(Track_X, Key_Height * 0.0f - Height + Y, Key_Width, Key_Height, ' ', ' ', -5, -5);

    OnScreen_Keyboard->Key_Close = Keyboard_Key_Create(OnScreen_Keyboard->Key_ChangePage.quad.v3.x, Key_Height * 0.0f - Height + Y, Key_Width, Key_Height, ' ', ' ', -5, -5);

    OnScreen_Keyboard->Keyboard_Keys[Length1 + Length2 + Length3] = Keyboard_Key_Create(-100.0f, -100.0f, -100.0f, -100.0f, -5, -5, -5, -5);

    OnScreen_Keyboard->Key_Space = Keyboard_Key_Create(OnScreen_Keyboard->Key_Maj.quad.v3.x, Y - Height, OnScreen_Keyboard->Key_ChangePage.quad.v1.x - OnScreen_Keyboard->Key_Maj.quad.v3.x, Key_Height, -1, -1, -1, -1);

    OnScreen_Keyboard->ScrollDown = false;
    OnScreen_Keyboard->ScrollUp = true;
}

void AddToQuadY(struct Quad *q, float a)
{
	q->v1.y += a;
	q->v2.y += a;
	q->v3.y += a;
	q->v4.y += a;
}

void Engine_ScrollUp()
{
	int i = 0;
	while(1)
	{
		if(OnScreen_Keyboard->Keyboard_Keys[i].quad.v1.x == -100.0f) break;
		AddToQuadY(&OnScreen_Keyboard->Keyboard_Keys[i].quad, SCROLL_PER_TICK);
		OnScreen_Keyboard->Keyboard_Keys[i].primary_Data.y += SCROLL_PER_TICK;
		OnScreen_Keyboard->Keyboard_Keys[i].secondary_Data.y += SCROLL_PER_TICK;
		i++;
	}

	AddToQuadY(&OnScreen_Keyboard->Key_Maj.quad, SCROLL_PER_TICK);
	AddToQuadY(&OnScreen_Keyboard->Key_Delete.quad, SCROLL_PER_TICK);
	AddToQuadY(&OnScreen_Keyboard->Key_Return.quad, SCROLL_PER_TICK);
	AddToQuadY(&OnScreen_Keyboard->Key_Space.quad, SCROLL_PER_TICK);
	AddToQuadY(&OnScreen_Keyboard->Key_Close.quad, SCROLL_PER_TICK);
	AddToQuadY(&OnScreen_Keyboard->Key_ChangePage.quad, SCROLL_PER_TICK);
	AddToQuadY(&OnScreen_Keyboard->Key_Tab.quad, SCROLL_PER_TICK);

	AddToQuadY(&OnScreen_Keyboard->Cursor_Up.quad, SCROLL_PER_TICK);
	AddToQuadY(&OnScreen_Keyboard->Cursor_Down.quad, SCROLL_PER_TICK);
	AddToQuadY(&OnScreen_Keyboard->Cursor_Left.quad, SCROLL_PER_TICK);
	AddToQuadY(&OnScreen_Keyboard->Cursor_Right.quad, SCROLL_PER_TICK);
}

void Engine_ScrollDown()
{
	int i = 0;
	while(1)
	{
		if(OnScreen_Keyboard->Keyboard_Keys[i].quad.v1.x == -100.0f) break;
		AddToQuadY(&OnScreen_Keyboard->Keyboard_Keys[i].quad, -SCROLL_PER_TICK);
		OnScreen_Keyboard->Keyboard_Keys[i].primary_Data.y += -SCROLL_PER_TICK;
		OnScreen_Keyboard->Keyboard_Keys[i].secondary_Data.y += -SCROLL_PER_TICK;
		i++;
	}

	AddToQuadY(&OnScreen_Keyboard->Key_Maj.quad, -SCROLL_PER_TICK);
	AddToQuadY(&OnScreen_Keyboard->Key_Delete.quad, -SCROLL_PER_TICK);
	AddToQuadY(&OnScreen_Keyboard->Key_Return.quad, -SCROLL_PER_TICK);
	AddToQuadY(&OnScreen_Keyboard->Key_Space.quad, -SCROLL_PER_TICK);
	AddToQuadY(&OnScreen_Keyboard->Key_Close.quad, -SCROLL_PER_TICK);
	AddToQuadY(&OnScreen_Keyboard->Key_ChangePage.quad, -SCROLL_PER_TICK);
	AddToQuadY(&OnScreen_Keyboard->Key_Tab.quad, -SCROLL_PER_TICK);

	AddToQuadY(&OnScreen_Keyboard->Cursor_Up.quad, -SCROLL_PER_TICK);
	AddToQuadY(&OnScreen_Keyboard->Cursor_Down.quad, -SCROLL_PER_TICK);
	AddToQuadY(&OnScreen_Keyboard->Cursor_Left.quad, -SCROLL_PER_TICK);
	AddToQuadY(&OnScreen_Keyboard->Cursor_Right.quad, -SCROLL_PER_TICK);
}

void Engine_closeKeyboard()
{
	free(OnScreen_Keyboard->Keyboard_Keys);
	Image_Free(OnScreen_Keyboard->Keyboard_Pack);
	free(OnScreen_Keyboard);
	OnScreen_Keyboard = NULL;
}

void Engine_renderKeyboard()
{
	if(!OnScreen_Keyboard)
		return;

	if(OnScreen_Keyboard->ScrollDown && OnScreen_Keyboard->Keyboard_Keys[0].quad.v2.y < OnScreen_Keyboard->Y) {
		Engine_closeKeyboard();
		return;
	}

    int Track = 0;
    char TempString[] = "A";

    while(true)
    {
        if(OnScreen_Keyboard->Keyboard_Keys[Track].quad.v1.x == -100.0f)
            break;

        if(Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), OnScreen_Keyboard->Keyboard_Keys[Track].quad))
        {
            if(Mouse.isPressed)
            	Image_Shader.pushQuad(OnScreen_Keyboard->Keyboard_Keys[Track].quad, Quad_Create(455 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 455 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 503 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 503 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));
            else
                Image_Shader.pushQuad(OnScreen_Keyboard->Keyboard_Keys[Track].quad, Quad_Create(405 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 405 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 453 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 453 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));

            if(!Mouse.justSuper_longReleased)
            {
				if(((Mouse.justReleased )&( !Mouse.justLongPressedReleased)) || Mouse.isSuper_longPress)
				{
					if(OnScreen_Keyboard->Page_On == 0)
					{
						Keyboard.justTyped = true;
						Keyboard.Character = OnScreen_Keyboard->Key_Max_pressed ? OnScreen_Keyboard->Keyboard_Keys[Track].Primary_Page1 - 32 : OnScreen_Keyboard->Keyboard_Keys[Track].Primary_Page1;
					} else {
						Keyboard.justTyped = true;
						Keyboard.Character = OnScreen_Keyboard->Keyboard_Keys[Track].Primary_Page2;
					}
				} else if(Mouse.justLongPressedReleased)
				{
					if(OnScreen_Keyboard->Page_On == 0)
					{
						Keyboard.justTyped = true;
						Keyboard.Character = OnScreen_Keyboard->Keyboard_Keys[Track].Secondary_Page1;
					} else if(OnScreen_Keyboard->Keyboard_Keys[Track].Secondary_Page2 > 0) {
						Keyboard.justTyped = true;
						Keyboard.Character = OnScreen_Keyboard->Keyboard_Keys[Track].Secondary_Page2;
					}
				}
            }
        }
        else
            Image_Shader.pushQuad(OnScreen_Keyboard->Keyboard_Keys[Track].quad, Quad_Create(405 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 405 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 453 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 453 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));

        if(OnScreen_Keyboard->Page_On == 0)
        {
			if(OnScreen_Keyboard->Key_Max_pressed)
				TempString[0] = OnScreen_Keyboard->Keyboard_Keys[Track].Primary_Page1 - 32;
			else
				TempString[0] = OnScreen_Keyboard->Keyboard_Keys[Track].Primary_Page1;
        } else {
        	TempString[0] = OnScreen_Keyboard->Keyboard_Keys[Track].Primary_Page2;
        }

        if(TempString[0] == '{' || TempString[0] == '}' || TempString[0] == '|' || TempString[0] == ';' || TempString[0] == ':')
			Font_FixedRender(DefaultFontManager, TempString,
					OnScreen_Keyboard->Keyboard_Keys[Track].primary_Data.x, OnScreen_Keyboard->Keyboard_Keys[Track].primary_Data.y + OnScreen_Keyboard->Keyboard_Keys[Track].primary_Data.w / 5.0f, OnScreen_Keyboard->Keyboard_Keys[Track].primary_Data.w, OnScreen_Keyboard->Keyboard_Keys[Track].primary_Data.z, 1.0f, COLOR_RED);
        else
        	Font_FixedRender(DefaultFontManager, TempString,
        			OnScreen_Keyboard->Keyboard_Keys[Track].primary_Data.x, OnScreen_Keyboard->Keyboard_Keys[Track].primary_Data.y, OnScreen_Keyboard->Keyboard_Keys[Track].primary_Data.w, OnScreen_Keyboard->Keyboard_Keys[Track].primary_Data.z, 1.0f, COLOR_RED);

        if(OnScreen_Keyboard->Page_On == 0)
        	TempString[0] = OnScreen_Keyboard->Keyboard_Keys[Track].Secondary_Page1;
        else
        	TempString[0] = OnScreen_Keyboard->Keyboard_Keys[Track].Secondary_Page2;

        if(TempString[0] < 0)
        {
        	Track++;
        	continue;
        }

        //printf("Bouya %s\n", TempString);
        if(TempString[0] == '_' || TempString[0] == ',' || TempString[0] == '.') {
            Font_FixedRenderCA(DefaultFontManager, TempString,
            		OnScreen_Keyboard->Keyboard_Keys[Track].secondary_Data.x,
					OnScreen_Keyboard->Keyboard_Keys[Track].secondary_Data.y +
					OnScreen_Keyboard->Keyboard_Keys[Track].secondary_Data.w / 3.0f,
					OnScreen_Keyboard->Keyboard_Keys[Track].secondary_Data.w,
					OnScreen_Keyboard->Keyboard_Keys[Track].secondary_Data.z, 1.0, COLOR_RED);
        }
        else
            Font_FixedRenderCA(DefaultFontManager, TempString,
            		OnScreen_Keyboard->Keyboard_Keys[Track].secondary_Data.x, OnScreen_Keyboard->Keyboard_Keys[Track].secondary_Data.y, OnScreen_Keyboard->Keyboard_Keys[Track].secondary_Data.w, OnScreen_Keyboard->Keyboard_Keys[Track].secondary_Data.z, 1.0, COLOR_RED);

        Track++;
    }

    // Maj key
    if(OnScreen_Keyboard->Key_Max_pressed || ((Mouse.isPressed)&Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), OnScreen_Keyboard->Key_Maj.quad)))
        Image_Shader.pushQuad(OnScreen_Keyboard->Key_Maj.quad, Quad_Create(455 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 455 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 503 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 503 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));
    else
        Image_Shader.pushQuad(OnScreen_Keyboard->Key_Maj.quad, Quad_Create(405 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 405 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 453 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 453 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));

    Image_Shader.pushQuad(OnScreen_Keyboard->Key_Maj.quad, Quad_Create(605 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 605 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 653 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 653 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0.0f, 0, 0, 0.0f));

    if(Mouse.justPressed && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), OnScreen_Keyboard->Key_Maj.quad))
    	OnScreen_Keyboard->Key_Max_pressed = !OnScreen_Keyboard->Key_Max_pressed;

    // Delete Key

    if(Mouse.isPressed && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), OnScreen_Keyboard->Key_Delete.quad))
    	Image_Shader.pushQuad(OnScreen_Keyboard->Key_Delete.quad, Quad_Create(455 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 455 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 503 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 503 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));
    else
        Image_Shader.pushQuad(OnScreen_Keyboard->Key_Delete.quad, Quad_Create(405 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 405 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 453 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 453 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));

    Image_Shader.pushQuad(OnScreen_Keyboard->Key_Delete.quad, Quad_Create(305 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 305 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 353 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 353 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0.0f, 0, 0, 0.0f));

    if(Mouse.isPressed && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), OnScreen_Keyboard->Key_Delete.quad))
    {
    	if(Mouse.isLongedPressed)
    	{
    		Keyboard.justPressed = true;
    		Keyboard.Key.key = 259;
    		Keyboard.Key.scancode = 22;
    		Keyboard.Key.mods = 0;
    	}
    } else if(Mouse.justReleased && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), OnScreen_Keyboard->Key_Delete.quad))
    {
    	Keyboard.justPressed = true;
    	Keyboard.Key.key = 259;
    	Keyboard.Key.scancode = 22;
    	Keyboard.Key.mods = 0;
    }

    // Return key
    if(Mouse.isPressed && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), OnScreen_Keyboard->Key_Return.quad))
    	Image_Shader.pushQuad(OnScreen_Keyboard->Key_Return.quad, Quad_Create(455 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 455 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 503 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 503 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));
    else
    	Image_Shader.pushQuad(OnScreen_Keyboard->Key_Return.quad, Quad_Create(405 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 405 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 453 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 453 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));

    Font_FixedRender(DefaultFontManager, "Return", OnScreen_Keyboard->Key_Return.quad.v1.x + (OnScreen_Keyboard->Key_Return.quad.v2.y - OnScreen_Keyboard->Key_Return.quad.v1.y) / 2.0f * 0.5f, OnScreen_Keyboard->Key_Return.quad.v1.y, (OnScreen_Keyboard->Key_Return.quad.v2.y - OnScreen_Keyboard->Key_Return.quad.v1.y) / 2.0f * 1.75f, (OnScreen_Keyboard->Key_Return.quad.v3.x - OnScreen_Keyboard->Key_Return.quad.v1.x) / 2.0f * 1.5f, 1.0f, COLOR_RED );

    if(Mouse.justReleased && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), OnScreen_Keyboard->Key_Return.quad))
    {
    	Keyboard.justPressed = true;
    	Keyboard.Key.key = GLFW_KEY_ENTER;
    	Keyboard.Key.mods = 0;
    }

    // Change Pag key
    if(Mouse.isPressed && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), OnScreen_Keyboard->Key_ChangePage.quad))
    	Image_Shader.pushQuad(OnScreen_Keyboard->Key_ChangePage.quad, Quad_Create(455 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 455 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 503 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 503 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));
    else
    	Image_Shader.pushQuad(OnScreen_Keyboard->Key_ChangePage.quad, Quad_Create(405 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 405 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 453 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 453 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));

    Image_Shader.pushQuad(OnScreen_Keyboard->Key_ChangePage.quad, Quad_Create(505 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 505 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 553 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 553 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));

    if(Mouse.justPressed && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), OnScreen_Keyboard->Key_ChangePage.quad))
    {
    	if(OnScreen_Keyboard->Page_On == 0) OnScreen_Keyboard->Page_On = 1;
    	else OnScreen_Keyboard->Page_On = 0;
    }

    // Space key
    if(Mouse.isPressed && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), OnScreen_Keyboard->Key_Space.quad))
    	Image_Shader.pushQuad(OnScreen_Keyboard->Key_Space.quad, Quad_Create(17 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 17 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 17) / TEXTURE_HEIGHT, 133 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 17) / TEXTURE_HEIGHT, 133 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(-0.7, -0.7, -0.7, 0));
    else
    	Image_Shader.pushQuad(OnScreen_Keyboard->Key_Space.quad, Quad_Create(17 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 17 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 17) / TEXTURE_HEIGHT, 133 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 17) / TEXTURE_HEIGHT, 133 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));

    if(Mouse.justReleased && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), OnScreen_Keyboard->Key_Space.quad))
    {
    	Keyboard.justTyped = true;
    	Keyboard.Character = ' ';
    }

    // Key close
    if(Mouse.isPressed && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), OnScreen_Keyboard->Key_Close.quad))
        Image_Shader.pushQuad(OnScreen_Keyboard->Key_Close.quad, Quad_Create(455 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 455 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 503 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 503 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));
    else
        Image_Shader.pushQuad(OnScreen_Keyboard->Key_Close.quad, Quad_Create(405 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 405 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 453 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 453 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));

    Image_Shader.pushQuad(OnScreen_Keyboard->Key_Close.quad, Quad_Create(605 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 605 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 653 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 653 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0.0f, 0, 0, 0.0f));

    if(Mouse.justReleased && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), OnScreen_Keyboard->Key_Close.quad))
    {
    	OnScreen_Keyboard->ScrollDown = true;
    	OnScreen_Keyboard->ScrollUp = false;
    }

    // Key tab
    if(Mouse.isPressed && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), OnScreen_Keyboard->Key_Tab.quad))
    	Image_Shader.pushQuad(OnScreen_Keyboard->Key_Tab.quad, Quad_Create(455 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 455 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 503 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 503 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));
    else
    	Image_Shader.pushQuad(OnScreen_Keyboard->Key_Tab.quad, Quad_Create(405 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 405 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 453 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 453 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));

    Image_Shader.pushQuad(OnScreen_Keyboard->Key_Tab.quad, Quad_Create(653 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 605 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 605 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 653 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0.0f, 0, 0, 0.0f));

    if(Mouse.justReleased && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), OnScreen_Keyboard->Key_Tab.quad))
    {
    	Keyboard.justPressed = true;
    	Keyboard.Key.key = GLFW_KEY_TAB;
    	Keyboard.Key.mods = 0;
    }

    // Cursor Right
    if(Mouse.isPressed && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), OnScreen_Keyboard->Cursor_Right.quad))
    	Image_Shader.pushQuad(OnScreen_Keyboard->Cursor_Right.quad, Quad_Create(455 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 455 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 503 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 503 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));
    else
    	Image_Shader.pushQuad(OnScreen_Keyboard->Cursor_Right.quad, Quad_Create(405 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 405 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 453 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 453 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));

    Image_Shader.pushQuad(OnScreen_Keyboard->Cursor_Right.quad, Quad_Create(653 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 605 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 605 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 653 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(-1.0f, -1.0f, -1.0f, 0.0f));

    if(((Mouse.justReleased) | (Mouse.isLongedPressed)) && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), OnScreen_Keyboard->Cursor_Right.quad))
    {
    	Keyboard.justPressed = true;
    	Keyboard.Key.key = GLFW_KEY_RIGHT;
    	Keyboard.Key.mods = 0;
    }

    // Cursor Left
    if(Mouse.isPressed && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), OnScreen_Keyboard->Cursor_Left.quad))
    	Image_Shader.pushQuad(OnScreen_Keyboard->Cursor_Left.quad, Quad_Create(455 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 455 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 503 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 503 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));
    else
    	Image_Shader.pushQuad(OnScreen_Keyboard->Cursor_Left.quad, Quad_Create(405 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 405 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 453 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 453 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));

    Image_Shader.pushQuad(OnScreen_Keyboard->Cursor_Left.quad, Quad_Create(653 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 605 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 605 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 653 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(-1.0f, -1.0f, -1.0f, 0.0f));

    if(((Mouse.justReleased) | (Mouse.isLongedPressed)) && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), OnScreen_Keyboard->Cursor_Left.quad))
    {
    	Keyboard.justPressed = true;
    	Keyboard.Key.key = GLFW_KEY_LEFT;
    	Keyboard.Key.mods = 0;
    }

    // Cursor Up
    if(Mouse.isPressed && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), OnScreen_Keyboard->Cursor_Up.quad))
    	Image_Shader.pushQuad(OnScreen_Keyboard->Cursor_Up.quad, Quad_Create(455 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 455 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 503 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 503 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));
    else
    	Image_Shader.pushQuad(OnScreen_Keyboard->Cursor_Up.quad, Quad_Create(405 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 405 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 453 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 453 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));

    Image_Shader.pushQuad(OnScreen_Keyboard->Cursor_Up.quad, Quad_Create(605 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 605 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 653 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 653 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(-1.0f, -1.0f, -1.0f, 0.0f));

    if(((Mouse.justReleased) | (Mouse.isLongedPressed)) && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), OnScreen_Keyboard->Cursor_Up.quad))
    {
    	Keyboard.justPressed = true;
    	Keyboard.Key.key = GLFW_KEY_UP;
    	Keyboard.Key.mods = 0;
    }

    // Cursor Down
    if(Mouse.isPressed && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), OnScreen_Keyboard->Cursor_Down.quad))
    	Image_Shader.pushQuad(OnScreen_Keyboard->Cursor_Down.quad, Quad_Create(455 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 455 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 503 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 503 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));
    else
    	Image_Shader.pushQuad(OnScreen_Keyboard->Cursor_Down.quad, Quad_Create(405 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 405 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 453 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 453 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(0, 0, 0, 0));

    Image_Shader.pushQuad(OnScreen_Keyboard->Cursor_Down.quad, Quad_Create(605 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT, 605 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 653 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 49) / TEXTURE_HEIGHT, 653 / TEXTURE_WIDTH, (TEXTURE_HEIGHT - 1) / TEXTURE_HEIGHT), OnScreen_Keyboard->Keyboard_Pack, Vector4_Create(-1.0f, -1.0f, -1.0f, 0.0f));

    if(((Mouse.justReleased) | (Mouse.isLongedPressed)) && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), OnScreen_Keyboard->Cursor_Down.quad))
    {
    	Keyboard.justPressed = true;
    	Keyboard.Key.key = GLFW_KEY_DOWN;
    	Keyboard.Key.mods = 0;
    }

    if(OnScreen_Keyboard->Key_Space.quad.v1.y < OnScreen_Keyboard->Y && OnScreen_Keyboard->ScrollUp) Engine_ScrollUp();
    else if(OnScreen_Keyboard->Keyboard_Keys[0].quad.v2.y > OnScreen_Keyboard->Y && OnScreen_Keyboard->ScrollDown) Engine_ScrollDown();
}

void Engine_requestCloseKeyboard()
{
	if(OnScreen_Keyboard)
	{
		OnScreen_Keyboard->ScrollDown = true;
		OnScreen_Keyboard->ScrollUp = false;
	}
}

//#endif


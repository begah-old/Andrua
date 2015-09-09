/*
 * Console.c
 *
 *  Created on: Aug 29, 2015
 *      Author: mathi
 */

#include "Console.h"

void Console_Init()
{
	if(Console)
		Console_Close();

	Console = malloc(sizeof(struct Lua_Console));
	Console->Ligne_inWindow = 10;
	Console->Minimized = false;
	Console->MaxWidth_Index = -1;

	Console->Width = Game_Width;
	Console->Height = Game_Height / 20.0f * 9.0f;
	Console->Ligne_Height = Console->Height / (Console->Ligne_inWindow + 2);
	Console->Default_Ligne_Height = (480.0f / 20.0f * 9.0f) / (Console->Ligne_inWindow + 2);

	Console->Background = Quad_Create(0, Console->Ligne_Height, 0, Console->Height, Console->Width, Console->Height, Console->Width, Console->Ligne_Height);
	Console->Lignes = vector_new(sizeof(struct String_Struct));

	Console->ScrollBar = Gui_Horizontal_ScrollBar_Create(0, 0, Console->Width, Console->Ligne_Height, COLOR_WHITE, Vector4_Create(0.8f, 0.8f, 0.8f, 1.0f), Vector4_Create(0.6f, 0.6f, 0.6f, 1.0f), 640, 640, NULL);

	Console->VSrollBar = Gui_Vertical_ScrollBar_Create(Console->Width / 10.0f * 9.75f, Console->Ligne_Height, Console->Width / 40.0f, Console->Height - 2 * Console->Ligne_Height, COLOR_WHITE, Vector4_Create(0.8f, 0.8f, 0.8f, 1.0f), Vector4_Create(0.6f, 0.6f, 0.6f, 1.0f), Console->Default_Ligne_Height * Console->Ligne_inWindow, Console->Default_Ligne_Height * Console->Ligne_inWindow, NULL);

	Console->Close_Text = Image_Load("Code_Editor/Close.png");

	Console->Close = Quad_Create(Console->Width / 10.0f * 9.0f, Console->Height - Console->Ligne_Height, Console->Width / 10.0f * 9.0f, Console->Height, Console->Width, Console->Height, Console->Width, Console->Height - Console->Ligne_Height);

	Console->Minimize = Quad_Create(Console->Width / 10.0f * 8.0f, Console->Height - Console->Ligne_Height, Console->Width / 10.0f * 8.0f, Console->Height, Console->Width / 10.0f * 9.0f, Console->Height, Console->Width / 10.0f * 9.0f, Console->Height - Console->Ligne_Height);
	Console->Minimize_Render = Quad_Create(Console->Width / 10.0f * 8.25f, Console->Height - Console->Ligne_Height / 10.0f * 4.0f, Console->Width / 10.0f * 8.25f, Console->Height - Console->Ligne_Height / 10.0f * 6.0f, Console->Width / 10.0f * 8.75f, Console->Height - Console->Ligne_Height / 10.0f * 6.0f, Console->Width / 10.0f * 8.75f, Console->Height - Console->Ligne_Height / 10.0f * 4.0f);
}

int Console_Render()
{
	if(Console->Height != Game_Height / 20.0f * 9.0f || Console->Width != Game_Width)
	{
		Console->Width = Game_Width;
		Console->Height = Game_Height / 20.0f * 9.0f;
		Console->Ligne_Height = Console->Height / (Console->Ligne_inWindow + 2);

		Console->Background = Quad_Create(0, Console->Ligne_Height, 0, Console->Height, Console->Width, Console->Height, Console->Width, Console->Ligne_Height);

		if(Console->MaxWidth_Index != -1)
		{
			float WIDTH = Font_HeightLength(DefaultFontManager, ((struct String_Struct *)Console->Lignes->items)[Console->MaxWidth_Index].String, Console->Ligne_Height, 1.0f);
			if(Console->Width >= WIDTH)
			{
				Console->MaxWidth_Index = -1;
				WIDTH = Console->Width;
			}
			Gui_Horizontal_ScrollBar_Resize(Console->ScrollBar, 0, 0, Console->Width, Console->Ligne_Height, Console->Width, WIDTH);
		}
		else
			Gui_Horizontal_ScrollBar_Resize(Console->ScrollBar, 0, 0, Console->Width, Console->Ligne_Height, Console->Width, Console->Width);

		Gui_Vertical_ScrollBar_Resize(Console->VSrollBar, Console->Width / 10.0f * 9.75f, Console->Ligne_Height, Console->Width / 40.0f, Console->Height - 2 * Console->Ligne_Height, Console->VSrollBar->BarValue, Console->VSrollBar->TotalValue);

		Console->Close = Quad_Create(Console->Width / 10.0f * 9.0f, Console->Height - Console->Ligne_Height, Console->Width / 10.0f * 9.0f, Console->Height, Console->Width, Console->Height, Console->Width, Console->Height - Console->Ligne_Height);

		Console->Minimize = Quad_Create(Console->Width / 10.0f * 8.0f, Console->Height - Console->Ligne_Height, Console->Width / 10.0f * 8.0f, Console->Height, Console->Width / 10.0f * 9.0f, Console->Height, Console->Width / 10.0f * 9.0f, Console->Height - Console->Ligne_Height);
		Console->Minimize_Render = Quad_Create(Console->Width / 10.0f * 8.25f, Console->Height - Console->Ligne_Height / 10.0f * 4.0f, Console->Width / 10.0f * 8.25f, Console->Height - Console->Ligne_Height / 10.0f * 6.0f, Console->Width / 10.0f * 8.75f, Console->Height - Console->Ligne_Height / 10.0f * 6.0f, Console->Width / 10.0f * 8.75f, Console->Height - Console->Ligne_Height / 10.0f * 4.0f);
	}

	_Bool ReturnMinimized = false;

	if(Console->Minimized)
	{
		struct Quad Close2 = Quad_Create(Console->Close.v1.x, 0, Console->Close.v2.x, Console->Ligne_Height, Console->Close.v3.x, Console->Ligne_Height, Console->Close.v4.x, 0), Minimize2 = Quad_Create(Console->Minimize.v1.x, 0, Console->Minimize.v2.x, Console->Ligne_Height, Console->Minimize.v3.x, Console->Ligne_Height, Console->Minimize.v4.x, 0),
				Minimize_Render2 = Quad_Create(Console->Minimize_Render.v1.x, Console->Ligne_Height / 10.0f * 4.0f, Console->Minimize_Render.v2.x, Console->Ligne_Height / 10.0f * 6.0f, Console->Minimize_Render.v3.x, Console->Ligne_Height / 10.0f * 6.0f, Console->Minimize_Render.v4.x, Console->Ligne_Height / 10.0f * 4.0f);

		_Bool Close_Hover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), Close2);
		_Bool Minimize_Hover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), Minimize2);

		if(Close_Hover && Mouse.justReleased)
		{
			Console_Close();
			return 1;
		} else if(Minimize_Hover && Mouse.justReleased)
		{
			Console->Minimized = false;
			ReturnMinimized = true;
			goto exit_Minimized;
		}

		if(Close_Hover)
			Image_Shader.pushQuad(Close2, Quad_Create(0, 0, 0, 1, 1, 1, 1, 0), Console->Close_Text, Vector4_Create(-0.3f, -0.3f, -0.3f, 0));
		else
			Image_Shader.pushQuad(Close2, Quad_Create(0, 0, 0, 1, 1, 1, 1, 0), Console->Close_Text, Vector4_Create(0, 0, 0, 0));

		if(Minimize_Hover)
			Default_Shader.pushQuad(Minimize_Render2, Vector4_Create(0.7f, 0, 0, 1));
		else
			Default_Shader.pushQuad(Minimize_Render2, COLOR_RED);

		return 0;
	} exit_Minimized:;

	_Bool Close_Hover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), Console->Close);
	_Bool Minimize_Hover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), Console->Minimize);

	if(Close_Hover && Mouse.justReleased)
	{
		Console_Close();
		return 1;
	}

	if(Close_Hover)
		Image_Shader.pushQuad(Console->Close, Quad_Create(0, 0, 0, 1, 1, 1, 1, 0), Console->Close_Text, Vector4_Create(-0.3f, -0.3f, -0.3f, 0));
	else
		Image_Shader.pushQuad(Console->Close, Quad_Create(0, 0, 0, 1, 1, 1, 1, 0), Console->Close_Text, Vector4_Create(0, 0, 0, 0));

	if(Minimize_Hover)
		Default_Shader.pushQuad(Console->Minimize_Render, Vector4_Create(0.7f, 0, 0, 1));
	else
		Default_Shader.pushQuad(Console->Minimize_Render, COLOR_RED);

	Default_Shader.pushQuad(Quad_Create(0, Console->Ligne_Height, 0, Console->Height - Console->Ligne_Height, Console->Width, Console->Height - Console->Ligne_Height, Console->Width, 0), COLOR_WHITE);

	if(Mouse.justScrolled)
	{
		Console->VSrollBar->BarY += Mouse.scrollY * 8;
	}

	{
		/*float Default_LigneHeight = (480.0f / 20.0f * 9.0f) / (Console->Ligne_inWindow + 2);
		float yy = Default_LigneHeight + Default_LigneHeight + (Default_LigneHeight * (Console->Lignes->size - 1)) + Console->Scroll_Value;

		if(yy < (480.0f / 20.0f * 9.0f) - Default_LigneHeight)
		{
			float delta = (480.0f / 20.0f * 9.0f) - Default_LigneHeight - yy;
			Console->Scroll_Value += delta;
		}*/
	}

	Gui_Horizontal_ScrollBar_Render(Console->ScrollBar);
	Gui_Vertical_ScrollBar_Render(Console->VSrollBar);

	struct String_Struct *String = Console->Lignes->items;
	for(int i = 0; i < Console->Lignes->size; i++)
	{
		float y = Console->Ligne_Height + (Console->Ligne_Height * i) - (Console->VSrollBar->BarY / (Console->Default_Ligne_Height * Console->Ligne_inWindow) * (Console->Ligne_Height * Console->Ligne_inWindow));
		float yy = y + Console->Ligne_Height;

		/*if(i == 0 && y > Console->Ligne_Height)
		{
			Console->VSrollBar->BarY = 0;
			y = Console->Ligne_Height + (Console->Ligne_Height * i) + (Console->VSrollBar->BarY / 480.0f * Game_Height);
			yy = Console->Ligne_Height + Console->Ligne_Height + (Console->Ligne_Height * i) + (Console->VSrollBar->BarY / 480.0f * Game_Height);
		}*/

		if(yy < Console->Ligne_Height)
			continue;
		if(y > Console->Height - Console->Ligne_Height)
			continue;
		if(yy > Console->Height - Console->Ligne_Height)
			yy = Console->Height - Console->Ligne_Height;

		if (y < Console->Ligne_Height)
			y = Console->Ligne_Height;

		Font_HeightRenderRenderConstraint(DefaultFontManager, String[i].String, -Console->ScrollBar->BarX, y, yy - y, 1.0f, COLOR_BLACK, Vector2_Create(0, Console->Width));
	}

	if(Minimize_Hover && Mouse.justReleased)
	{
		Console->Minimized = true;
		return 2;
	}

	if(ReturnMinimized)
		return 3;
	return 0;
}

void Console_addLigne(char *Data)
{
	if(!Console)
		return;
	char *OData = Data;
	for(int i = 0; i < String_length(Data); i++)
		Data[i] = Character_isValid(Data[i]) ? Data[i] : '\0';

	while(*Data)
	{
		Console->MaxWidth_Index++;

		while(*Data && *Data != '\n')
			Data++;

		int TempLength = Data - OData;
		if(!TempLength)
			return;

		struct String_Struct SS;
		SS.String = malloc(sizeof(char) * (TempLength + 1));
		for(int i = 0; i < TempLength; i++)
		{
			SS.String[i] = OData[i];
		}
		SS.String[TempLength] = '\0';

		float WIDTH = Font_HeightLength(DefaultFontManager, SS.String, Console->Ligne_Height, 1.0f);
		if(Console->ScrollBar->TotalValue < WIDTH)
		{
			Console->ScrollBar->TotalValue = WIDTH;
			Console->MaxWidth_Index = 0;
		}

		float HEIGHT = Console->Default_Ligne_Height * (Console->Lignes->size + 1);
		if(Console->VSrollBar->TotalValue < HEIGHT)
			Console->VSrollBar->TotalValue = HEIGHT;

		if(Console->Lignes->size == 200)
		{
			struct String_Struct *SSS = ((struct String_Struct *)Console->Lignes->items) + 200;
			free(SSS->String);
			vector_erase(Console->Lignes, 199);
		}
		vector_insert(Console->Lignes, 0, &SS);

		if(*Data)
		{
			Data++;
			while(*Data == '\n')
			{
				Data++;

				struct String_Struct SS;
				SS.String = malloc(sizeof(char) * 1);
				SS.String[0] = '\0';
				vector_insert(Console->Lignes, 0, &SS);

				float HEIGHT = Console->Default_Ligne_Height * (Console->Lignes->size + 1);
				if(Console->VSrollBar->TotalValue < HEIGHT)
					Console->VSrollBar->TotalValue = HEIGHT;

				if(Console->Lignes->size == 200)
				{
					struct String_Struct *SSS = ((struct String_Struct *)Console->Lignes->items) + 200;
					free(SSS->String);
					vector_erase(Console->Lignes, 199);
				}
			}
		}
		OData = Data;
	}

	if(Console->MaxWidth_Index >= 200)
		Console->MaxWidth_Index = -1;
	Console->VSrollBar->BarY = 0;
}

void Console_Close()
{
	Image_Free(Console->Close_Text);

	struct String_Struct *SSS = (struct String_Struct *)Console->Lignes->items;

	for(int i = 0; i < Console->Lignes->size; i++)
		free(SSS[i].String);

	vector_delete(Console->Lignes);

	Gui_Horizontal_ScrollBar_Free(&Console->ScrollBar);
	Gui_Vertical_ScrollBar_Free(&Console->VSrollBar);

	free(Console);
	Console = NULL;
}

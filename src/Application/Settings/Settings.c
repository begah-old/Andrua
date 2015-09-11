/*
 * Settings.c
 *
 *  Created on: Sep 10, 2015
 *      Author: mathi
 */

#include "Settings.h"

/* TextBox to set a main script to be runned instead of current file */
struct Gui_TextBox *LuaScript_Executable = NULL;

/* Save current settings to file and update project structure */
struct Gui_Button *Setting_Save = NULL;

float Setting_Width = 0, Setting_Height = 0;
float Setting_X = 0, Setting_Y = 0;

struct Quad Keyboard_Show;

void Settings_Resize()
{
	Keyboard_Show = Quad_Create(Setting_X, Setting_Y, Setting_X, Setting_Y + Setting_Height / 20.0f, Setting_X + Setting_Width / 10.0f, Setting_Y + Setting_Height / 20.0f, Setting_X + Setting_Width / 10.0f, Setting_Y);

	if(!LuaScript_Executable)
	{
		LuaScript_Executable = Gui_TextBox_Create(Setting_X + Setting_Width / 10.0f * 1.5f, Setting_Y + Setting_Height / 10.0f * 5.0f,
				Setting_Width / 10.0f * 7.0f, Setting_Height / 20.0f, "main script : ", 15, 0, 0, Setting_Height / 20.0f, Setting_Width / 10.0f * 7.0f,
				0.5f, 0.5f, 0.5f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
		LuaScript_Executable->AcceptLetters = LuaScript_Executable->AcceptNumbers = LuaScript_Executable->AcceptDot = true;
		LuaScript_Executable->Quad_takeMouse = Quad_Create(Setting_X, Setting_Y + Setting_Height / 20.0f, Setting_X, Setting_Y + Setting_Height, Setting_X + Setting_Width, Setting_Y + Setting_Height, Setting_X + Setting_Width, Setting_Y + Setting_Height / 20.0f);
	} else {
		Gui_TextBox_Resize(LuaScript_Executable, Setting_X + Setting_Width / 10.0f * 1.5f, Setting_Y + Setting_Height / 10.0f * 5.0f,
				Setting_Width / 10.0f * 7.0f, Setting_Height / 20.0f);
		LuaScript_Executable->TextHeight = Setting_Height / 20.0f;
		LuaScript_Executable->TextMaxWidth = Setting_Width / 10.0f * 7.0f;
		LuaScript_Executable->Quad_takeMouse = Quad_Create(Setting_X, Setting_Y + Setting_Height / 20.0f, Setting_X, Setting_Y + Setting_Height, Setting_X + Setting_Width, Setting_Y + Setting_Height, Setting_X + Setting_Width, Setting_Y + Setting_Height / 20.0f);
	}

	if(!Setting_Save)
	{
		Setting_Save = Gui_Button_Create(Setting_X + Setting_Width / 10.0f * 9.0f, Setting_Y, Setting_Width / 10.0f, Setting_Height / 20.0f, "Save", 0, 0, Setting_Width / 10.0f, Setting_Height / 20.0f,
				0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
		Setting_Save->CenterText = true;
	} else {
		Gui_Button_Resize(Setting_Save, Setting_X + Setting_Width / 10.0f * 9.0f, Setting_Y, Setting_Width / 10.0f, Setting_Height / 20.0f);
		Setting_Save->TextHeight = Setting_Height / 20.0f;
		Setting_Save->TextWidth = Setting_Width / 10.0f;
	}
}

void Settings_Init()
{
	Setting_X = 0;
	Setting_Width = Game_Width;
	if(OnScreen_Keyboard)
	{
		Setting_Y = Game_Height / 20.0f * 9.0f;
		Setting_Height = Game_Height / 20.0f * 11.0f;
	} else if(!OnScreen_Keyboard)
	{
		Setting_Y = 0;
		Setting_Height = Game_Height;
	}

	Settings_Resize();

	if(Project.Script_toExecute)
		memcpy(LuaScript_Executable->Value, Project.Script_toExecute, sizeof(char) * (String_length(Project.Script_toExecute) + 1));
}

void Settings_Render()
{
	if(OnScreen_Keyboard && !Setting_Y)
	{
		Setting_Y = Game_Height / 20.0f * 9.0f;
		Setting_Height = Game_Height / 20.0f * 11.0f;
		Settings_Resize();
	} else if(!OnScreen_Keyboard && Setting_Y)
	{
		Setting_Y = 0;
		Setting_Height = Game_Height;
		Settings_Resize();
	}

	Gui_TextBox_Render(LuaScript_Executable);

	// Render Keyboard show
	if(Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), Keyboard_Show))
	{
		Default_Shader.pushQuad(Keyboard_Show, Vector4_Create(0.50f, 0.50f, 0.50f, 1.0f));

		if(Mouse.justReleased)
		{
			if(!OnScreen_Keyboard)Engine_requestKeyboard(0, 0, Game_Width, Game_Height / 20.0f * 9.0f);
			else Engine_requestCloseKeyboard();
		}
	} else
		Default_Shader.pushQuad(Keyboard_Show, Vector4_Create(1.0f, 1.0f, 1.0f, 1.0f));

	Font_FixedRender(DefaultFontManager, "Keyboard", Keyboard_Show.v1.x, Keyboard_Show.v1.y, Keyboard_Show.v2.y - Keyboard_Show.v1.y, Keyboard_Show.v3.x - Keyboard_Show.v1.x, 1.0f, Vector4_Create(1.0f, 0.0f, 0.0f, 1.0f));

	if(Gui_Button_Render(Setting_Save)) {
		if(Project.Script_toExecute)
			free(Project.Script_toExecute);
		if(LuaScript_Executable->Value[0])
		{
			int Length = String_length(LuaScript_Executable->Value);
			Project.Script_toExecute = malloc(sizeof(char) * (Length + 1));
			Project.Script_toExecute = memcpy(Project.Script_toExecute, LuaScript_Executable->Value, sizeof(char) * (Length + 1));
		} else
			Project.Script_toExecute = NULL;

		char *Path = FileExternal_GetFullPath("settings.le");
		FILE *f = fopen(Path, "wb");
		_Bool Boole = Project.Script_toExecute ? true : false;
		fwrite(&Boole, sizeof(_Bool), 1, f);
		if(Boole) {
			int L = String_length(Project.Script_toExecute) + 1;

			fwrite(&L, sizeof(int), 1, f);
			fwrite(Project.Script_toExecute, sizeof(char), L, f);
		}
		fclose(f);
		free(Path);
	}
}

void Settings_Free()
{
	Gui_TextBox_Free(&LuaScript_Executable);
	Gui_Button_Free(&Setting_Save);
}

/*
 * Application.c
 *
 *  Created on: Jul 15, 2015
 *      Author: begah
 */

#include "Application.h"

#define STATE_MENU 0
#define STATE_EDITOR 1
#define STATE_SETTINGS 2
#define STATE_PAINT 3

_Bool Editor_Inited = false, Paint_Inited = false, Settings_Inited = false;

/* Application State */
int STATE = STATE_MENU;

/* Project Menu GUI */
struct Gui_Button *Button_New, *Button_Open;
struct Gui_TextBox *TextBox_ProjectName;

/* Position of the "Keyboard" Button */
struct Quad Keyboard_Show;

/* Track position where mouse pressed, need to calculate if on mouse release a screen change is needed */
double Mouse_PressX = -1;

void Application_Init()
{
	Button_New = Gui_Button_Create(Game_Width / 10.0f * 1.0f, Game_Height / 10.0f * 4.0f, Game_Width / 10.0f * 3.0f, Game_Height / 20.0f, "New",
			0, 0, Game_Width / 10.0f * 3.0f, Game_Height / 20.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.2f, 0.2f, 0.2f, 1.0f);

	Button_Open = Gui_Button_Create(Game_Width / 10.0f * 6.0f, Game_Height / 10.0f * 4.0f, Game_Width / 10.0f * 3.0f, Game_Height / 20.0f, "Open",
			0, 0, Game_Width / 10.0f * 3.0f, Game_Height / 20.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.2f, 0.2f, 0.2f, 1.0f);

	Button_New->CenterText = true;
	Button_Open->CenterText = true;

	TextBox_ProjectName = Gui_TextBox_Create(Game_Width / 10.0f * 2.50f, Game_Height / 10.0f * 6.0f, Game_Width / 10.0f * 5.0f, Game_Height / 20.0f, "Project : ", 15,
			0, 0, Game_Height / 20.0f, Game_Width / 10.0f * 5.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.2f, 0.2f, 0.2f, 1.0f);
	TextBox_ProjectName->AcceptLetters = TextBox_ProjectName->AcceptNumbers = true; // The name of a project can only contain letters and numbers
}

void Application_Render()
{
	static int SHOW_ERROR = 0;
	static long SHOW_EXISTS_TIME = 0;

	if(Mouse.justPressed)
	{
		Cancel_SwitchScreen = false;
		Mouse_PressX = Mouse.x;
	}

	// Switch between different screens
	else if(Mouse.justReleased && Mouse.x - Mouse_PressX > Game_Width / 2.0f && !Cancel_SwitchScreen)
	{
		if(STATE == STATE_EDITOR)
		{
			// Move to Image Editor
			STATE = STATE_SETTINGS;
		} else if(STATE == STATE_SETTINGS)
		{
			// Move to Settings
			STATE = STATE_PAINT;
		}
	} else if(Mouse.justReleased && Mouse_PressX - Mouse.x > Game_Width / 2.0f && !Cancel_SwitchScreen)
	{
		if(STATE == STATE_PAINT)
		{
			// Move to Settings
			STATE = STATE_SETTINGS;
		} else if(STATE == STATE_SETTINGS)
		{
			// Move to Code Editor
			STATE = STATE_EDITOR;
		}
	}

	if(STATE == STATE_MENU)
	{
		if(OnScreen_Keyboard && Button_New->Y == Game_Height / 10.0f * 4.0f)
		{
			float Height = Game_Height / 20.0f * 11.0f;
			Gui_Button_Resize(Button_New, Game_Width / 10.0f * 1.0f, Height / 10.0f * 4.0f + Game_Height / 20.0f * 9.0f, Game_Width / 10.0f * 3.0f, Height / 20.0f);
			Gui_Button_Resize(Button_Open, Game_Width / 10.0f * 6.0f, Height / 10.0f * 4.0f + Game_Height / 20.0f * 9.0f, Game_Width / 10.0f * 3.0f, Height / 20.0f);
			Button_New->TextHeight = Button_Open->TextHeight = Height / 20.0f;

			Gui_TextBox_Resize(TextBox_ProjectName, Game_Width / 10.0f * 2.50f, Height / 10.0f * 6.0f + Game_Height / 20.0f * 9.0f, Game_Width / 10.0f * 5.0f, Height / 20.0f);
			TextBox_ProjectName->TextHeight = Height / 20.0f;

			TextBox_ProjectName->Quad_takeMouse = Quad_Create(0, Game_Height / 20.0f * 9.0f, 0, Game_Height, Game_Width, Game_Height, Game_Width, Game_Height / 20.0f * 9.0f);
		} else if(!OnScreen_Keyboard && Button_New->Y != Game_Height / 10.0f * 4.0f)
		{
			Gui_Button_Resize(Button_New, Game_Width / 10.0f * 1.0f, Game_Height / 10.0f * 4.0f, Game_Width / 10.0f * 3.0f, Game_Height / 20.0f);
			Gui_Button_Resize(Button_Open, Game_Width / 10.0f * 6.0f, Game_Height / 10.0f * 4.0f, Game_Width / 10.0f * 3.0f, Game_Height / 20.0f);
			Button_New->TextHeight = Button_Open->TextHeight = Game_Height / 20.0f;

			Gui_TextBox_Resize(TextBox_ProjectName, Game_Width / 10.0f * 2.50f, Game_Height / 10.0f * 6.0f, Game_Width / 10.0f * 5.0f, Game_Height / 20.0f);
			TextBox_ProjectName->TextHeight = Game_Height / 20.0f;

			TextBox_ProjectName->Quad_takeMouse = Quad_Create(0, 0, 0, Game_Height, Game_Width, Game_Height, Game_Width, 0);
		}

		/* Create new project */
		if(Gui_Button_Render(Button_New))
		{
			if(Dir_Exists(TextBox_ProjectName->Value))
			{
				/* Project already exists */
				SHOW_EXISTS_TIME = Game_FPS * 4;
				SHOW_ERROR = 1;
			} else {
				/* Create project */
				Dir_Create(TextBox_ProjectName->Value);
				Project_Name = malloc(sizeof(char) * (String_length(TextBox_ProjectName->Value)+2));
				Project_Name = memcpy(Project_Name, TextBox_ProjectName->Value, sizeof(char) * (String_length(TextBox_ProjectName->Value)+1));
#ifndef _WIN32
				Project_Name[String_length(TextBox_ProjectName->Value)] = '/';
#else
				Project_Name[String_length(TextBox_ProjectName->Value)] = '\\';
#endif
				Project_Name[String_length(TextBox_ProjectName->Value) + 1] = '\0';

				char *T = Executable_Path;
				Executable_Path = malloc(sizeof(char) * (String_length(T) + String_length(Project_Name) + 1));
				memcpy(Executable_Path, T, sizeof(char) * (String_length(T)));
				memcpy(Executable_Path + String_length(T), Project_Name, sizeof(char) * (String_length(Project_Name) + 1));
				free(T);

				/* Set state to Lua Editor */
				STATE = STATE_EDITOR;

				char *Path = FileExternal_GetFullPath("settings.le");
				FILE *f = fopen(Path, "wb");
				_Bool FFALSE = false;
				fwrite(&FFALSE, sizeof(_Bool), 1, f); // false : Not chosen a file for permanent execution
				fclose(f);
				free(Path);
				Project.Script_toExecute = NULL;
			}
		}

		/* Open existing project */
		if(Gui_Button_Render(Button_Open))
		{
			if(!Dir_Exists(TextBox_ProjectName->Value))
			{
				/* Project doesn't exists */
				SHOW_EXISTS_TIME = Game_FPS * 4;
				SHOW_ERROR = 2;
			} else {
				/* Project exists, so open it */
				Project_Name = malloc(sizeof(char) * (String_length(TextBox_ProjectName->Value)+2));
				Project_Name = memcpy(Project_Name, TextBox_ProjectName->Value, sizeof(char) * (String_length(TextBox_ProjectName->Value)+1));
#ifndef _WIN32
				Project_Name[String_length(TextBox_ProjectName->Value)] = '/';
#else
				Project_Name[String_length(TextBox_ProjectName->Value)] = '\\';
#endif
				Project_Name[String_length(TextBox_ProjectName->Value) + 1] = '\0';

				char *T = Executable_Path;
				Executable_Path = malloc(sizeof(char) * (String_length(T) + String_length(Project_Name) + 1));
				memcpy(Executable_Path, T, sizeof(char) * (String_length(T)));
				memcpy(Executable_Path + String_length(T), Project_Name, sizeof(char) * (String_length(Project_Name) + 1));
				free(T);

				STATE = STATE_EDITOR;

				char *Path = FileExternal_GetFullPath("settings.le");
				FILE *f = fopen(Path, "rb");
				if(!f) {
					f = fopen(Path, "wb");
					_Bool FFALSE = false;
					fwrite(&FFALSE, sizeof(_Bool), 1, f);
					fclose(f);
					f = fopen(Path, "rb");
				}
				_Bool FFALSE = false;
				fread(&FFALSE, sizeof(_Bool), 1, f); // false : Not chosen a file for permanent execution

				if(FFALSE)
				{
					int Length = 0;
					fread(&Length, sizeof(int), 1, f);

					Project.Script_toExecute = malloc(sizeof(char) * Length);
					fread(Project.Script_toExecute, sizeof(char), Length, f);
				} else
					Project.Script_toExecute = NULL;

				fclose(f);
				free(Path);
			}
		}

		if(SHOW_ERROR == 1)
		{
			// Show already exist project
			Font_FixedRender(DefaultFontManager, "Project already exists",
					Game_Width / 10.0f * 2.50f, Game_Height / 10.0f * 5.0f, Game_Height / 20.0f, Game_Width / 10.0f * 5.0f, 1.0f, COLOR_RED);
		} else if(SHOW_ERROR == 2)
		{
			// Show project doesn't exists
			Font_FixedRender(DefaultFontManager, "Project doesn't exists",
					Game_Width / 10.0f * 2.50f, Game_Height / 10.0f * 5.0f, Game_Height / 20.0f, Game_Width / 10.0f * 5.0f, 1.0f, COLOR_RED);
		}

		if(SHOW_ERROR)
		{
			/* Decrement Timer and set show error to false if timer ran out */
			SHOW_EXISTS_TIME--;
			if(SHOW_EXISTS_TIME <= 0)
				SHOW_ERROR = 0;
		}

		Gui_TextBox_Render(TextBox_ProjectName);

		if(STATE != STATE_MENU)
		{
			Gui_Button_Free(&Button_New);
			Gui_Button_Free(&Button_Open);
			Gui_TextBox_Free(&TextBox_ProjectName);
		}

		if(OnScreen_Keyboard)
			Keyboard_Show = Quad_Create(0, Game_Height / 20.0f * 9.0f, 0, Game_Height / 20.0f * 10.0f, Game_Width / 10.0f, Game_Height / 20.0f * 10.0f, Game_Width / 10.0f, Game_Height / 20.0f * 9.0f);
		else
			Keyboard_Show = Quad_Create(0, 0, 0, Game_Height / 20.0f * 1.0f, Game_Width / 10.0f, Game_Height / 20.0f * 1.0f, Game_Width / 10.0f, 0);

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
	}
	else if(STATE == STATE_EDITOR)
	{ // Render Code Editor
		if(!Editor_Inited)
		{
			Editor_Init();
			Editor_Inited = true;
		}
		Editor_Render();
	}
	else if(STATE == STATE_SETTINGS)
	{ // Render Project Settings
		if(!Settings_Inited)
		{
			Settings_Init();
			Settings_Inited = true;
		}
		Settings_Render();
	}
	else if(STATE == STATE_PAINT)
	{ // Render Image Editor
		if(!Paint_Inited)
		{
			ImageEditor_Init();
			Paint_Inited = true;
		}
		ImageEditor_Render();
	}
}

void Application_Free()
{
	if(Editor_Inited)
		Editor_Close();
	if(Paint_Inited)
		ImageEditor_Close();
}

/*
 * Editor.c
 *
 *  Created on: Sep 2, 2015
 *      Author: mathi
 */

#include "Editor.h"

struct Lua_Code_Editor *Code_Editor;

FILE *Log, *Input;
int Log_ReadAt = 2;

int Size_Limit = 10000;

_Bool Console_Show = false;
_Bool Console_Minimized = false;

_Bool Lua_hasInit = false, Lua_hasRender = false, Lua_hasClose = false, Lua_hasMouseInput = false;
_Bool Lua_giveMouseX = false, Lua_giveMouseY = false;

static void Console_Update()
{
	if(!Console || !Log)
		return;
	int newLength = ftell(Log);

	if(newLength > Log_ReadAt - 1)
	{
		fseek(Log, Log_ReadAt - 2, SEEK_SET);
		char m[newLength - Log_ReadAt + 2];
		fread(m, sizeof(char), newLength - Log_ReadAt + 2, Log);
		m[newLength - Log_ReadAt + 2] = '\0';

		int Last = -1;
		for (int i = 0; i < newLength - Log_ReadAt + 2; i++) {
			if (m[i] == '\n')
				Last = i;
		}
		if (Last >= 0)
			m[Last] = '\0';
		Console_addLigne(m);

		if(newLength >= Size_Limit)
		{
			fseek(Log, 0L, SEEK_SET);
			Log_ReadAt = 0L;
			newLength = 0L;
		} else {
			Log_ReadAt = newLength;
			fseek(Log, Log_ReadAt, SEEK_SET);
		}
		fwrite("\n", sizeof(char), 1, Log);
		Log_ReadAt = ftell(Log);
		fseek(Log, newLength, SEEK_SET);
	}
}

int Lua_Close(lua_State *L)
{
	if(!Lua_State)
		return 0;
	if(L)
	{
		Lua_requestClose = true;
		return 0;
	}

	log_info("Closing Lua");

	App_UsingDisplay = false;

	if(App_UseFullScreen)
	{
		App_UseFullScreen = false;
		Console_Show = true;
	}

	if(Lua_hasClose)
	{
		lua_getglobal(Lua_State, "close");
		if(lua_pcall(Lua_State, 0, 0, 0) != 0)
		{
			const char *Error = lua_tostring(Lua_State, -1);
			fprintf(Log, "Error close : %s\n", Error);
		}
	}

	Lua_closeLibrary();

    lua_gc(Lua_State, LUA_GCCOLLECT, 0);
    lua_close(Lua_State);
    Lua_State = NULL;

    log_info("Lua closed");

    Console_Update();

    fclose(Log);
    fclose(Input);
    Log = Input = NULL;

    Log_ReadAt = 2;
    log_info("Done closing Lua");
	return 0;
}

static _Bool Lua_checkFunctionExists(char *Name)
{
	lua_getglobal(Lua_State, Name);
	_Bool test = lua_isfunction(Lua_State, -1);
	lua_pop(Lua_State, -1);
	return test;
}

static _Bool Lua_checkNumberExists(char *Name)
{
	lua_getglobal(Lua_State, Name);
	_Bool test = lua_isnumber(Lua_State, -1);
	lua_pop(Lua_State, -1);
	return test;
}

static _Bool Lua_checkBooleanExistsAndTrue(char *Name)
{
	lua_getglobal(Lua_State, Name);
	_Bool test = lua_isboolean(Lua_State, -1);
	if(test)
	{
		test = lua_toboolean(Lua_State, -1);
	}
	lua_pop(Lua_State, -1);
	return test;
}

static void Lua_Init()
{
    log_info("Loading IO streams");
	char *Path = malloc(sizeof(char) * (String_length(Executable_Path) - String_length(Project_Name) + String_length("Input.txt")));
	memcpy(Path, Executable_Path, sizeof(char) * (String_length(Executable_Path) - String_length(Project_Name)));
	Path[String_length(Executable_Path) - String_length(Project_Name)] = '\0';

	String_Add(Path, "Log.txt");
	Log = fopen(Path, "w+");

	String_Remove(Path, "Log.txt");
	String_Add(Path, "Input.txt");

	Input = fopen(Path, "rb");
	if(!Input)
	{
		Input = fopen(Path, "w");
		fclose(Input);
		Input = fopen(Path, "rb");
	}

    log_info("Loading Lua");
    Lua_State = luaL_newstate(Log, Input);   /* opens Lua */

    luaL_openlibs(Lua_State);

    log_info("Loading Andrua's libs");
    Lua_LoadLibrary(Log);
    log_info("Finished Loading in Lua");

    Lua_requestClose = false;
}

static void Lua_runScript(void *Data, const char *FileName)
{
	if(Lua_State)
		Lua_Close(NULL);

    log_info("Loading Lua script");
	Lua_Init();

    log_info("Loading console");
	Console_Init();
	log_info("Console loaded");

	if(luaL_loadfile(Lua_State, FileName))
	{
		const char *Error = lua_tostring(Lua_State, -1);
		fprintf(Log, "Error loading file : %s\n", Error);
	}
	log_info("Done loading in script");

	if(lua_pcall(Lua_State, 0, 0, 0) != 0)
	{
    	const char *Error = lua_tostring(Lua_State, -1);
    	fprintf(Log, "Error on load script : %s\n", Error);
    	Console_Show = true;
    	Lua_hasClose = false;
    	Lua_Close(NULL);
    	return;
	}
	log_info("Done initializing script");

    Lua_hasInit = Lua_checkFunctionExists("init");
    Lua_hasRender = Lua_checkFunctionExists("render");
    Lua_hasClose = Lua_checkFunctionExists("close");
    Lua_hasMouseInput = Lua_checkFunctionExists("mouse_action");
    Lua_hasResize = Lua_checkFunctionExists("resize");

    Lua_giveMouseX = Lua_checkNumberExists("mouse_x");
    Lua_giveMouseY = Lua_checkNumberExists("mouse_y");

    Lua_giveWidth = Lua_checkNumberExists("Screen_Width");
    Lua_giveHeight = Lua_checkNumberExists("Screen_Height");

    App_UsingDisplay = Lua_checkBooleanExistsAndTrue("useDisplay");
    App_UseFullScreen = Lua_checkBooleanExistsAndTrue("useFullScreen");

    Lua_requestClose = false;

    if(App_UseFullScreen)
    {
    	Lua_Window = Vector4_Create(0, 0, Game_Width, Game_Height);
    	Console_Show = false;
    } else if(App_UsingDisplay)
    {
    	Lua_Window = Vector4_Create(0, Game_Height / 20.0f * 9.0f, Game_Width, Game_Height / 20.0f * 11.0f);
    }

    log_info("Calling init if script has one");
    if(Lua_hasInit)
    {
		if(Lua_giveWidth)
		{
			lua_pushnumber(Lua_State, Lua_Window.z);
			lua_setglobal(Lua_State, "Screen_Width");
		}

		if(Lua_giveHeight)
		{
			lua_pushnumber(Lua_State, Lua_Window.w);
			lua_setglobal(Lua_State, "Screen_Height");
		}

    	lua_getglobal(Lua_State, "init");

        if(lua_pcall(Lua_State, 0, 0, 0) != 0)
        {
        	const char *Error = lua_tostring(Lua_State, -1);
        	fprintf(Log, "Error init : %s\n", Error);
        	Console_Show = true;
        	Lua_Close(NULL);
        	return;
        }
    }
    log_info("Done calling init");

	if(OnScreen_Keyboard)
		Engine_closeKeyboard();

	if(!App_UseFullScreen)
	{
		Console_Show = true;
	}
	log_info("Done loading script");
}

void Editor_Init()
{
	Code_Editor = Lua_Code_Editor_Init(25, 0, 0, Game_Width, Game_Height);
	Code_Editor->Launch_Game = Lua_runScript;

	Lua_State = NULL;
}

void lua_giveMouse()
{
	if(!Lua_hasMouseInput)
		return;

	if(Mouse.justPressed)
	{
		lua_getglobal(Lua_State, "mouse_action");
		lua_pushstring(Lua_State,"pressed");
		if(lua_pcall(Lua_State, 1, 0, 0))
		{
		   	const char *Error = lua_tostring(Lua_State, -1);
		   	fprintf(Log, "Error mouse pressed : %s", Error);
		   	Lua_Close(NULL);
		   	return;
		}
	} else if(Mouse.justReleased)
	{
		lua_getglobal(Lua_State, "mouse_action");
		lua_pushstring(Lua_State,"released");
		if(lua_pcall(Lua_State, 1, 0, 0))
		{
		   	const char *Error = lua_tostring(Lua_State, -1);
		   	fprintf(Log, "Error mouse released : %s", Error);
		   	Lua_Close(NULL);
		   	return;
		}
	}
}

void Editor_Render()
{
	int Console_Message = 0;
	if (Console_Show && !OnScreen_Keyboard) {
		Console_Message = Console_Render();
		if (Console_Message == 1) {
			Console_Minimized = false;
			Console_Show = false;

			Lua_requestClose = true;
		} else if (Console_Message == 2)
			Console_Minimized = true;
		else if (Console_Message == 3)
			Console_Minimized = false;
	}

	if (Lua_State) {
		if(Lua_requestClose)
		{
			Lua_Close(NULL);
			goto Render_Code_Editor;
		}
		// Check if Lua wrote to log
		Console_Update();

		if (Keyboard.justReleased && Keyboard.Key.key == GLFW_KEY_ESCAPE) //TODO: Simulate Escape with return button on android
		{
			Lua_requestClose = true;
			goto Render_Code_Editor;
		}

		_Bool Resize = false;
		if(App_UseFullScreen)
		{
			if(OnScreen_Keyboard && !Lua_Window.y)
			{
				Lua_Window.x = 0;
				Lua_Window.y = Game_Height / 20.0f * 9.0f;
				Lua_Window.z = Game_Width;
				Lua_Window.w = Game_Height / 20.0f * 11.0f;
				Resize = true;
			} else if(!OnScreen_Keyboard && Lua_Window.y)
			{
				Lua_Window.x = 0;
				Lua_Window.y = 0;
				Lua_Window.z = Game_Width;
				Lua_Window.w = Game_Height;
				Resize = true;
			}
		}
		if(App_UsingDisplay)
		{
			if((((App_UseFullScreen)&&(OnScreen_Keyboard))|(!App_UseFullScreen)) && ((Lua_Window.z != Game_Width )|( Lua_Window.w != Game_Height / 20.0f * 11.0f)))
			{
				Lua_Window.x = 0;
				Lua_Window.y = Game_Height / 20.0f * 9.0f;
				Lua_Window.z = Game_Width;
				Lua_Window.w = Game_Height / 20.0f * 11.0f;
				Resize = true;
			} else if(App_UseFullScreen && !OnScreen_Keyboard && ((Lua_Window.z != Game_Width)|(Lua_Window.w != Game_Height)))
			{
				Lua_Window.x = 0;
				Lua_Window.y = 0;
				Lua_Window.z = Game_Width;
				Lua_Window.w = Game_Height;
				Resize = true;
			}
		}

		if (Lua_giveWidth) {
			lua_pushnumber(Lua_State, Lua_Window.z);
			lua_setglobal(Lua_State, "Screen_Width");
		}

		if (Lua_giveHeight) {
			lua_pushnumber(Lua_State, Lua_Window.w);
			lua_setglobal(Lua_State, "Screen_Height");
		}

		if(Resize && Lua_hasResize)
		{
			lua_getglobal(Lua_State, "resize");

			if (lua_pcall(Lua_State, 0, 0, 0) != 0) {
				const char *Error = lua_tostring(Lua_State, -1);
				fprintf(Log, "Error resize : %s", Error);
				Lua_requestClose = true;
				return;
			}
		}

		if (Mouse.x >= Lua_Window.x && Mouse.x <= Lua_Window.x + Lua_Window.z
				&& Lua_giveMouseX) {
			lua_pushnumber(Lua_State, Mouse.x - Lua_Window.x);
			lua_setglobal(Lua_State, "mouse_x");
		}

		if (Mouse.y >= Lua_Window.y && Mouse.y <= Lua_Window.y + Lua_Window.w
				&& Lua_giveMouseY) {
			lua_pushnumber(Lua_State, Mouse.y - Lua_Window.y);
			lua_setglobal(Lua_State, "mouse_y");
		}

		lua_giveMouse();

		if (Lua_hasRender) {
			lua_getglobal(Lua_State, "render");
			if (lua_pcall(Lua_State, 0, 0, 0) != 0) {
				const char *Error = lua_tostring(Lua_State, -1);
				fprintf(Log, "Error render : %s", Error);
				Lua_requestClose = true;
				return;
			}
		}

		LuaLibrary_Render();

		_Bool UseDisplay = App_UsingDisplay, FullScreen = App_UseFullScreen;

		if (FullScreen)
			return;

		if (UseDisplay)
			return;

		// If not using display let code_editor render
	}

	Render_Code_Editor:

	if (Code_Editor) {
		if (OnScreen_Keyboard
				&& ((Code_Editor->Width != Game_Width)
						| (Code_Editor->Height != Game_Height / 20.0f * 11.0f)
						| (Code_Editor->X != 0)
						| (Code_Editor->Y != Game_Height / 20.0f * 9.0f))) {
			Code_Editor->Width = Game_Width;
			Code_Editor->Height = Game_Height / 20.0f * 11.0f;
			Code_Editor->X = 0;
			Code_Editor->Y = Game_Height / 20.0f * 9.0f;
		} else if (Console_Show && !OnScreen_Keyboard) {
			if (!Console_Minimized) {
				Code_Editor->Width = Game_Width;
				Code_Editor->Height = Game_Height / 20.0f * 11.0f;
				Code_Editor->X = 0;
				Code_Editor->Y = Game_Height / 20.0f * 9.0f;
			} else {
			    Code_Editor->Width = Game_Width;
				Code_Editor->Height = Game_Height - Console->Ligne_Height;
				Code_Editor->X = 0;
				Code_Editor->Y = Console->Ligne_Height;
			}
		} else if (!OnScreen_Keyboard
				&& ((Code_Editor->Width != Game_Width)
						| (Code_Editor->Height != Game_Height)
						| (Code_Editor->X != 0) | (Code_Editor->Y != 0))) {
			Code_Editor->Width = Game_Width;
			Code_Editor->Height = Game_Height;
			Code_Editor->X = 0;
			Code_Editor->Y = 0;
		}

		Lua_Code_Editor_Render(Code_Editor);
	}
}

void Editor_Close()
{
	if (Code_Editor)
		Lua_Code_Editor_Free(&Code_Editor);

	if (Lua_State)
		Lua_Close(NULL);
}

#include "LuaLibrary.h"

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
		fprintf(LuaLibrary_Log, "Error creating button, argument 1 needs to be a string\n");
		Lua_requestClose = true;
		lua_pushnumber(L, -1.0);
		return 1;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(LuaLibrary_Log, "Error creating button, argument 2 needs to be a number\n");
		Lua_requestClose = true;
		lua_pushnumber(L, -1.0);
		return 1;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(LuaLibrary_Log, "Error creating button, argument 3 needs to be a number\n");
		Lua_requestClose = true;
		lua_pushnumber(L, -1.0);
		return 1;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(LuaLibrary_Log, "Error creating button, argument 4 needs to be a number\n");
		Lua_requestClose = true;
		lua_pushnumber(L, -1.0);
		return 1;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(LuaLibrary_Log, "Error creating button, argument 5 needs to be a number\n");
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
		fprintf(LuaLibrary_Log, "Error coloring button, argument 1 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -8))
	{
		fprintf(LuaLibrary_Log, "Error coloring button, argument 2 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -7))
	{
		fprintf(LuaLibrary_Log, "Error coloring button, argument 3 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -6))
	{
		fprintf(LuaLibrary_Log, "Error coloring button, argument 4 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -5))
	{
		fprintf(LuaLibrary_Log, "Error coloring button, argument 5 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(LuaLibrary_Log, "Error coloring button, argument 6 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(LuaLibrary_Log, "Error coloring button, argument 7 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(LuaLibrary_Log, "Error coloring button, argument 8 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(LuaLibrary_Log, "Error coloring button, argument 9 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}

	double ID = lua_tonumber(L, -9);

	if(ID < 0 || ID >= Gui_Button_NextID)
	{
		fprintf(LuaLibrary_Log, "Error coloring button, argument 1 needs to be a valid button");
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
		fprintf(LuaLibrary_Log, "Error coloring button, didn't find button");
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
		fprintf(LuaLibrary_Log, "Error creating button, argument 1 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(LuaLibrary_Log, "Error creating button, argument 2 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(LuaLibrary_Log, "Error creating button, argument 3 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(LuaLibrary_Log, "Error creating button, argument 4 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(LuaLibrary_Log, "Error creating button, argument 5 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}

	double ID = lua_tonumber(L, -5);

	if(ID < 0 || ID >= Gui_Button_NextID)
	{
		fprintf(LuaLibrary_Log, "Error coloring button, argument 1 needs to be a valid button");
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
		fprintf(LuaLibrary_Log, "Error coloring button, didn't find button");
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
		fprintf(LuaLibrary_Log, "Error rendering button, argument needs to be a number");
		lua_pushboolean(Lua_State, false);
		return 1;
	}

	double ID = lua_tonumber(L, -1);

	if(ID < 0 || ID >= Gui_Button_NextID)
	{
		fprintf(LuaLibrary_Log, "Error rendering button, argument needs to be a valid button");
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
		fprintf(LuaLibrary_Log, "Error rendering button, didn't find button");
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
		fprintf(LuaLibrary_Log, "Error resizing button, argument 1 needs to be a number");
		return 0;
	} 	if(!lua_isnumber(L, -4))
	{
		fprintf(LuaLibrary_Log, "Error resizing button, argument 2 needs to be a number");
		return 0;
	} 	if(!lua_isnumber(L, -3))
	{
		fprintf(LuaLibrary_Log, "Error resizing button, argument 3 needs to be a number");
		return 0;
	} 	if(!lua_isnumber(L, -2))
	{
		fprintf(LuaLibrary_Log, "Error resizing button, argument 4 needs to be a number");
		return 0;
	} 	if(!lua_isnumber(L, -0))
	{
		fprintf(LuaLibrary_Log, "Error resizing button, argument 5 needs to be a number");
		return 0;
	}

	double ID = lua_tonumber(L, -5);

	if(ID < 0 || ID >= Gui_Button_NextID)
	{
		fprintf(LuaLibrary_Log, "Error resizing button, argument needs to be a valid button");
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
		fprintf(LuaLibrary_Log, "Error resizing button, didn't find button");
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
	if(!Lua_Button_List) return 0;

	if(!lua_isnumber(L, -1))
	{
		fprintf(LuaLibrary_Log, "Error freeing button, argument needs to be a number\n");
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
			break;
		}
	}

	if(!Lua_Button_List->size)
    {
        vector_delete(Lua_Button_List);
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
		fprintf(LuaLibrary_Log, "Error creating text box, argument 1 needs to be a string\n");
		Lua_requestClose = true;
		lua_pushnumber(L, -1.0);
		return 1;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(LuaLibrary_Log, "Error creating text box, argument 2 needs to be a number\n");
		Lua_requestClose = true;
		lua_pushnumber(L, -1.0);
		return 1;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(LuaLibrary_Log, "Error creating text box, argument 3 needs to be a number\n");
		Lua_requestClose = true;
		lua_pushnumber(L, -1.0);
		return 1;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(LuaLibrary_Log, "Error creating text box, argument 4 needs to be a number\n");
		Lua_requestClose = true;
		lua_pushnumber(L, -1.0);
		return 1;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(LuaLibrary_Log, "Error creating text box, argument 5 needs to be a number\n");
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
		fprintf(LuaLibrary_Log, "Error textbox.letters in text box, argument 1 needs to be a number");
		return 0;
	} 	if(!lua_isboolean(L, -1))
	{
		fprintf(LuaLibrary_Log, "Error textbox.letters in text box, argument 2 needs to be a boolean");
		return 0;
	}

	double ID = lua_tonumber(L, -2);

	if(ID < 0 || ID >= Gui_TextBox_NextID)
	{
		fprintf(LuaLibrary_Log, "Error textbox.letters, argument needs to be a valid button");
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
		fprintf(LuaLibrary_Log, "Error textbox.letters, didn't find button");
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
		fprintf(LuaLibrary_Log, "Error textbox.numbers in text box, argument 1 needs to be a number");
		return 0;
	} 	if(!lua_isboolean(L, -1))
	{
		fprintf(LuaLibrary_Log, "Error textbox.numbers in text box, argument 2 needs to be a boolean");
		return 0;
	}

	double ID = lua_tonumber(L, -2);

	if(ID < 0 || ID >= Gui_TextBox_NextID)
	{
		fprintf(LuaLibrary_Log, "Error textbox.numbers, argument needs to be a valid button");
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
		fprintf(LuaLibrary_Log, "Error textbox.numbers, didn't find button");
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
		fprintf(LuaLibrary_Log, "Error textbox.dot in text box, argument 1 needs to be a number");
		return 0;
	} 	if(!lua_isboolean(L, -1))
	{
		fprintf(LuaLibrary_Log, "Error textbox.dot in text box, argument 2 needs to be a boolean");
		return 0;
	}

	double ID = lua_tonumber(L, -2);

	if(ID < 0 || ID >= Gui_TextBox_NextID)
	{
		fprintf(LuaLibrary_Log, "Error textbox.dot, argument needs to be a valid button");
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
		fprintf(LuaLibrary_Log, "Error textbox.dot, didn't find button");
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
		fprintf(LuaLibrary_Log, "Error textbox.symbols in text box, argument 1 needs to be a number");
		return 0;
	} 	if(!lua_isboolean(L, -1))
	{
		fprintf(LuaLibrary_Log, "Error textbox.symbols in text box, argument 2 needs to be a boolean");
		return 0;
	}

	double ID = lua_tonumber(L, -2);

	if(ID < 0 || ID >= Gui_TextBox_NextID)
	{
		fprintf(LuaLibrary_Log, "Error textbox.symbols, argument needs to be a valid button");
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
		fprintf(LuaLibrary_Log, "Error textbox.symbols, didn't find button");
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
		fprintf(LuaLibrary_Log, "Error coloring text box, argument 1 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -8))
	{
		fprintf(LuaLibrary_Log, "Error coloring text box, argument 2 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -7))
	{
		fprintf(LuaLibrary_Log, "Error coloring text box, argument 3 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -6))
	{
		fprintf(LuaLibrary_Log, "Error coloring text box, argument 4 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -5))
	{
		fprintf(LuaLibrary_Log, "Error coloring text box, argument 5 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(LuaLibrary_Log, "Error coloring text box, argument 6 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(LuaLibrary_Log, "Error coloring text box, argument 7 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(LuaLibrary_Log, "Error coloring text box, argument 8 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(LuaLibrary_Log, "Error coloring text box, argument 9 needs to be a number");
		Lua_requestClose = true;
		return 0;
	}

	double ID = lua_tonumber(L, -9);

	if(ID < 0 || ID >= Gui_TextBox_NextID)
	{
		fprintf(LuaLibrary_Log, "Error coloring text box, argument 1 needs to be a valid button");
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
		fprintf(LuaLibrary_Log, "Error coloring text box, didn't find button");
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
		fprintf(LuaLibrary_Log, "Error textbox.textColor, argument 1 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(LuaLibrary_Log, "Error textbox.textColor, argument 2 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(LuaLibrary_Log, "Error textbox.textColor, argument 3 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(LuaLibrary_Log, "Error textbox.textColor, argument 4 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(LuaLibrary_Log, "Error textbox.textColor, argument 5 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}

	double ID = lua_tonumber(L, -5);

	if(ID < 0 || ID >= Gui_TextBox_NextID)
	{
		fprintf(LuaLibrary_Log, "Error textbox.textColor, argument 1 needs to be a valid button");
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
		fprintf(LuaLibrary_Log, "Error textbox.textColor, didn't find button");
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
		fprintf(LuaLibrary_Log, "Error textbox.resize, argument 1 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -4))
	{
		fprintf(LuaLibrary_Log, "Error textbox.resize, argument 2 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -3))
	{
		fprintf(LuaLibrary_Log, "Error textbox.resize, argument 3 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -2))
	{
		fprintf(LuaLibrary_Log, "Error textbox.resize, argument 4 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}
	if(!lua_isnumber(L, -1))
	{
		fprintf(LuaLibrary_Log, "Error textbox.resize, argument 5 needs to be a number\n");
		Lua_requestClose = true;
		return 0;
	}

	double ID = lua_tonumber(L, -5);

	if(ID < 0 || ID >= Gui_TextBox_NextID)
	{
		fprintf(LuaLibrary_Log, "Error textbox.resize, argument 1 needs to be a valid button");
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
		fprintf(LuaLibrary_Log, "Error textbox.resize, didn't find button");
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
		fprintf(LuaLibrary_Log, "Error rendering text box, argument needs to be a number");
		lua_pushboolean(Lua_State, false);
		return 1;
	}

	double ID = lua_tonumber(L, -1);

	if(ID < 0 || ID >= Gui_TextBox_NextID)
	{
		fprintf(LuaLibrary_Log, "Error rendering text box, argument needs to be a valid button");
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
		fprintf(LuaLibrary_Log, "Error rendering text box, didn't find button");
		lua_pushboolean(Lua_State, false);
		return 1;
	}

	if(lb->ShowKeyboard) {
		if(Mouse.justReleased && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), Quad_Create(b->X, b->Y, b->X, b->Y + b->Height, b->X + b->Width, b->Y + b->Height, b->X + b->Width, b->Y)))
			LuaLibrary_OpenKeyboard(NULL);
		else if(Mouse.justReleased && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), b->Quad_takeMouse))
			LuaLibrary_CloseKeyboard(NULL);
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
		fprintf(LuaLibrary_Log, "Error textbox.text in text box, argument needs to be a number");
		lua_pushboolean(Lua_State, false);
		return 1;
	}

	double ID = lua_tonumber(L, -1);

	if(ID < 0 || ID >= Gui_TextBox_NextID)
	{
		fprintf(LuaLibrary_Log, "Error textbox.text, argument needs to be a valid button");
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
		fprintf(LuaLibrary_Log, "Error textbox.text, didn't find button");
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
		fprintf(LuaLibrary_Log, "Error textbox.set, argument 1 needs to be a number");
		lua_pushstring(Lua_State, "0");
		return 1;
	}
	if(!lua_isstring(L, -1))
	{
		fprintf(LuaLibrary_Log, "Error textbox.set, argument 2 needs to be a string");
		lua_pushstring(Lua_State, "0");
		return 1;
	}

	double ID = lua_tonumber(L, -2);

	if(ID < 0 || ID >= Gui_TextBox_NextID)
	{
		fprintf(LuaLibrary_Log, "Error textbox.set, argument 1 needs to be a valid button");
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
		fprintf(LuaLibrary_Log, "Error textbox.set, didn't find button");
		lua_pushstring(Lua_State, "0");
		return 1;
	}

	const char *Value = lua_tostring(L, -1);
	memcpy(b->Value, Value, sizeof(char) * String_length(Value));

	return 1;
}

static int Lua_freeTextBox(lua_State *L)
{
	if(!Lua_TextBox_List) return 0;

	if(!lua_isnumber(L, -1))
	{
		fprintf(LuaLibrary_Log, "Error freeing button, argument needs to be a number\n");
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

void LuaLibrary_Gui_Load()
{
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

    lua_newtable(Lua_State);
    luaL_setfuncs(Lua_State, Button_Functions, 0);
    lua_setglobal(Lua_State, "button");

    lua_newtable(Lua_State);
    luaL_setfuncs(Lua_State, TextBox_Functions, 0);
    lua_setglobal(Lua_State, "textbox");
}

void LuaLibrary_Gui_Render()
{

}

void LuaLibrary_Gui_Close()
{
    if (Lua_Button_List) {
		struct Lua_Button *lb = Lua_Button_List->items;

		for (int i = 0; i < Lua_Button_List->size; i++)
			Gui_Button_Free(&(lb + i)->B);

		vector_delete(Lua_Button_List);
		Lua_Button_List = NULL;
		Gui_Button_NextID = 0.0;
	}
	log_info("Free'd all buttons");
}

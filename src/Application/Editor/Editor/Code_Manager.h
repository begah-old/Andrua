/*
 * Code_Manager.h
 *
 *  Created on: Jun 30, 2015
 *      Author: begah
 */

#ifndef CODE_MANAGER_H_
#define CODE_MANAGER_H_

#include "../Editor.h"

#define CODE_MAX_LENGTH 100 // Max Length of one line

struct Lua_Code_Ligne
{
	char *Ligne;
	_Bool Static;
};

struct Int_Pointer
{
	int *Integer;
};

struct Lua_Code
{
	struct vector_t *Code;
	struct vector_t *Static_Variable;
};

struct Lua_Code_Editor
{
	short STATE;

	GLuint LignesInWindow;
	float X, Y, Width, Height;
	GLfloat Ligne_Height, Default_Ligne_Height;

	GLint XtoReset; // If the cursor goes up and down tries to stay at same height
	int Cursor_Timer;

	_Bool WritingCode;

	struct Gui_TextBox *TextBox_Name;

	struct Vector4f Hover_addColor;
	struct Vector4f Color_ReservedWords, Color_Comments, Color_Text, Color_Constants; // Constants are either numbers of false/true

	struct Image *Confirm_Text, *Save_Text, *Close_Text, *Launch_Text, *New_Text, *CopyPaste_Text, *Load_Text, *Delete_Text;
	struct Quad Confirm, Save, Close, Launch, New, Load, Delete;
	_Bool Confirm_Hover, Save_Hover, Close_Hover, Launch_Hover, New_Hover, Load_Hover, Delete_Hover;

	int Ligne_Default_Width;

	struct Quad Ligne_Num, Ligne_Text, Keyboard_Show, File_Tab;
	struct Vector4f Ligne_Num_Color, Ligne_Text_Color, File_Tab_Color;

	float File_Tab_Scroll;
	int File_Tab_Index;
	struct vector_t *File_Tab_List;
	_Bool File_Tab_canScroll, File_Tab_isScrolling;

	struct Gui_Horizontal_ScrollBar *ScrollBar;
	struct Gui_Vertical_ScrollBar *VScrollBar;

	void *Data;

	_Bool isSelecting, canBeSelected ,isShowingCopyPaste;

	void (*Launch_Game)(void *Data, const char *FileName);
	void (*Run_Script)(void *Data);

	struct String_Struct *ReserveWords;
	int ReserveWords_Size;

	struct Gui_Button *Button_Confirm, *Button_Cancel;
	struct Gui_TextBox *TextBox;
};

struct Lua_Code *Lua_Code_Init(char *Code);
struct Lua_Code *Lua_Code_OpenFile(char *Path);
struct Lua_Code *Lua_Code_Copy(struct Lua_Code *LC);
void Lua_Code_Copy_noMalloc(struct Lua_Code *LC, struct Lua_Code *LC2);
void Lua_Code_addLigne(struct Lua_Code *LC, char *Ligne, _Bool Static);
int *Lua_Code_addVariable(struct Lua_Code *LC, const char *Variables);
void Lua_Code_changeVariable(struct Lua_Code *LC, int *ID, const char *OldName, const char *NewName);
void Lua_Code_removeVariable(struct Lua_Code *LC, int *ID);
void Lua_Code_clear(struct Lua_Code *LC);
char *Lua_Code_toString(struct Lua_Code *LC);
void Lua_Code_Free(struct Lua_Code **Code);

struct Lua_Code_Editor *Lua_Code_Editor_Init(GLuint LigneInWindow, float x, float y, float width, float height);
void Lua_Code_Editor_Render(struct Lua_Code_Editor *LCE);
void Lua_Code_Editor_Free(struct Lua_Code_Editor **LCE);

#endif /* CODE_MANAGER_H_ */

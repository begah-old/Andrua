/*
 * Code_Editor.c
 *
 *  Created on: Jun 30, 2015
 *      Author: begah
 */

#include "Code_Manager.h"

#define STATE_DEFAULT 0
#define STATE_OPENFILE 1
#define STATE_NEWFILE 2
#define STATE_CONFIRMCLOSEFILE 3
#define STATE_CONFIRMDELETEFILE 4

#define FILE_TAB_MAXWIDTH (LCE->Width / 4.0f)

/* A structure define opened file you can edit */
struct File_Tab_File
{
	char *FileName_Edit, *FileName_Orig; /* Two names, so you can edit the name of the file */

	struct Lua_Code *Original, *Editing; /* Used to check if a file has been changed, it is the original's backup ( when saving, Original is overwritten ) */
	struct Vector2f Cursor_Pos; /* The position of the cursor x = character on in the line, y = line on */

	GLfloat ScrollValue; /* The vertical scroll of a file, it is file specific */
	_Bool isTextSelected; /* Selected text is remembered even when switching to different files */
	struct Vector4f Selecting_Data; /* The position of the selected text, x and z are the character on and y and w are the lines numbers */
};

static void Header_Button_Resize(struct Lua_Code_Editor *LCE)
{
	LCE->Ligne_Num = Quad_Create(LCE->X, LCE->Y + LCE->Height / 20.0f, LCE->X,
								 LCE->Y + LCE->Height / 10.0f * 9.0f, LCE->X + (LCE->Width / 10.0f),
								 LCE->Y + LCE->Height / 10.0f * 9.0f, LCE->X + (LCE->Width / 10.0f),
								 LCE->Y + LCE->Height / 20.0f); /* The space where the line numbers are displayed */
	LCE->Ligne_Text = Quad_Create(LCE->X + (LCE->Width / 10.0f),
								  LCE->Y + LCE->Height / 20.0f, LCE->X + (LCE->Width / 10.0f),
								  LCE->Y + LCE->Height / 10.0f * 9.0f, LCE->X + (LCE->Width / 20.0f * 19.0f),
								  LCE->Y + LCE->Height / 10.0f * 9.0f, LCE->X + (LCE->Width / 20.0f * 19.0f),
								  LCE->Y + LCE->Height / 20.0f); /* The space where the text of the file is displayed */

	/* File_Tab is the space where all the files are shown by name */
	LCE->File_Tab = Quad_Create(LCE->X, LCE->Y + LCE->Height - (LCE->Height / 20.0f) * 2.0f, LCE->X, LCE->Y + LCE->Height - (LCE->Height / 20.0f),
			LCE->X + LCE->Width, LCE->Y + LCE->Height - (LCE->Height / 20.0f), LCE->X + LCE->Width, LCE->Y + LCE->Height - (LCE->Height / 20.0f) * 2.0f);

	/* Confirm closes and saves the current file */
	LCE->Confirm = Quad_Create(LCE->X,
							   LCE->File_Tab.v2.y, LCE->X,
							   LCE->Y + LCE->Height, LCE->X + (LCE->Width / 10.0f),
							   LCE->Y + LCE->Height, LCE->X + (LCE->Width / 10.0f),
							   LCE->File_Tab.v2.y);

	LCE->New = Quad_Create(LCE->X + (LCE->Width / 10.0f),
							LCE->File_Tab.v2.y,
						   LCE->X + (LCE->Width / 10.0f), LCE->Y + LCE->Height,
						   LCE->X + (LCE->Width / 10.0f) * 1.75f, LCE->Y + LCE->Height,
						   LCE->X + (LCE->Width / 10.0f) * 1.75f,
						   LCE->File_Tab.v2.y);

	LCE->Load = Quad_Create(LCE->X + (LCE->Width / 10.0f) * 2.5f,
							LCE->File_Tab.v2.y,
							LCE->X + (LCE->Width / 10.0f) * 2.5f, LCE->Y + LCE->Height,
							LCE->X + (LCE->Width / 10.0f) * 3.50f, LCE->Y + LCE->Height,
							LCE->X + (LCE->Width / 10.0f) * 3.50f,
							LCE->File_Tab.v2.y);

	LCE->Save = Quad_Create(LCE->X + (LCE->Width / 10.0f) * 1.75f,
							LCE->File_Tab.v2.y,
							LCE->X + (LCE->Width / 10.0f) * 1.75f, LCE->Y + LCE->Height,
							LCE->X + (LCE->Width / 10.0f) * 2.5f, LCE->Y + LCE->Height,
							LCE->X + (LCE->Width / 10.0f) * 2.5f,
							LCE->File_Tab.v2.y);

	LCE->Launch = Quad_Create(LCE->X + (LCE->Width / 10.0f) * 3.50f,
							  LCE->File_Tab.v2.y,
							  LCE->X + (LCE->Width / 10.0f) * 3.50f, LCE->Y + LCE->Height,
							  LCE->X + (LCE->Width / 10.0f) * 4.50f, LCE->Y + LCE->Height,
							  LCE->X + (LCE->Width / 10.0f) * 4.50f,
							  LCE->File_Tab.v2.y);

	LCE->Delete = Quad_Create(LCE->X + (LCE->Width / 10.0f) * 8.0f,
							 LCE->File_Tab.v2.y,
							 LCE->X + (LCE->Width / 10.0f) * 8.0f, LCE->Y + LCE->Height,
							 LCE->X + LCE->Width / 10.0f * 9.0f, LCE->Y + LCE->Height, LCE->X + LCE->Width / 10.0f * 9.0f,
							 LCE->File_Tab.v2.y);

	LCE->Close = Quad_Create(LCE->X + (LCE->Width / 10.0f) * 9,
							 LCE->File_Tab.v2.y,
							 LCE->X + (LCE->Width / 10.0f) * 9, LCE->Y + LCE->Height,
							 LCE->X + LCE->Width, LCE->Y + LCE->Height, LCE->X + LCE->Width,
							 LCE->File_Tab.v2.y);

	/* The space where the "Keyboard" button is */
	LCE->Keyboard_Show = Quad_Create(LCE->X, LCE->Y, LCE->X, LCE->Ligne_Num.v1.y, LCE->Ligne_Num.v3.x, LCE->Ligne_Num.v1.y, LCE->Ligne_Num.v3.x, LCE->Y);

	/* Height of a line in a file */
	LCE->Ligne_Height = (LCE->Ligne_Text.v2.y - LCE->Ligne_Text.v1.y)
						/ (float) LCE->LignesInWindow;

	/* Height of a line in a file when Game_Height is 480 */
	LCE->Default_Ligne_Height = ((LCE->Height / (float)Game_Height * 480.0f) / 20.0f * 17.0f) / (float) LCE->LignesInWindow;

	char *MaxStringLength = malloc(sizeof(char) * (CODE_MAX_LENGTH + 1));
	for (int i = 0; i < CODE_MAX_LENGTH; i++)
		MaxStringLength[i] = 'W';
	MaxStringLength[CODE_MAX_LENGTH] = '\0';

	/* Max width of a line for Game_Height = 480 */
	LCE->Ligne_Default_Width = Font_HeightLength(DefaultFontManager,
												 MaxStringLength, LCE->Ligne_Height, 1.0f);
	free(MaxStringLength);

	/* Horizontal scroll bar used to show different parts of a line */
	if(!LCE->ScrollBar)
		LCE->ScrollBar = Gui_Horizontal_ScrollBar_Create(
				LCE->Ligne_Text.v1.x, LCE->Y, LCE->X + (LCE->Width / 10.0f) * 9,
				LCE->Height / 20, Vector4_Create(0.0f, 0.8f, 0.8f, 1.0f),
				Vector4_Create(0.7f, 0.7f, 0.7f, 1.0f),
				Vector4_Create(0.6f, 0.6f, 0.6f, 1.0f),
				LCE->X + (LCE->Width / 10.0f) * 9, LCE->Ligne_Default_Width + (LCE->Ligne_Text.v3.x - LCE->Ligne_Text.v1.x) / 35.0f, NULL);
	else
	{
		Gui_Horizontal_ScrollBar_Resize(LCE->ScrollBar, LCE->X + (LCE->Width / 10.0f), LCE->Y, LCE->X + (LCE->Width / 10.0f) * 9,
										LCE->Height / 20, LCE->X + (LCE->Width / 20.0f) * 17.0f, LCE->Ligne_Default_Width + (LCE->Ligne_Text.v3.x - LCE->Ligne_Text.v1.x) / 35.0f);
	}

	/* Vertical scroll bar used to change the lines displayed */
	if(!LCE->VScrollBar)
	{
		LCE->VScrollBar = Gui_Vertical_ScrollBar_Create(
				LCE->Ligne_Text.v3.x, LCE->Ligne_Text.v1.y, LCE->Width - LCE->Ligne_Text.v3.x,
				LCE->Ligne_Text.v2.y - LCE->Ligne_Text.v1.y, Vector4_Create(0.0f, 0.8f, 0.8f, 1.0f),
				Vector4_Create(0.7f, 0.7f, 0.7f, 1.0f),
				Vector4_Create(0.6f, 0.6f, 0.6f, 1.0f),
				LCE->Default_Ligne_Height * LCE->LignesInWindow, LCE->Default_Ligne_Height * LCE->LignesInWindow, NULL);
		LCE->VScrollBar->Inversed = true;
	}
	else
	{
		if(LCE->File_Tab_List->size)
			Gui_Vertical_ScrollBar_Resize(LCE->VScrollBar, LCE->Ligne_Text.v3.x, LCE->Ligne_Text.v1.y, LCE->Width - LCE->Ligne_Text.v3.x,
				LCE->Ligne_Text.v2.y - LCE->Ligne_Text.v1.y, LCE->Default_Ligne_Height * LCE->LignesInWindow, LCE->Default_Ligne_Height * ((struct File_Tab_File *)LCE->File_Tab_List->items)[LCE->File_Tab_Index].Editing->Code->size);
		else
			Gui_Vertical_ScrollBar_Resize(LCE->VScrollBar, LCE->Ligne_Text.v3.x, LCE->Ligne_Text.v1.y, LCE->Width - LCE->Ligne_Text.v3.x,
				LCE->Ligne_Text.v2.y - LCE->Ligne_Text.v1.y, LCE->Default_Ligne_Height * LCE->LignesInWindow, LCE->Default_Ligne_Height * LCE->LignesInWindow);
	}

	/* These structures are only allocated when the user is opening or creating a new file */
	if(LCE->Button_Confirm)
	{
		Gui_Button_Resize(LCE->Button_Confirm, LCE->X + LCE->Width / 10.0f * 2.0f, LCE->Y + LCE->Height / 10.0f * 4.0f, LCE->Width / 10.0f * 2.0f, LCE->Height / 20.0f);
		LCE->Button_Confirm->TextHeight = LCE->Height / 20.0f;
		LCE->Button_Confirm->TextWidth = LCE->Width / 10.0f * 2.0f;

		Gui_Button_Resize(LCE->Button_Cancel, LCE->X + LCE->Width / 10.0f * 6.0f, LCE->Y + LCE->Height / 10.0f * 4.0f, LCE->Width / 10.0f * 2.0f, LCE->Height / 20.0f);
		LCE->Button_Cancel->TextHeight = LCE->Height / 20.0f;
		LCE->Button_Cancel->TextWidth = LCE->Width / 10.0f * 2.0f;

		Gui_TextBox_Resize(LCE->TextBox, LCE->X + LCE->Width / 10.0f * 3.0f, LCE->Y + LCE->Height / 10.0f * 6.0f, LCE->Width / 10.0f * 4.0f, LCE->Height / 20.0f);

		LCE->TextBox->Quad_takeMouse = LCE->Ligne_Text;

		LCE->TextBox->TextOffSetX = 0; LCE->TextBox->TextOffSetY = 0;
		LCE->TextBox->TextMaxWidth = LCE->Width / 10.0f * 4.0f; LCE->TextBox->TextHeight = LCE->Height / 20.0f;
	}

	/* The Textbox where the user can change the file's name */
	if(!LCE->TextBox_Name)
	{
		LCE->TextBox_Name = Gui_TextBox_Create(LCE->Width / 10.0f * 4.5f + LCE->X, LCE->Height / 20.0f * 19.0f + LCE->Y,
					LCE->Width / 10.0f * 3.5f, LCE->Height / 20.0f, "Name : ", 15, 0, 0, LCE->Height / 20.0f,
					LCE->Width / 10.0f * 3.5f, 0.5f, 0.5f, 0.5f,
					1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
		LCE->TextBox_Name->Quad_takeMouse = Quad_Create(LCE->X, LCE->Y, LCE->X, LCE->Y + LCE->Height, LCE->X + LCE->Width, LCE->Y + LCE->Height, LCE->X + LCE->Width, LCE->Y);
		LCE->TextBox_Name->AcceptDot = LCE->TextBox_Name->AcceptLetters = LCE->TextBox_Name->AcceptNumbers = true;
	} else {
		Gui_TextBox_Resize(LCE->TextBox_Name, LCE->Width / 10.0f * 4.5f + LCE->X, LCE->Height / 20.0f * 19.0f + LCE->Y,
					LCE->Width / 10.0f * 3.5f, LCE->Height / 20.0f);
		LCE->TextBox_Name->TextMaxWidth = LCE->Width / 10.0f * 3.5f;
		LCE->TextBox_Name->TextHeight = LCE->Height / 20.0f;
		LCE->TextBox_Name->Quad_takeMouse = Quad_Create(LCE->X, LCE->Y, LCE->X, LCE->Y + LCE->Height, LCE->X + LCE->Width, LCE->Y + LCE->Height, LCE->X + LCE->Width, LCE->Y);
	}
}

static struct File_Tab_File *Load_File(char *FileName)
{
	struct File_Tab_File *File = malloc(sizeof(struct File_Tab_File));
	File->Original = Lua_Code_OpenFile(FileName);
	File->Editing = Lua_Code_Copy(File->Original);

	File->Cursor_Pos.x = File->Cursor_Pos.y = 0;
	File->ScrollValue = 0;
	File->Selecting_Data.x = File->Selecting_Data.y = File->Selecting_Data.z = File->Selecting_Data.w = 0;
	File->isTextSelected = false;

	File->FileName_Edit = malloc(sizeof(char) * (String_length(FileName) + 1));
	File->FileName_Edit = strncpy(File->FileName_Edit, FileName, sizeof(char) * (String_length(FileName) + 1));

	File->FileName_Orig = malloc(sizeof(char) * (String_length(FileName) + 1));
	File->FileName_Orig = strncpy(File->FileName_Orig, FileName, sizeof(char) * (String_length(FileName) + 1));
	return File;
}

static void Close_File(struct File_Tab_File *FTF)
{
	free(FTF->FileName_Edit);
	free(FTF->FileName_Orig);

	Lua_Code_Free(&FTF->Original);
	Lua_Code_Free(&FTF->Editing);
}

static void Save_File(struct File_Tab_File *FTF)
{
	Lua_Code_Copy_noMalloc(FTF->Editing, FTF->Original);

	if(strcmp(FTF->FileName_Edit, FTF->FileName_Orig))
	{
		char *Path = FileExternal_GetFullPath(FTF->FileName_Orig);
		remove(Path);
		free(Path);

		free(FTF->FileName_Orig);
		FTF->FileName_Orig = malloc(sizeof(char) * (String_length(FTF->FileName_Edit) + 1));
		FTF->FileName_Orig = memcpy(FTF->FileName_Orig, FTF->FileName_Edit, sizeof(char) * (String_length(FTF->FileName_Edit) + 1));
	}

	struct F_FileExternal *File = FileExternal_OpenWrite(FTF->FileName_Orig, false, false);
	char *String = Lua_Code_toString(FTF->Original);

	FileExternal_Write(String, sizeof(char), String_length(String), File);
	FileExternal_Close(File);
	free(String);
}

static _Bool File_isDirty(struct File_Tab_File *FTF)
{
	if(strcmp(FTF->FileName_Edit, FTF->FileName_Orig))
		return true;

	char *S1 = Lua_Code_toString(FTF->Original), *S2 = Lua_Code_toString(FTF->Editing);
	int Length1 = String_length(S1), Length2 = String_length(S2);

	if(Length1 != Length2) return true;

	while(*S1) {
		if(*S1 != *S2) return true;
		S1++; S2++;
	}

	return false;
}

struct Lua_Code_Editor *Lua_Code_Editor_Init(GLuint LigneInWindow, float X,
											 float Y, float Width, float Height)
{
	struct Lua_Code_Editor *LCE = malloc(sizeof(struct Lua_Code_Editor));

	LCE->X = X;
	LCE->Y = Y;
	LCE->Width = Width;
	LCE->Height = Height;

	LCE->LignesInWindow = LigneInWindow;

	LCE->ScrollBar = NULL;
	LCE->VScrollBar = NULL;

	LCE->Button_Confirm = LCE->Button_Cancel = NULL;
	LCE->TextBox = NULL;
	LCE->TextBox_Name = NULL;

	Header_Button_Resize(LCE);

	LCE->Confirm_Text = Image_Load("Code_Editor/Confirm.png");
	LCE->Save_Text = Image_Load("Code_Editor/Save.png");
	LCE->Close_Text = Image_Load("Code_Editor/Close.png");
	LCE->Launch_Text = Image_Load("Code_Editor/Launch.png");
	LCE->New_Text = Image_Load("Code_Editor/NewFile.png");
	LCE->CopyPaste_Text = Image_Load("Code_Editor/Paste.png");
	LCE->Load_Text = Image_Load("Code_Editor/Load.png");
	LCE->Delete_Text = Image_Load("Code_Editor/Bin.png");

	LCE->Confirm_Hover = false;
	LCE->Save_Hover = false;
	LCE->Close_Hover = false;
	LCE->Launch_Hover = false;
	LCE->New_Hover = false;
	LCE->Delete_Hover = false;

	LCE->Ligne_Num_Color = Vector4_Create(0.5f, 0.5f, 0.5f, 1.0f);
	LCE->Ligne_Text_Color = Vector4_Create(1.0f, 1.0f, 1.0f, 1.0f);
	LCE->File_Tab_Color = Vector4_Create(0.4f, 0.4f, 0.4f, 1.0f);

	LCE->WritingCode = true;

	LCE->Cursor_Timer = 100;
	LCE->XtoReset = -1;

	LCE->ReserveWords_Size = 0;
	LCE->ReserveWords = String_Split("function end return local if elseif for in do then not or and type true false nil", ' ',
									 &LCE->ReserveWords_Size);

	LCE->Data = NULL;
	LCE->Launch_Game = NULL;

	LCE->Color_ReservedWords = Vector4_Create(0.70f, 0.0f, 0.60f, 1.0f);
	LCE->Color_Comments = Vector4_Create(0.6f, 0.6f, 0.6f, 1.0f);
	LCE->Color_Text = Vector4_Create(0.2f, 0.2f, 0.9f, 1.0f);
	LCE->Color_Constants = Vector4_Create(0.8f, 0.3f, 0.3f, 1.0f);

	LCE->Hover_addColor = Vector4_Create(-0.3f, -0.3f, -0.3f, 0.0f);

	LCE->isSelecting = false;
	LCE->canBeSelected = false;
	LCE->isShowingCopyPaste = false;
	LCE->File_Tab_Scroll = 0;
	LCE->File_Tab_canScroll = LCE->File_Tab_isScrolling = false;

	LCE->File_Tab_List = vector_new(sizeof(struct File_Tab_File));
	LCE->File_Tab_Index = -1;

	LCE->STATE = STATE_DEFAULT;

	return LCE;
}

struct Code_Part
{
	char *Part;
	struct Vector4f Color;
};

_Bool Comment_Block = false, Multi_LineQuote = false;

struct Vector6f *
Code_findCodeHighlits(struct Lua_Code_Editor *LCE, char *Code, int *Num)
{
	struct vector_t *V = vector_new(sizeof(struct Vector6f));

	static int MLQ_Num_OfEquals = 0; // Number of '=' for multi line quotes

	//int NumTemp = 0;
	int Code_Length = String_length(Code);
	//log_info("Test : %s", Code);
	for (int TempIndex = 0; TempIndex < Code_Length; TempIndex++)
	{
		if(Comment_Block)
		{
			int Length = 1;
			_Bool Continue = true;
			while(Code_Length != Length + TempIndex)
			{
				if(Code[Length + TempIndex] == '-' && Code_Length != Length + 3 + TempIndex
						&& Code[Length + 1 + TempIndex] == '-' && Code[Length + 2 + TempIndex] == ']' && Code[Length + 3 + TempIndex] == ']')
				{
					Length += 4;
					Continue = false;
					break;
				}
				Length++;
			}

			struct Vector6f *vec = malloc(sizeof(struct Vector6f));
			*vec = Vector6_Create(Vector2_Create(TempIndex,
				TempIndex + Length), LCE->Color_Comments);
			vector_push_back(V, vec);
			free(vec);
			(*Num)++;

			Comment_Block = Continue;
			TempIndex += Length;
			continue;
		}

		if(Multi_LineQuote)
		{
			_Bool CONTINUE = false;
			for(int i = 0; i + 2 + MLQ_Num_OfEquals <= Code_Length; i++)
			{
				if(Code[i] == ']')
				{
					for(int i2 = 1; i2 <= MLQ_Num_OfEquals; i2++)
						if(Code[i + i2] != '=')
							continue;
					if(Code[i + MLQ_Num_OfEquals + 1] == ']')
					{
						struct Vector6f *vec = malloc(sizeof(struct Vector6f));
						*vec = Vector6_Create(Vector2_Create(0,
								i + MLQ_Num_OfEquals + 2), LCE->Color_Text);
						vector_push_back(V, vec);
						free(vec);
						(*Num)++;
						TempIndex = i + MLQ_Num_OfEquals + 2;
						Multi_LineQuote = false;
						CONTINUE = true;
						goto exit_MLQ;
					}
				}
			}
			exit_MLQ: if(CONTINUE) continue;
			struct Vector6f *vec = malloc(sizeof(struct Vector6f));
			*vec = Vector6_Create(Vector2_Create(0,
					Code_Length), LCE->Color_Text);
			vector_push_back(V, vec);
			free(vec);
			(*Num)++;
			break;
		}
		if(Code[TempIndex] == '-' && TempIndex + 1 < Code_Length && Code[TempIndex + 1] == '-')
		{
			if(TempIndex + 3 < Code_Length && Code[TempIndex + 2] == '[' && Code[TempIndex + 3] == '[')
			{
				int Length = 3;
				_Bool Continue = true;
				while(Code_Length != Length + TempIndex)
				{
					if(Code[Length + TempIndex] == '-' && Code_Length != Length + 3 + TempIndex
							&& Code[Length + 1 + TempIndex] == '-' && Code[Length + 2 + TempIndex] == ']' && Code[Length + 3 + TempIndex] == ']')
					{
						Length += 3;
						Continue = false;
						break;
					}
					Length++;
				}

				struct Vector6f *vec = malloc(sizeof(struct Vector6f));
				*vec = Vector6_Create(Vector2_Create(TempIndex,
					TempIndex + Length), LCE->Color_Comments);
				vector_push_back(V, vec);
				free(vec);
				(*Num)++;

				Comment_Block = Continue;
				TempIndex += Length;
				continue;
			} else {
				struct Vector6f *vec = malloc(sizeof(struct Vector6f));
				*vec = Vector6_Create(Vector2_Create(TempIndex,
					TempIndex + (Code_Length - TempIndex)), LCE->Color_Comments);
				vector_push_back(V, vec);
				free(vec);
				(*Num)++;
				break;
			}
		}

		/* Check for " or ' or [ */
		if(Code[TempIndex] == '"')
		{
			int Length = 1;
			while(Code[TempIndex + Length] != '"' && Code_Length != Length + TempIndex)
				Length++;
			Length++;
			if(Code_Length < Length + TempIndex)
				Length = Code_Length - TempIndex;
			struct Vector6f *vec = malloc(sizeof(struct Vector6f));
			*vec = Vector6_Create(Vector2_Create(TempIndex,
				TempIndex + Length), LCE->Color_Text);
			vector_push_back(V, vec);
			free(vec);
			(*Num)++;
			TempIndex += Length - 1;
			continue;
		}
		if(Code[TempIndex] == '\'')
		{
			int Length = 1;
			while(Code[TempIndex + Length] != '\'' && Code_Length != Length + TempIndex)
				Length++;
			Length++;
			if(Code_Length < Length + TempIndex)
				Length = Code_Length - TempIndex;
			struct Vector6f *vec = malloc(sizeof(struct Vector6f));
			*vec = Vector6_Create(Vector2_Create(TempIndex,
				TempIndex + Length), LCE->Color_Text);
			vector_push_back(V, vec);
			free(vec);
			(*Num)++;
			TempIndex += Length - 1;
			continue;
		}
		if(Code[TempIndex] == '[')
		{
			if(TempIndex + 1 >= Code_Length)
				goto exit;
			MLQ_Num_OfEquals = 0;
			if(Code[TempIndex + 1] == '=')
			{
				MLQ_Num_OfEquals = 1;
				while(Code[TempIndex + MLQ_Num_OfEquals + 1] == '=' && TempIndex + MLQ_Num_OfEquals + 1 < Code_Length)
					MLQ_Num_OfEquals++;
				if(TempIndex + MLQ_Num_OfEquals + 1 >= Code_Length)
					goto exit;
			}

			if(Code[TempIndex + MLQ_Num_OfEquals + 1] == '[')
			{
				_Bool CONTINUE = false;
				for(int i = TempIndex + MLQ_Num_OfEquals + 1; i + 2 + MLQ_Num_OfEquals <= Code_Length; i++)
				{
					if(Code[i] == ']')
					{
						for(int i2 = 1; i2 <= MLQ_Num_OfEquals; i2++)
							if(Code[i + i2] != '=')
								continue;
						if(Code[i + MLQ_Num_OfEquals + 1] == ']')
						{
							struct Vector6f *vec = malloc(sizeof(struct Vector6f));
							*vec = Vector6_Create(Vector2_Create(TempIndex,
									i + MLQ_Num_OfEquals + 2), LCE->Color_Text);
							vector_push_back(V, vec);
							free(vec);
							(*Num)++;
							TempIndex += MLQ_Num_OfEquals + 2;
							Multi_LineQuote = false;
							CONTINUE = true;
							goto exit_MLQ_2;
						}
					}
				}
				exit_MLQ_2:
				if(CONTINUE) continue;
				struct Vector6f *vec = malloc(sizeof(struct Vector6f));
				*vec = Vector6_Create(Vector2_Create(0,
						Code_Length), LCE->Color_Text);
				vector_push_back(V, vec);
				free(vec);
				(*Num)++;
				Multi_LineQuote = true;
				break;
			}
		}exit:


		if(TempIndex > 0 && isalpha(Code[TempIndex - 1]))
			continue;

		if(isdigit(Code[TempIndex])) // Check if there is a number constants
		{
			if(TempIndex > 0 && isdigit(Code[TempIndex- 1]))
				goto escape;
			int Length = 1;
			while(isdigit(Code[TempIndex + Length]))
				Length++;

			if(!isalpha(Code[TempIndex + Length]))
			{
				struct Vector6f *vec = malloc(sizeof(struct Vector6f));
				*vec = Vector6_Create(Vector2_Create(TempIndex,
					TempIndex + Length), LCE->Color_Constants);
				vector_push_back(V, vec);
				free(vec);
				(*Num)++;
				TempIndex += Length - 1;
				continue;
			}
		}escape:
		if(String_contain(Code + TempIndex, "false"))
		{
			int Length = 5;

			if(!isalpha(Code[TempIndex + Length]))
			{
				struct Vector6f *vec = malloc(sizeof(struct Vector6f));
				*vec = Vector6_Create(Vector2_Create(TempIndex,
					TempIndex + Length), LCE->Color_Constants);
				vector_push_back(V, vec);
				free(vec);
				(*Num)++;
				TempIndex += Length - 1;
				continue;
			}
		}
		if(String_contain(Code + TempIndex, "true"))
		{
			int Length = 4;

			if(!isalpha(Code[TempIndex + Length]))
			{
				struct Vector6f *vec = malloc(sizeof(struct Vector6f));
				*vec = Vector6_Create(Vector2_Create(TempIndex,
					TempIndex + Length), LCE->Color_Constants);
				vector_push_back(V, vec);
				free(vec);
				(*Num)++;
				TempIndex += Length - 1;
				continue;
			}
		}

		if(TempIndex > 0 && isdigit(Code[TempIndex - 1]))
			continue;

		for (int i = 0; i < LCE->ReserveWords_Size; i++)
		{

			if (String_contain(Code + TempIndex, LCE->ReserveWords[i].String))
			{
				int Length = String_length(LCE->ReserveWords[i].String);
				if(TempIndex + Length < Code_Length && ((isalpha(Code[TempIndex + Length])) | (isdigit(Code[TempIndex + Length]))))
					continue;
				struct Vector6f *vec = malloc(sizeof(struct Vector6f));
				*vec = Vector6_Create(Vector2_Create(TempIndex, TempIndex + Length), LCE->Color_ReservedWords);

				vector_push_back(V, vec);
				free(vec);
				(*Num)++;
				TempIndex += Length;
				break;
			}
		}
	}

	if(!V->size)
	{
		vector_delete(V);
		return NULL;
	}

	struct Vector6f *Vector = malloc(sizeof(struct Vector6f) * *Num);
	for (int i = 0; i < *Num; i++)
	{
		Vector[i] = *((struct Vector6f *) V->items + i);
	}
	vector_delete(V);

	return Vector;
}

static struct Code_Part *
Code_SplitToParts(struct Lua_Code_Editor *LCE, char *Code, int *NumOfParts)
{
	int Num = 0, Code_Length = String_length(Code);

	if(!Code_Length)
		return NULL;

	struct Vector6f *ReserveWords = Code_findCodeHighlits(LCE, Code, &Num);

	if(!ReserveWords)
	{
		struct Code_Part *Part = malloc(sizeof(struct Code_Part));
		Part->Part = malloc(sizeof(char) * (Code_Length + 1));
		strcpy(Part->Part, Code);
		Part->Color = COLOR_BLACK;

		*NumOfParts = 1;
		return Part;
	}

	int NumParts = 0;
	for (int i = 0; i < Num; i++)
	{
		if (i == 0)
		{
			if ((int) ReserveWords[i].x != 0)
				NumParts++;
		}
		else if ((int) ReserveWords[i].x != (int) ReserveWords[i - 1].y)
			NumParts++;
	}
	if ((int) ReserveWords[Num - 1].y != Code_Length )
		NumParts++;

	struct Code_Part *Part = malloc(
			sizeof(struct Code_Part) * (NumParts + Num));
	int j = 0;

	for (int i = 0; i < Num; i++)
	{
		if (i == 0)
		{
			if ((int) ReserveWords[i].x != 0)
			{
				Part[0].Part = malloc(sizeof(char) * ((int) ReserveWords[0].x + 1));
				Part[0].Part = memcpy(Part[0].Part, Code, sizeof(char) * (int) ReserveWords[0].x);
				Part[0].Part[(int) ReserveWords[0].x] = '\0';
				Part[0].Color = COLOR_BLACK;
				Part[1].Part = malloc(sizeof(char) * ((int) ReserveWords[0].y - (int) ReserveWords[0].x + 1));
				Part[1].Part = memcpy(Part[1].Part, Code + (int) ReserveWords[0].x, sizeof(char) * (int) ReserveWords[0].y - (int) ReserveWords[0].x);
				Part[1].Part[(int) ReserveWords[0].y - (int) ReserveWords[0].x] = '\0';
				Part[1].Color = Vector4_Create(ReserveWords[0].z, ReserveWords[0].w, ReserveWords[0].h, ReserveWords[0].o);
				j += 2;
			}
			else
			{
				Part[0].Part = malloc(sizeof(char) * ((int) ReserveWords[0].y + 1));
				Part[0].Part = memcpy(Part[0].Part, Code, sizeof(char) * (int) ReserveWords[0].y);
				Part[0].Part[(int) ReserveWords[0].y] = '\0';
				Part[0].Color = Vector4_Create(ReserveWords[0].z, ReserveWords[0].w, ReserveWords[0].h, ReserveWords[0].o);
				j += 1;
			}
		}
		else if ((int) ReserveWords[i].x != (int) ReserveWords[i - 1].y)
		{
			Part[j].Part = malloc(sizeof(char) * ((int) ReserveWords[i].x - (int) ReserveWords[i - 1].y + 1));
			Part[j].Part = memcpy(Part[j].Part,  Code + (int) ReserveWords[i - 1].y, sizeof(char) * ((int) ReserveWords[i].x - (int) ReserveWords[i - 1].y));
			Part[j].Part[(int) ReserveWords[i].x - (int) ReserveWords[i - 1].y] = '\0';
			Part[j].Color = COLOR_BLACK;
			Part[j + 1].Part = malloc(sizeof(char) * ((int) ReserveWords[i].y - (int) ReserveWords[i].x + 1));
			Part[j + 1].Part = memcpy(Part[j + 1].Part, Code + (int) ReserveWords[i].x, sizeof(char) * (int) ReserveWords[i].y - (int) ReserveWords[i].x);
			Part[j + 1].Part[(int) ReserveWords[i].y - (int) ReserveWords[i].x] = '\0';
			Part[j + 1].Color = Vector4_Create(ReserveWords[i].z, ReserveWords[i].w, ReserveWords[i].h, ReserveWords[i].o);
			j += 2;
		}
		else
		{
			Part[j].Part = malloc(sizeof(char) * ((int) ReserveWords[i].y - (int) ReserveWords[i].x + 1));
			Part[j].Part = memcpy(Part[j].Part, Code + (int) ReserveWords[i].x, sizeof(char) * (int) ReserveWords[i].y - (int) ReserveWords[i].x);
			Part[j].Part[(int) ReserveWords[i].y - (int) ReserveWords[i].x] = '\0';
			Part[j].Color = Vector4_Create(ReserveWords[i].z, ReserveWords[i].w, ReserveWords[i].h, ReserveWords[i].o);
			j += 1;
		}
	}

	if ((int) ReserveWords[Num - 1].y != Code_Length)
	{
		Part[Num + NumParts - 1].Part = malloc(sizeof(char) * (Code_Length - (int) ReserveWords[Num - 1].y + 1));

		Part[Num + NumParts - 1].Part = memcpy(Part[Num + NumParts - 1].Part, Code + (int) ReserveWords[Num - 1].y, (Code_Length - (int) ReserveWords[Num - 1].y));

		Part[Num + NumParts - 1].Part[Code_Length - (int) ReserveWords[Num - 1].y] = '\0';
		Part[Num + NumParts - 1].Color = COLOR_BLACK;
	}

	free(ReserveWords);
	*NumOfParts = Num + NumParts;
	return Part;
}

static _Bool Header_Render(struct Lua_Code_Editor *LCE)
{
	// Make sure unfinished block comment doesn't transition over to next frame
	Comment_Block = Multi_LineQuote = false;

	if (LCE->Close.v3.x != LCE->X + LCE->Width || LCE->Close.v3.y != LCE->Y + LCE->Height || LCE->ScrollBar->ScrollBar.v1.y != LCE->Y || LCE->Ligne_Num.v1.x != LCE->X)
	{
		log_info("Code Editor Resizing : %i %i", Game_Width, Game_Height);

		Header_Button_Resize(LCE);
	}

	LCE->Confirm_Hover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y),
									  LCE->Confirm);
	LCE->Save_Hover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), LCE->Save);
	LCE->Close_Hover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y),
									LCE->Close);
	LCE->Launch_Hover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y),
									 LCE->Launch);
	LCE->New_Hover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), LCE->New);

	LCE->Load_Hover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), LCE->Load);

	LCE->Delete_Hover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), LCE->Delete);

	if (Mouse.justPressed && LCE->Confirm_Hover && LCE->File_Tab_Index != -1)
	{
		struct File_Tab_File *FTF = ((struct File_Tab_File *)LCE->File_Tab_List->items) + LCE->File_Tab_Index;

		if(LCE->TextBox_Name->Value[0])
		{
			free(FTF->FileName_Edit);
			FTF->FileName_Edit = malloc(sizeof(char) * (String_length(LCE->TextBox_Name->Value) + 1));
			FTF->FileName_Edit = memcpy(FTF->FileName_Edit, LCE->TextBox_Name->Value, sizeof(char) * (String_length(LCE->TextBox_Name->Value) + 1));
		}

		Save_File(FTF);
		Close_File(FTF);

		vector_erase(LCE->File_Tab_List, LCE->File_Tab_Index);

		if(LCE->File_Tab_Index > 0)
			LCE->File_Tab_Index--;
	}
	else if (Mouse.justPressed && LCE->Save_Hover && LCE->File_Tab_Index != -1)
	{
		struct File_Tab_File *FTF = ((struct File_Tab_File *)LCE->File_Tab_List->items) + LCE->File_Tab_Index;
		if(LCE->TextBox_Name->Value[0])
		{
			free(FTF->FileName_Edit);
			FTF->FileName_Edit = malloc(sizeof(char) * (String_length(LCE->TextBox_Name->Value) + 1));
			FTF->FileName_Edit = memcpy(FTF->FileName_Edit, LCE->TextBox_Name->Value, sizeof(char) * (String_length(LCE->TextBox_Name->Value) + 1));
		}
		Save_File(FTF);
	}
	else if (Mouse.justPressed && LCE->Close_Hover && LCE->File_Tab_Index != -1)
	{
		struct File_Tab_File *FTF = ((struct File_Tab_File *)LCE->File_Tab_List->items) + LCE->File_Tab_Index;

		if(!File_isDirty(FTF))
		{
			Close_File(FTF);

			vector_erase(LCE->File_Tab_List, LCE->File_Tab_Index);

			if(LCE->File_Tab_Index > 0)
				LCE->File_Tab_Index--;
			if(LCE->File_Tab_List->size == 0)
				LCE->File_Tab_Index = -1;
		} else {
			LCE->STATE = STATE_CONFIRMCLOSEFILE;
		}
	}
	else if (Mouse.justReleased && LCE->Launch_Hover)
	{
		if(Project.Script_toExecute)
		{
			if(LCE->Launch_Game)
				LCE->Launch_Game(LCE->Data, Project.Script_toExecute);
		} else if(LCE->File_Tab_Index != -1) {
			struct File_Tab_File *FTF = ((struct File_Tab_File *)LCE->File_Tab_List->items) + LCE->File_Tab_Index;
			if (LCE->Launch_Game)
				LCE->Launch_Game(LCE->Data, FTF->FileName_Orig);
		}
	}
	else if (Mouse.justPressed && LCE->New_Hover)
	{
		LCE->STATE = STATE_NEWFILE;

		if(LCE->Button_Confirm)
		{
			Gui_Button_Free(&LCE->Button_Confirm);
			Gui_Button_Free(&LCE->Button_Cancel);
			Gui_TextBox_Free(&LCE->TextBox);
		}
	} else if (Mouse.justPressed && LCE->Delete_Hover && LCE->File_Tab_Index != -1 && LCE->STATE == STATE_DEFAULT)
	{
		LCE->STATE = STATE_CONFIRMDELETEFILE;
	} else if (Mouse.justPressed && LCE->Load_Hover)
	{
		LCE->STATE = STATE_OPENFILE;

		if(LCE->Button_Confirm)
		{
			Gui_Button_Free(&LCE->Button_Confirm);
			Gui_Button_Free(&LCE->Button_Cancel);
			Gui_TextBox_Free(&LCE->TextBox);
		}
	}

	if (LCE->Confirm_Hover)
		Image_Shader.pushQuad(LCE->Confirm, Quad_Create(LCE->Confirm_Text->x, LCE->Confirm_Text->y2, LCE->Confirm_Text->x, LCE->Confirm_Text->y, LCE->Confirm_Text->x2, LCE->Confirm_Text->y, LCE->Confirm_Text->x2, LCE->Confirm_Text->y2),
							  LCE->Confirm_Text->Image, LCE->Hover_addColor);
	else
		Image_Shader.pushQuad(LCE->Confirm, Quad_Create(LCE->Confirm_Text->x, LCE->Confirm_Text->y2, LCE->Confirm_Text->x, LCE->Confirm_Text->y, LCE->Confirm_Text->x2, LCE->Confirm_Text->y, LCE->Confirm_Text->x2, LCE->Confirm_Text->y2),
							  LCE->Confirm_Text->Image, Vector4_Create(0.0f, 0.0f, 0.0f, 0.0f));

	if (LCE->Save_Hover)
		Image_Shader.pushQuad(LCE->Save, Quad_Create(LCE->Save_Text->x, LCE->Save_Text->y2, LCE->Save_Text->x, LCE->Save_Text->y, LCE->Save_Text->x2, LCE->Save_Text->y, LCE->Save_Text->x2, LCE->Save_Text->y2),
							  LCE->Save_Text->Image, LCE->Hover_addColor);
	else
		Image_Shader.pushQuad(LCE->Save, Quad_Create(LCE->Save_Text->x, LCE->Save_Text->y2, LCE->Save_Text->x, LCE->Save_Text->y, LCE->Save_Text->x2, LCE->Save_Text->y, LCE->Save_Text->x2, LCE->Save_Text->y2),
							  LCE->Save_Text->Image, Vector4_Create(0.0f, 0.0f, 0.0f, 0.0f));

	if (LCE->Close_Hover)
		Image_Shader.pushQuad(LCE->Close, Quad_Create(LCE->Close_Text->x, LCE->Close_Text->y2, LCE->Close_Text->x, LCE->Close_Text->y, LCE->Close_Text->x2, LCE->Close_Text->y, LCE->Close_Text->x2, LCE->Close_Text->y2),
							  LCE->Close_Text->Image, LCE->Hover_addColor);
	else
		Image_Shader.pushQuad(LCE->Close, Quad_Create(LCE->Close_Text->x, LCE->Close_Text->y2, LCE->Close_Text->x, LCE->Close_Text->y, LCE->Close_Text->x2, LCE->Close_Text->y, LCE->Close_Text->x2, LCE->Close_Text->y2),
							  LCE->Close_Text->Image, Vector4_Create(0.0f, 0.0f, 0.0f, 0.0f));

	if (LCE->Launch_Hover)
		Image_Shader.pushQuad(LCE->Launch, Quad_Create(LCE->Launch_Text->x, LCE->Launch_Text->y2, LCE->Launch_Text->x, LCE->Launch_Text->y, LCE->Launch_Text->x2, LCE->Launch_Text->y, LCE->Launch_Text->x2, LCE->Launch_Text->y2),
							  LCE->Launch_Text->Image, LCE->Hover_addColor);
	else
		Image_Shader.pushQuad(LCE->Launch, Quad_Create(LCE->Launch_Text->x, LCE->Launch_Text->y2, LCE->Launch_Text->x, LCE->Launch_Text->y, LCE->Launch_Text->x2, LCE->Launch_Text->y, LCE->Launch_Text->x2, LCE->Launch_Text->y2),
							  LCE->Launch_Text->Image, Vector4_Create(0.0f, 0.0f, 0.0f, 0.0f));

	if (LCE->New_Hover)
		Image_Shader.pushQuad(LCE->New, Quad_Create(LCE->New_Text->x, LCE->New_Text->y2, LCE->New_Text->x, LCE->New_Text->y, LCE->New_Text->x2, LCE->New_Text->y, LCE->New_Text->x2, LCE->New_Text->y2),
							  LCE->New_Text->Image, LCE->Hover_addColor);
	else
		Image_Shader.pushQuad(LCE->New, Quad_Create(LCE->New_Text->x, LCE->New_Text->y2, LCE->New_Text->x, LCE->New_Text->y, LCE->New_Text->x2, LCE->New_Text->y, LCE->New_Text->x2, LCE->New_Text->y2),
							  LCE->New_Text->Image, Vector4_Create(0.0f, 0.0f, 0.0f, 0.0f));

	if(LCE->Delete_Hover)
		Image_Shader.pushQuad(LCE->Delete, Quad_Create(LCE->Delete_Text->x, LCE->Delete_Text->y2, LCE->Delete_Text->x, LCE->Delete_Text->y, LCE->Delete_Text->x2, LCE->Delete_Text->y, LCE->Delete_Text->x2, LCE->Delete_Text->y2),
							  LCE->Delete_Text->Image, LCE->Hover_addColor);
	else
		Image_Shader.pushQuad(LCE->Delete, Quad_Create(LCE->Delete_Text->x, LCE->Delete_Text->y2, LCE->Delete_Text->x, LCE->Delete_Text->y, LCE->Delete_Text->x2, LCE->Delete_Text->y, LCE->Delete_Text->x2, LCE->Delete_Text->y2),
							  LCE->Delete_Text->Image, Vector4_Create(0.0f, 0.0f, 0.0f, 0.0f));

	if(LCE->Load_Hover)
		Image_Shader.pushQuad(LCE->Load, Quad_Create(LCE->Load_Text->x, LCE->Load_Text->y2, LCE->Load_Text->x, LCE->Load_Text->y, LCE->Load_Text->x2, LCE->Load_Text->y, LCE->Load_Text->x2, LCE->Load_Text->y2),
							  LCE->Load_Text->Image, LCE->Hover_addColor);
	else
		Image_Shader.pushQuad(LCE->Load, Quad_Create(LCE->Load_Text->x, LCE->Load_Text->y2, LCE->Load_Text->x, LCE->Load_Text->y, LCE->Load_Text->x2, LCE->Load_Text->y, LCE->Load_Text->x2, LCE->Load_Text->y2),
							  LCE->Load_Text->Image, Vector4_Create(0.0f, 0.0f, 0.0f, 0.0f));

	Default_Shader.pushQuad(LCE->File_Tab, LCE->File_Tab_Color);

	if(Mouse.justPressed && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), LCE->File_Tab))
		LCE->File_Tab_canScroll = true;
	else if(Mouse.justPressed)
		LCE->File_Tab_canScroll = false;

	if(Mouse.justReleased)
		LCE->File_Tab_isScrolling = false;

	if(Mouse.isLongedPressed && !LCE->File_Tab_isScrolling && LCE->File_Tab_canScroll)
		LCE->File_Tab_isScrolling = true;

	float FileTab_Width[LCE->File_Tab_List->size];
	float AllWidth = 0;
	float Next_FileTab = View_TranslateTo(LCE->File_Tab_Scroll, 640, Game_Width);

	for(int i = 0; i < LCE->File_Tab_List->size; i++)
	{
		FileTab_Width[i] = Font_HeightLength(DefaultFontManager, (((struct File_Tab_File *)LCE->File_Tab_List->items) + i)->FileName_Orig, LCE->File_Tab.v3.y - LCE->File_Tab.v4.y, 1.0f);
		FileTab_Width[i] = FileTab_Width[i] > FILE_TAB_MAXWIDTH ? FILE_TAB_MAXWIDTH : FileTab_Width[i];
		AllWidth += FileTab_Width[i];
	}

	if(LCE->File_Tab_Index != -1)
	{
		Gui_TextBox_Render(LCE->TextBox_Name);
	}

	if(LCE->File_Tab_isScrolling)
	{
		double X = View_TranslateTo(Mouse.x, Game_Width, 640);
		double preX = View_TranslateTo(Mouse.prevX, Game_Width, 640);

		double diffX = X - preX;

		if(((Next_FileTab + AllWidth > LCE->File_Tab.v3.x )&( diffX < 0 ))|| (((View_TranslateTo(LCE->File_Tab_Scroll, 640, Game_Width) + LCE->X) < 0) & ( diffX > 0 )))
			LCE->File_Tab_Scroll  += diffX;

		if(LCE->File_Tab_Scroll > 0)
			LCE->File_Tab_Scroll = 0;
		if(Next_FileTab + AllWidth < LCE->File_Tab.v3.x && LCE->File_Tab_Scroll < 0.0f)
		{
			float Temp = LCE->File_Tab.v3.x - (Next_FileTab + AllWidth);
			LCE->File_Tab_Scroll += View_TranslateTo(Temp, Game_Width, 640);
		}
	}

	for(int i = 0; i < LCE->File_Tab_List->size; i++)
	{
		struct Quad Quad = Quad_Create(Next_FileTab,
				LCE->File_Tab.v1.y, Next_FileTab,
				LCE->File_Tab.v2.y, Next_FileTab + FileTab_Width[i],
				LCE->File_Tab.v3.y, Next_FileTab + FileTab_Width[i], LCE->File_Tab.v4.y);

		_Bool Hover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), Quad);

		if(LCE->File_Tab_Index == i || Hover)
			Default_Shader.pushQuad(Quad, Vector4_Create(0.90f, 0.90f, 0.90f, 1.0f));

		struct File_Tab_File *File = ((struct File_Tab_File *)LCE->File_Tab_List->items) + i;
		Font_HeightRenderRenderConstraint(DefaultFontManager, File->FileName_Orig, Quad.v1.x + FileTab_Width[i] / 10.0f, Quad.v1.y, Quad.v2.y - Quad.v1.y, 1.0f, COLOR_RED, Vector2_Create(Quad.v1.x, Quad.v3.x));
		File = ((struct File_Tab_File *)LCE->File_Tab_List->items) + LCE->File_Tab_Index;
		if(Mouse.justQuickPressed && Hover)
		{
			if(i == LCE->File_Tab_Index)
				LCE->File_Tab_Index = -1;
			else {
				File->ScrollValue  = LCE->VScrollBar->BarY;
				LCE->File_Tab_Index = i;

				File = ((struct File_Tab_File *)LCE->File_Tab_List->items) + i;
				LCE->VScrollBar->BarY = File->ScrollValue;
				LCE->VScrollBar->TotalValue = LCE->Default_Ligne_Height * LCE->LignesInWindow;

				for(int j = 0; j < LCE->TextBox_Name->ValueLength; j++)
					LCE->TextBox_Name->Value[j] = '\0';
				memcpy(LCE->TextBox_Name->Value, File->FileName_Edit, sizeof(char) * String_length(File->FileName_Edit));
			}
		}

		Next_FileTab += FileTab_Width[i];
	}

	// Render Keyboard show
	if(Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), LCE->Keyboard_Show))
	{
		Default_Shader.pushQuad(LCE->Keyboard_Show, Vector4_Create(0.50f, 0.50f, 0.50f, 1.0f));

		if(Mouse.justReleased)
		{
			if(!OnScreen_Keyboard)Engine_requestKeyboard(0, 0, Game_Width, Game_Height / 20.0f * 9.0f);
			else Engine_requestCloseKeyboard();
		}
	} else
		Default_Shader.pushQuad(LCE->Keyboard_Show, Vector4_Create(1.0f, 1.0f, 1.0f, 1.0f));

	Font_FixedRender(DefaultFontManager, "Keyboard", LCE->Keyboard_Show.v1.x, LCE->Keyboard_Show.v1.y, LCE->Keyboard_Show.v2.y - LCE->Keyboard_Show.v1.y, LCE->Keyboard_Show.v3.x - LCE->Keyboard_Show.v1.x, 1.0f, Vector4_Create(1.0f, 0.0f, 0.0f, 1.0f));

	/* When the user pressed mouse or touches screen, Application.c track the movement on release to see if the user wants to change screen.
	 * To change screen in the Code Editor you need to press in the Ligne_Num space and drag, else the user doesn't want to change screen.
	 * If user isn't currently displaying any file, he can switch screen also by pressing on the Ligne_Text space */
	if(Mouse.justPressed && !Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), LCE->Ligne_Num)) {
		if(LCE->File_Tab_Index == -1 && !Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), LCE->Ligne_Text))
			Cancel_SwitchScreen = true; // Defined in Application.h
		else if(LCE->File_Tab_Index != -1)
			Cancel_SwitchScreen = true; // Defined in Application.h
	}

	return false;
}

static void Code_keyboardCopy(struct Lua_Code_Editor *LCE);
static void Code_keyboardPaste(struct Lua_Code_Editor *LCE);

static void Lignes_Render(struct Lua_Code_Editor *LCE)
{
	Default_Shader.pushQuad(LCE->Ligne_Num, LCE->Ligne_Num_Color);
	Default_Shader.pushQuad(LCE->Ligne_Text, LCE->Ligne_Text_Color);

	if(LCE->File_Tab_Index == -1)
		return;

	if(Mouse.y > LCE->Ligne_Text.v1.y && Mouse.y < LCE->Ligne_Text.v2.y && Mouse.justPressed)
		LCE->WritingCode = true;
	if(Mouse.justPressed && Mouse.y > LCE->Ligne_Text.v2.y)
		LCE->WritingCode = false;

	Gui_Horizontal_ScrollBar_Render(LCE->ScrollBar);

	struct File_Tab_File *File = ((struct File_Tab_File * ) LCE->File_Tab_List->items) + LCE->File_Tab_Index;

	if(File->Editing->Code->size * LCE->Default_Ligne_Height != LCE->VScrollBar->TotalValue)
	{
		if(File->Editing->Code->size * LCE->Default_Ligne_Height < LCE->LignesInWindow * LCE->Default_Ligne_Height && LCE->VScrollBar->TotalValue != LCE->LignesInWindow * LCE->Default_Ligne_Height)
			LCE->VScrollBar->TotalValue = LCE->LignesInWindow * LCE->Default_Ligne_Height;
		else
			LCE->VScrollBar->TotalValue = File->Editing->Code->size * LCE->Default_Ligne_Height;
	}

	Gui_Vertical_ScrollBar_Render(LCE->VScrollBar);

	File->ScrollValue = LCE->VScrollBar->BarY;

	struct Lua_Code_Ligne *Code =
			(struct Lua_Code_Ligne *) File->Editing->Code->items;

	float Line_DefaultStart = LCE->Ligne_Text.v2.x + (LCE->Ligne_Text.v3.x - LCE->Ligne_Text.v1.x) / 35.0f - LCE->ScrollBar->BarX;

	// Cursor position calculation
	float PositionX = Font_HeightMaxCharacterPosition(
			DefaultFontManager, Code[(int)File->Cursor_Pos.y].Ligne,
			Line_DefaultStart, LCE->Ligne_Height, LCE->Ligne_Default_Width,
			(int)File->Cursor_Pos.x);

	float Cursor_PositionX = PositionX, Cursor_PositionY = LCE->Ligne_Text.v2.y - LCE->Ligne_Height * (int)File->Cursor_Pos.y
															- (float) File->ScrollValue / 480.0f * Game_Height;

	Cursor_PositionX -= Game_Width / 10.0f / 2.0f;

	if(Cursor_PositionY + LCE->Ligne_Height > LCE->Ligne_Text.v2.y)
		Cursor_PositionY = LCE->Ligne_Text.v2.y - LCE->Ligne_Height;
	if(Cursor_PositionX < LCE->Ligne_Text.v1.x)
		Cursor_PositionX = LCE->Ligne_Text.v1.x;
	if(Cursor_PositionX + Game_Width / 10.0f > LCE->Ligne_Text.v3.x)
		Cursor_PositionX = LCE->Ligne_Text.v3.x - Game_Width / 10.0f;

	Cursor_PositionX += Game_Width / 10.0f / 2.0f;

	_Bool PasteCopy_Hover = false;
	if(LCE->isShowingCopyPaste)PasteCopy_Hover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), Quad_Create(Cursor_PositionX - (Game_Width / 20.0f), Cursor_PositionY, Cursor_PositionX - (Game_Width / 20.0f), Cursor_PositionY + LCE->Ligne_Height, Cursor_PositionX + (Game_Width / 20.0f), Cursor_PositionY + LCE->Ligne_Height, Cursor_PositionX + (Game_Width / 20.0f), Cursor_PositionY));

	if(Mouse.just_DoubleClicked && LCE->canBeSelected)
	{
		if(isalpha(Code[(int)File->Cursor_Pos.y].Ligne[(int)File->Cursor_Pos.x]))
		{
			int Y = (int)File->Cursor_Pos.y;
			int StartX = (int)File->Cursor_Pos.x, EndX = StartX;


			while(StartX > 0 && isalpha(Code[Y].Ligne[StartX - 1]))
				StartX--;
			while(EndX < CODE_MAX_LENGTH && isalpha(Code[Y].Ligne[EndX + 1]))
				EndX++;
			EndX++;

			File->isTextSelected = true;
			File->Selecting_Data.x = StartX;
			File->Selecting_Data.y = Y;
			File->Selecting_Data.z = EndX;
			File->Selecting_Data.w = Y;
		} else if((int)File->Cursor_Pos.x > 0 && isalpha(Code[(int)File->Cursor_Pos.y].Ligne[(int)File->Cursor_Pos.x - 1]))
		{
			int Y = (int)File->Cursor_Pos.y;
			int StartX = (int)File->Cursor_Pos.x - 1, EndX = StartX;


			while(StartX > 0 && isalpha(Code[Y].Ligne[StartX - 1]))
				StartX--;
			while(EndX < CODE_MAX_LENGTH && isalpha(Code[Y].Ligne[EndX + 1]))
				EndX++;
			EndX++;
			File->isTextSelected = true;
			File->Selecting_Data.x = StartX;
			File->Selecting_Data.y = Y;
			File->Selecting_Data.z = EndX;
			File->Selecting_Data.w = Y;
		}

		if(isdigit(Code[(int)File->Cursor_Pos.y].Ligne[(int)File->Cursor_Pos.x]))
		{
			int Y = (int)File->Cursor_Pos.y;
			int StartX = (int)File->Cursor_Pos.x, EndX = StartX;


			while(StartX > 0 && isdigit(Code[Y].Ligne[StartX - 1]))
				StartX--;
			while(EndX < CODE_MAX_LENGTH && isdigit(Code[Y].Ligne[EndX + 1]))
				EndX++;
			EndX++;

			File->isTextSelected = true;
			File->Selecting_Data.x = StartX;
			File->Selecting_Data.y = Y;
			File->Selecting_Data.z = EndX;
			File->Selecting_Data.w = Y;
		} else if((int)File->Cursor_Pos.x > 0 && isdigit(Code[(int)File->Cursor_Pos.y].Ligne[(int)File->Cursor_Pos.x - 1]))
		{
			int Y = (int)File->Cursor_Pos.y;
			int StartX = (int)File->Cursor_Pos.x - 1, EndX = StartX;


			while(StartX > 0 && isdigit(Code[Y].Ligne[StartX - 1]))
				StartX--;
			while(EndX < CODE_MAX_LENGTH && isdigit(Code[Y].Ligne[EndX + 1]))
				EndX++;
			EndX++;
			File->isTextSelected = true;
			File->Selecting_Data.x = StartX;
			File->Selecting_Data.y = Y;
			File->Selecting_Data.z = EndX;
			File->Selecting_Data.w = Y;
		}
	}

	if(!PasteCopy_Hover)
	{
		if(Mouse.justPressed)
		{
			if(Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), LCE->Ligne_Text)) {
				LCE->canBeSelected = true;
			}
			else
				LCE->canBeSelected = false;
		}
		if(Mouse.justQuickPressed && !Mouse.just_DoubleClicked && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), LCE->Ligne_Text))
		{
			File->isTextSelected = false;
			File->Selecting_Data.x = File->Selecting_Data.y = File->Selecting_Data.z = File->Selecting_Data.w = -5;
			LCE->isShowingCopyPaste = false;
		}
		if(Mouse.justReleased)
		{
			LCE->isSelecting = false;
		}

		if(Mouse.isSuper_longPress && !LCE->isSelecting && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), LCE->Ligne_Text) && LCE->canBeSelected)
		{
			LCE->isShowingCopyPaste = true;
		}

		if (LCE->isSelecting && Mouse.isLongedPressed)
		{
			if (Mouse.y < LCE->Ligne_Text.v1.y)
			{
				Mouse.justScrolled = true;
				Mouse.scrollY = -1;
				Mouse.Faked_Input = true;
			}
			else if (Mouse.y > LCE->Ligne_Text.v2.y)
			{
				Mouse.justScrolled = true;
				Mouse.scrollY = 1;
				Mouse.Faked_Input = true;
			}

			if(Mouse.x < LCE->Ligne_Text.v1.x && LCE->ScrollBar->BarX - 1 >= 0)
			{
				LCE->ScrollBar->BarX -= 2;
			} else if(Mouse.x > LCE->Ligne_Text.v3.x && LCE->ScrollBar->BarX + 1 <= LCE->ScrollBar->TotalValue)
			{
				LCE->ScrollBar->BarX += 2;
			}
		}
	}
	// For selection variables :
	int StartX = 0, StartY = 0, EndX = 0, EndY = 0;

	if(File->isTextSelected) {
		if (((File->Selecting_Data.x > File->Selecting_Data.z)
			 & (File->Selecting_Data.y == File->Selecting_Data.w))
			|| File->Selecting_Data.y > File->Selecting_Data.w) {
			StartX = (int) File->Selecting_Data.z;
			StartY = (int) File->Selecting_Data.w;
			EndX = (int) File->Selecting_Data.x;
			EndY = (int) File->Selecting_Data.y;
		}
		else {
			StartX = (int) File->Selecting_Data.x;
			StartY = (int) File->Selecting_Data.y;
			EndX = (int) File->Selecting_Data.z;
			EndY = (int) File->Selecting_Data.w;
		}
	}
	float Scroll =  File->ScrollValue / 480.0f * (float)Game_Height;
	// Render Text for each lignes
	for (int i = 0; i < File->Editing->Code->size; i++)
	{
		struct Quad Quad = Quad_Create(Line_DefaultStart,
									   LCE->Ligne_Text.v2.y - LCE->Ligne_Height - LCE->Ligne_Height * i
									   - Scroll, Line_DefaultStart,
									   LCE->Ligne_Text.v2.y - LCE->Ligne_Height * i
									   - Scroll,
									   Line_DefaultStart + Font_HeightLength(DefaultFontManager, Code[i].Ligne, LCE->Ligne_Height, 1.0f),
									   LCE->Ligne_Text.v2.y - LCE->Ligne_Height * i
									   - Scroll,
									   Line_DefaultStart + Font_HeightLength(DefaultFontManager, Code[i].Ligne, LCE->Ligne_Height, 1.0f),
									   LCE->Ligne_Text.v2.y - LCE->Ligne_Height - LCE->Ligne_Height * i
									   - Scroll);
		if (Quad.v4.y > LCE->Ligne_Text.v2.y)
			continue;
		else if (Quad.v3.y <= LCE->Ligne_Text.v1.y)
			continue;
		if (Quad.v4.y < LCE->Ligne_Text.v1.y)
			Quad.v4.y = Quad.v1.y = LCE->Ligne_Text.v1.y;
		if (Quad.v3.y > LCE->Ligne_Text.v2.y)
			Quad.v2.y = Quad.v3.y = LCE->Ligne_Text.v2.y;

		if (LCE->canBeSelected && Mouse.justPressed && Mouse.x >= LCE->Ligne_Text.v1.x
			&& Mouse.x <= LCE->Ligne_Text.v3.x && Mouse.y >= Quad.v1.y
			&& Mouse.y <= Quad.v2.y && !PasteCopy_Hover && !LCE->isShowingCopyPaste)
		{
			File->Cursor_Pos.x = Font_HeightCharacterAt(DefaultFontManager, Code[i].Ligne, Line_DefaultStart, Quad.v2.y - Quad.v1.y, Mouse.x);
			log_info("%f", File->Cursor_Pos.x);
			File->Cursor_Pos.y = i;

			//Updating Cursor X position needed
			PositionX = Font_HeightMaxCharacterPosition(
				DefaultFontManager, Code[(int)File->Cursor_Pos.y].Ligne,
				Line_DefaultStart, LCE->Ligne_Height, LCE->Ligne_Default_Width,
				(int)File->Cursor_Pos.x);

			Cursor_PositionX = PositionX, Cursor_PositionY = LCE->Ligne_Text.v2.y - LCE->Ligne_Height * (int)File->Cursor_Pos.y
					- (float) Scroll;
		}
		char *Ligne_Num = Integer_toString(i+1);

		Font_HeightMaxRenderCA(DefaultFontManager, Ligne_Num,
							   (LCE->Ligne_Num.v3.x - LCE->Ligne_Num.v1.x) / 2.0f, Quad.v4.y,
							   Quad.v2.y - Quad.v1.y,
							   LCE->Ligne_Num.v3.x - LCE->Ligne_Num.v1.x, 1.0f, COLOR_BLACK);

		free(Ligne_Num);

		if (Quad.v3.x < LCE->Ligne_Text.v1.x)
			continue;
		if (Quad.v1.x > LCE->Ligne_Text.v3.x)
			continue;

		int NumParts = 0;
		struct Code_Part *CP = Code_SplitToParts(LCE, Code[i].Ligne, &NumParts);
		float x = 0;

		if(CP) {
			x = Quad.v1.x;

			for (int ji = 0; ji < NumParts; ji++) {
				x += Font_HeightRenderRenderConstraint(DefaultFontManager,
													   CP[ji].Part, x, Quad.v4.y,
													   Quad.v2.y - Quad.v1.y, 1.0f,
													   CP[ji].Color,
													   Vector2_Create(LCE->Ligne_Text.v1.x,
																	  LCE->Ligne_Text.v3.x));
				free(CP[ji].Part);
			}

			free(CP);
		}

		// Cursor shananigans
		if (i == File->Cursor_Pos.y)
		{
			LCE->Cursor_Timer--;

			float Width = 2;

			if (LCE->Cursor_Timer >= 50)
			{
				if (PositionX >= LCE->Ligne_Text.v1.x && PositionX <= LCE->Ligne_Text.v3.x)
				{
					Default_Shader.pushQuad(
							Quad_Create(PositionX - Width, Quad.v1.y,
									PositionX - Width, Quad.v2.y, PositionX,
										Quad.v2.y, PositionX, Quad.v1.y),
							Vector4_Create(0.0f, 0.0f, 0.0f, 1.0f));
				}
			}
			else if (LCE->Cursor_Timer == 0)
				LCE->Cursor_Timer = 100;
		}
		if (Mouse.isLongedPressed && Mouse.x >= LCE->Ligne_Text.v1.x
			&& Mouse.x <= LCE->Ligne_Text.v3.x && Mouse.y >= Quad.v1.y
			&& Mouse.y <= Quad.v2.y && !PasteCopy_Hover)
		{
			int x = Font_HeightCharacterAt(DefaultFontManager, Code[i].Ligne,
										  Line_DefaultStart, Quad.v2.y - Quad.v1.y, Mouse.x);

			if (!LCE->isSelecting  && !File->isTextSelected
				&& Point_inQuad(Vector2_Create(Mouse.x, Mouse.y),
								LCE->Ligne_Text) && !Mouse.justScrolled  && LCE->canBeSelected)
			{
				if(File->Selecting_Data.x == -5)
				{
					File->Selecting_Data.x = x;
					File->Selecting_Data.y = i;
				} else if(File->Selecting_Data.x != x || File->Selecting_Data.y != i) {
					File->Selecting_Data.z = x;
					File->Selecting_Data.w = i;
					File->Cursor_Pos.x = x;
					File->Cursor_Pos.y = i;
					LCE->Cursor_Timer = 100;
					LCE->isSelecting = true;
					File->isTextSelected = true;
					LCE->canBeSelected = false;
				}
			}
			else if(LCE->isSelecting)
			{
				File->Selecting_Data.z = x;
				File->Selecting_Data.w = i;
				File->Cursor_Pos.x = x;
				File->Cursor_Pos.y = i;
				LCE->Cursor_Timer = 100;
			}
		}

		// Selecting rendering
		if (File->isTextSelected && Code[i].Ligne[0] != '\0')
		{
			if (i > StartY && i < EndY)
			{
				float EndXX = Quad.v3.x;
				if(EndXX > LCE->Ligne_Text.v3.x)
					EndXX = LCE->Ligne_Text.v3.x;

				if(Quad.v1.x < LCE->Ligne_Text.v1.x)
					Default_Shader.pushQuad(Quad_Create(LCE->Ligne_Text.v1.x, Quad.v1.y, LCE->Ligne_Text.v1.x, Quad.v2.y, EndXX, Quad.v3.y, EndXX, Quad.v4.y), Vector4_Create(0.2f, 0.2f, 0.8f, 0.5f));
				else
					Default_Shader.pushQuad(Quad_Create(Quad.v1.x, Quad.v1.y, Quad.v2.x, Quad.v2.y, EndXX, Quad.v3.y, EndXX, Quad.v4.y), Vector4_Create(0.2f, 0.2f, 0.8f, 0.5f));
			}
			else if (i == StartY)
			{
				float StartXX = Quad.v1.x + Font_HeightLength_SLength(DefaultFontManager,
																	  StartX + String_numoftab_Length(Code[StartY].Ligne, StartX), Quad.v2.y - Quad.v1.y, 1.0f);

				float EndXX = 0;
				if (i == EndY)
					EndXX = StartXX + Font_HeightLength_SLength( DefaultFontManager,
																 EndX - StartX + String_numoftab_Length(((struct Lua_Code_Ligne *) File->Editing->Code->items
																										 + StartY)->Ligne + StartX, EndX - StartX), Quad.v2.y - Quad.v1.y, 1.0f);
				else
					EndXX = StartXX + Font_HeightLength(DefaultFontManager, ((struct Lua_Code_Ligne *) File->Editing->Code->items
																			 + i)->Ligne + StartX, Quad.v2.y - Quad.v1.y, 1.0f);

				if(StartXX < LCE->Ligne_Text.v1.x)
					StartXX = LCE->Ligne_Text.v1.x;
				if(EndXX < LCE->Ligne_Text.v1.x)
					continue;
				if(EndXX > LCE->Ligne_Text.v3.x)
					EndXX = LCE->Ligne_Text.v3.x;

				Default_Shader.pushQuad(
						Quad_Create(StartXX, Quad.v1.y, StartXX, Quad.v2.y,
									EndXX, Quad.v3.y, EndXX, Quad.v4.y),
						Vector4_Create(0.2f, 0.2f, 0.8f, 0.5f));
			}
			else if (i == EndY)
			{
				float StartXX = LCE->Ligne_Text.v2.x
								+ (LCE->Ligne_Text.v3.x - LCE->Ligne_Text.v1.x) / 35.0f
								- LCE->ScrollBar->BarX;
				float EndXX = StartXX + Font_HeightLength_SLength(DefaultFontManager, EndX
																					  + String_numoftab_Length( ((struct Lua_Code_Ligne *) File->Editing->Code->items
																												 + i)->Ligne, EndY), Quad.v2.y - Quad.v1.y, 1.0f);

				if(StartXX < LCE->Ligne_Text.v1.x)
					StartXX = LCE->Ligne_Text.v1.x;
				if(EndXX < LCE->Ligne_Text.v1.x)
					continue;
				if(EndXX > LCE->Ligne_Text.v3.x)
					EndXX = LCE->Ligne_Text.v3.x;

				Default_Shader.pushQuad(
						Quad_Create(StartXX, Quad.v1.y, StartXX, Quad.v2.y,
									EndXX, Quad.v3.y, EndXX, Quad.v4.y),
						Vector4_Create(0.2f, 0.2f, 0.8f, 0.5f));
			}
		}
	}

	Util_Flush();
	if(LCE->isShowingCopyPaste)
	{
		Cursor_PositionX -= Game_Width / 10.0f / 2.0f;
		_Bool Paste_Hover = false, Copy_Hover = false;
		if(PasteCopy_Hover)
		{
			if(Mouse.x < Cursor_PositionX + Game_Width / 10.0f / 2.0f)
			{
				Copy_Hover = true;
				Paste_Hover = false;
			} else
			{
				Copy_Hover = false;
				Paste_Hover = true;
			}
		}

		if(Copy_Hover)
			Image_Shader.pushQuad(Quad_Create(Cursor_PositionX, Cursor_PositionY, Cursor_PositionX, Cursor_PositionY + LCE->Ligne_Height, Cursor_PositionX + Game_Width / 10.0f / 2.0f, Cursor_PositionY + LCE->Ligne_Height, Cursor_PositionX + Game_Width / 10.0f / 2.0f, Cursor_PositionY), Quad_Create(LCE->CopyPaste_Text->x, LCE->CopyPaste_Text->y2, LCE->CopyPaste_Text->x, LCE->CopyPaste_Text->y2, LCE->CopyPaste_Text->x + (LCE->CopyPaste_Text->x2 - LCE->CopyPaste_Text->x)/2.0f, LCE->CopyPaste_Text->y, LCE->CopyPaste_Text->x + (LCE->CopyPaste_Text->x2 - LCE->CopyPaste_Text->x)/2.0f, LCE->CopyPaste_Text->y2), LCE->CopyPaste_Text->Image, Vector4_Create(-0.2f, -0.2f, -0.2f, 1));
		else
			Image_Shader.pushQuad(Quad_Create(Cursor_PositionX, Cursor_PositionY, Cursor_PositionX, Cursor_PositionY + LCE->Ligne_Height, Cursor_PositionX + Game_Width / 10.0f / 2.0f, Cursor_PositionY + LCE->Ligne_Height, Cursor_PositionX + Game_Width / 10.0f / 2.0f, Cursor_PositionY), Quad_Create(LCE->CopyPaste_Text->x, LCE->CopyPaste_Text->y2, LCE->CopyPaste_Text->x, LCE->CopyPaste_Text->y2, LCE->CopyPaste_Text->x + (LCE->CopyPaste_Text->x2 - LCE->CopyPaste_Text->x)/2.0f, LCE->CopyPaste_Text->y, LCE->CopyPaste_Text->x + (LCE->CopyPaste_Text->x2 - LCE->CopyPaste_Text->x)/2.0f, LCE->CopyPaste_Text->y2), LCE->CopyPaste_Text->Image, Vector4_Create(0.0f, 0.0f, 0.0f, 1));

		if(Paste_Hover)
			Image_Shader.pushQuad(Quad_Create(Cursor_PositionX + Game_Width / 10.0f / 2.0f, Cursor_PositionY, Cursor_PositionX + Game_Width / 10.0f / 2.0f, Cursor_PositionY + LCE->Ligne_Height, Cursor_PositionX + Game_Width / 10.0f, Cursor_PositionY + LCE->Ligne_Height, Cursor_PositionX + Game_Width / 10.0f, Cursor_PositionY), Quad_Create(LCE->CopyPaste_Text->x + (LCE->CopyPaste_Text->x2 - LCE->CopyPaste_Text->x)/2.0f, LCE->CopyPaste_Text->y2, LCE->CopyPaste_Text->x + (LCE->CopyPaste_Text->x2 - LCE->CopyPaste_Text->x)/2.0f, LCE->CopyPaste_Text->y2, LCE->CopyPaste_Text->x2, LCE->CopyPaste_Text->y, LCE->CopyPaste_Text->x2, LCE->CopyPaste_Text->y2), LCE->CopyPaste_Text->Image, Vector4_Create(-0.2f, -0.2f, -0.2f, 1));
		else
			Image_Shader.pushQuad(Quad_Create(Cursor_PositionX + Game_Width / 10.0f / 2.0f, Cursor_PositionY, Cursor_PositionX + Game_Width / 10.0f / 2.0f, Cursor_PositionY + LCE->Ligne_Height, Cursor_PositionX + Game_Width / 10.0f, Cursor_PositionY + LCE->Ligne_Height, Cursor_PositionX + Game_Width / 10.0f, Cursor_PositionY), Quad_Create(LCE->CopyPaste_Text->x + (LCE->CopyPaste_Text->x2 - LCE->CopyPaste_Text->x)/2.0f, LCE->CopyPaste_Text->y2, LCE->CopyPaste_Text->x + (LCE->CopyPaste_Text->x2 - LCE->CopyPaste_Text->x)/2.0f, LCE->CopyPaste_Text->y2, LCE->CopyPaste_Text->x2, LCE->CopyPaste_Text->y, LCE->CopyPaste_Text->x2, LCE->CopyPaste_Text->y2), LCE->CopyPaste_Text->Image, Vector4_Create(0.0f, 0.0f, 0.0f, 1));

		if(Mouse.justReleased)
		{
			if(Copy_Hover)
				Code_keyboardCopy(LCE);
			else if(Paste_Hover)
				Code_keyboardPaste(LCE);
			LCE->isShowingCopyPaste = false;
		}
	}
}

static void OpenFile_Render(struct Lua_Code_Editor *LCE)
{
	static _Bool Display_Error_Message = false;
	static int Display_toGo = 0;

	if(!LCE->Button_Confirm)
	{
		LCE->Button_Confirm = Gui_Button_Create(LCE->X + LCE->Width / 10.0f * 2.0f, LCE->Y + LCE->Height / 10.0f * 4.0f, LCE->Width / 10.0f * 2.0f, LCE->Height / 20.0f,
				"Open", 0, 0, LCE->Width / 10.0f * 2.0f, LCE->Height / 20.0f, 0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);

		LCE->Button_Cancel = Gui_Button_Create(LCE->X + LCE->Width / 10.0f * 6.0f, LCE->Y + LCE->Height / 10.0f * 4.0f, LCE->Width / 10.0f * 2.0f, LCE->Height / 20.0f,
				"Cancel", 0, 0, LCE->Width / 10.0f * 2.0f, LCE->Height / 20.0f, 0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);

		LCE->Button_Confirm->CenterText = true;
		LCE->Button_Cancel->CenterText = true;

		LCE->TextBox = Gui_TextBox_Create(LCE->X + LCE->Width / 10.0f * 3.0f, LCE->Y + LCE->Height / 10.0f * 6.0f, LCE->Width / 10.0f * 4.0f, LCE->Height / 20.0f, "File : ", 15, 0, 0, LCE->Height / 20.0f, LCE->Width / 10.0f * 4.0f, 0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
		LCE->TextBox->AcceptDot = LCE->TextBox->AcceptLetters = LCE->TextBox->AcceptNumbers = true;

		LCE->TextBox->Quad_takeMouse = LCE->Ligne_Text;
	}

	if(Display_Error_Message)
	{
		Font_FixedRender(DefaultFontManager, "File does not exists", Game_Width / 10.0f * 3.0f, Game_Height / 10.0f * 5.0f, Game_Height / 20.0f, Game_Width / 10.0f * 4.0f, 1.0f, COLOR_RED);
		Display_toGo--;

		if(Display_toGo <= 0)
			Display_Error_Message = false;
	}

	Gui_TextBox_Render(LCE->TextBox);

	if(Gui_Button_Render(LCE->Button_Confirm))
	{
		if(!FileExternal_Exists(LCE->TextBox->Value))
		{
			Display_Error_Message = true;
			Display_toGo = 500;
		}
		else
		{
			struct File_Tab_File *FTF = Load_File(LCE->TextBox->Value);
			LCE->File_Tab_Index = LCE->File_Tab_List->size;
			vector_push_back(LCE->File_Tab_List, FTF);

			for(int j = 0; j < LCE->TextBox_Name->ValueLength; j++)
				LCE->TextBox_Name->Value[j] = '\0';
			memcpy(LCE->TextBox_Name->Value, FTF->FileName_Edit, sizeof(char) * String_length(FTF->FileName_Edit));

			free(FTF);
			LCE->STATE = STATE_DEFAULT;
		}
	}

	if(Gui_Button_Render(LCE->Button_Cancel))
	{
		LCE->STATE = STATE_DEFAULT;
	}

	if(LCE->STATE != STATE_OPENFILE)
	{
		Gui_Button_Free(&LCE->Button_Confirm);
		Gui_Button_Free(&LCE->Button_Cancel);
		Gui_TextBox_Free(&LCE->TextBox);
	}
}

static void NewFile_Render(struct Lua_Code_Editor *LCE)
{
	static _Bool Display_Error_Message = false;
	static int Display_toGo = 0;

	if(!LCE->Button_Confirm)
	{
		LCE->Button_Confirm = Gui_Button_Create(LCE->X + LCE->Width / 10.0f * 2.0f, LCE->Y + LCE->Height / 10.0f * 4.0f, LCE->Width / 10.0f * 2.0f, LCE->Height / 20.0f,
				"New", 0, 0, LCE->Width / 10.0f * 2.0f, LCE->Height / 20.0f, 0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);

		LCE->Button_Cancel = Gui_Button_Create(LCE->X + LCE->Width / 10.0f * 6.0f, LCE->Y + LCE->Height / 10.0f * 4.0f, LCE->Width / 10.0f * 2.0f, LCE->Height / 20.0f,
				"Cancel", 0, 0, LCE->Width / 10.0f * 2.0f, LCE->Height / 20.0f, 0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);

		LCE->Button_Confirm->CenterText = true;
		LCE->Button_Cancel->CenterText = true;

		LCE->TextBox = Gui_TextBox_Create(LCE->X + LCE->Width / 10.0f * 3.0f, LCE->Y + LCE->Height / 10.0f * 6.0f, LCE->Width / 10.0f * 4.0f, LCE->Height / 20.0f, "File : ", 15, 0, 0, LCE->Height / 20.0f, LCE->Width / 10.0f * 4.0f, 0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
		LCE->TextBox->AcceptDot = LCE->TextBox->AcceptLetters = LCE->TextBox->AcceptNumbers = true;

		LCE->TextBox->Quad_takeMouse = LCE->Ligne_Text;
	}

	if(Display_Error_Message)
	{
		Font_FixedRender(DefaultFontManager, "File already exists", Game_Width / 10.0f * 3.0f, Game_Height / 10.0f * 5.0f, Game_Height / 20.0f, Game_Width / 10.0f * 4.0f, 1.0f, COLOR_RED);
		Display_toGo--;

		if(Display_toGo <= 0)
			Display_Error_Message = false;
	}

	Gui_TextBox_Render(LCE->TextBox);

	if(Gui_Button_Render(LCE->Button_Confirm))
	{
		if(FileExternal_Exists(LCE->TextBox->Value))
		{
			Display_Error_Message = true;
			Display_toGo = 500;
		}
		else
		{
			struct F_FileExternal *File = FileExternal_OpenWrite(LCE->TextBox->Value, false, false);
			FileExternal_Close(File);

			struct File_Tab_File *FTF = Load_File(LCE->TextBox->Value);
			LCE->File_Tab_Index = LCE->File_Tab_List->size;
			vector_push_back(LCE->File_Tab_List, FTF);

			for(int j = 0; j < LCE->TextBox_Name->ValueLength; j++)
				LCE->TextBox_Name->Value[j] = '\0';
			memcpy(LCE->TextBox_Name->Value, FTF->FileName_Edit, sizeof(char) * String_length(FTF->FileName_Edit));

			free(FTF);
			LCE->STATE = STATE_DEFAULT;
		}
	}

	if(Gui_Button_Render(LCE->Button_Cancel))
	{
		LCE->STATE = STATE_DEFAULT;
	}

	if(LCE->STATE != STATE_NEWFILE)
	{
		Gui_Button_Free(&LCE->Button_Confirm);
		Gui_Button_Free(&LCE->Button_Cancel);
		Gui_TextBox_Free(&LCE->TextBox);
	}
}

static void ConfirmClose_Render(struct Lua_Code_Editor *LCE)
{
	if(!LCE->Button_Confirm)
	{
		LCE->Button_Confirm = Gui_Button_Create(LCE->X + LCE->Width / 10.0f * 2.0f, LCE->Y + LCE->Height / 10.0f * 4.0f, LCE->Width / 10.0f * 2.0f, LCE->Height / 20.0f,
				"Confirm", 0, 0, LCE->Width / 10.0f * 2.0f, LCE->Height / 20.0f, 0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);

		LCE->Button_Cancel = Gui_Button_Create(LCE->X + LCE->Width / 10.0f * 6.0f, LCE->Y + LCE->Height / 10.0f * 4.0f, LCE->Width / 10.0f * 2.0f, LCE->Height / 20.0f,
				"Cancel", 0, 0, LCE->Width / 10.0f * 2.0f, LCE->Height / 20.0f, 0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);

		LCE->Button_Confirm->CenterText = true;
		LCE->Button_Cancel->CenterText = true;
	}

	LCE->Button_Confirm->TextOffSetX = LCE->Width / 50.0f;

	Font_HeightRender(DefaultFontManager, "This file has been modified, do you wish to close", LCE->X + LCE->Width / 10.0f * 0.3f, LCE->Y + LCE->Height / 10.0f * 6.0f, LCE->Height / 25.0f, 1.0f, COLOR_RED);
	Font_HeightRender(DefaultFontManager, "(it will not save)", LCE->X + LCE->Width / 10.0f * 3.0f, LCE->Y + LCE->Height / 10.0f * 5.5f, LCE->Height / 25.0f, 1.0f, COLOR_RED);

	if(Gui_Button_Render(LCE->Button_Confirm))
	{
		struct File_Tab_File *FTF = ((struct File_Tab_File *)LCE->File_Tab_List->items) + LCE->File_Tab_Index;
		Close_File(FTF);

		vector_erase(LCE->File_Tab_List, LCE->File_Tab_Index);

		if(LCE->File_Tab_Index > 0)
		{
			LCE->File_Tab_Index--;

			struct File_Tab_File *FTF = ((struct File_Tab_File *)LCE->File_Tab_List->items) + LCE->File_Tab_Index;
			for(int j = 0; j < LCE->TextBox_Name->ValueLength; j++)
				LCE->TextBox_Name->Value[j] = '\0';
			memcpy(LCE->TextBox_Name->Value, FTF->FileName_Edit, sizeof(char) * String_length(FTF->FileName_Edit));

		} else
			LCE->File_Tab_Index = -1;

		LCE->STATE = STATE_DEFAULT;
	}

	Gui_Button_Render(LCE->Button_Cancel);
	if(LCE->Button_Cancel->Hover && Mouse.justReleased)
	{
		LCE->STATE = STATE_DEFAULT;
	}

	if(LCE->STATE != STATE_CONFIRMCLOSEFILE)
	{
		Gui_Button_Free(&LCE->Button_Confirm);
		Gui_Button_Free(&LCE->Button_Cancel);
	}
}

static void ConfirmDelete_Render(struct Lua_Code_Editor *LCE)
{
	if(!LCE->Button_Confirm)
	{
		LCE->Button_Confirm = Gui_Button_Create(LCE->X + LCE->Width / 10.0f * 2.0f, LCE->Y + LCE->Height / 10.0f * 4.0f, LCE->Width / 10.0f * 2.0f, LCE->Height / 20.0f,
				"Confirm", 0, 0, LCE->Width / 10.0f * 2.0f, LCE->Height / 20.0f, 0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);

		LCE->Button_Cancel = Gui_Button_Create(LCE->X + LCE->Width / 10.0f * 6.0f, LCE->Y + LCE->Height / 10.0f * 4.0f, LCE->Width / 10.0f * 2.0f, LCE->Height / 20.0f,
				"Cancel", 0, 0, LCE->Width / 10.0f * 2.0f, LCE->Height / 20.0f, 0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);

		LCE->Button_Confirm->CenterText = true;
		LCE->Button_Cancel->CenterText = true;
	}

	LCE->Button_Confirm->TextOffSetX = LCE->Width / 50.0f;

	Font_HeightRenderCA(DefaultFontManager, "Confirm you want to delete the file", LCE->X + LCE->Width / 10.0f * 5.0f, LCE->Y + LCE->Height / 10.0f * 6.0f, LCE->Height / 25.0f, 1.0f, COLOR_RED);

	if(Gui_Button_Render(LCE->Button_Confirm))
	{
		struct File_Tab_File *FTF = ((struct File_Tab_File *)LCE->File_Tab_List->items) + LCE->File_Tab_Index;
		char *Path = FileExternal_GetFullPath(FTF->FileName_Orig);
		FILE *f = fopen(Path, "rb");
		if (f) {
			fclose(f);
			remove(Path);
		}
		Close_File(FTF);
		vector_erase(LCE->File_Tab_List, LCE->File_Tab_Index);
		if (LCE->File_Tab_Index > 0) {
			LCE->File_Tab_Index--;

			FTF = ((struct File_Tab_File *) LCE->File_Tab_List->items)
					+ LCE->File_Tab_Index;
			for (int j = 0; j < LCE->TextBox_Name->ValueLength; j++)
				LCE->TextBox_Name->Value[j] = '\0';
			memcpy(LCE->TextBox_Name->Value, FTF->FileName_Edit,
					sizeof(char) * String_length(FTF->FileName_Edit));
		} else
			LCE->File_Tab_Index = -1;
		free(Path);

		LCE->STATE = STATE_DEFAULT;
	}

	Gui_Button_Render(LCE->Button_Cancel);
	if(LCE->Button_Cancel->Hover && Mouse.justReleased)
	{
		LCE->STATE = STATE_DEFAULT;
	}

	if(LCE->STATE != STATE_CONFIRMDELETEFILE)
	{
		Gui_Button_Free(&LCE->Button_Confirm);
		Gui_Button_Free(&LCE->Button_Cancel);
	}
}

static void
		Check_Input(struct Lua_Code_Editor *LCE);

void Lua_Code_Editor_Render(struct Lua_Code_Editor *LCE)
{
	check_mem(LCE);

	if (!Header_Render(LCE))
	{
		if(LCE->STATE == STATE_DEFAULT)
		{
			Lignes_Render(LCE);
			if(LCE->File_Tab_Index != -1) Check_Input(LCE);
		}
		else if(LCE->STATE == STATE_OPENFILE)
			OpenFile_Render(LCE);
		else if(LCE->STATE == STATE_NEWFILE)
			NewFile_Render(LCE);
		else if(LCE->STATE == STATE_CONFIRMCLOSEFILE)
			ConfirmClose_Render(LCE);
		else if(LCE->STATE == STATE_CONFIRMDELETEFILE)
			ConfirmDelete_Render(LCE);
	}

	return;
	error: return;
}

static void
		Check_CharacterTyped(struct Lua_Code_Editor *LCE);
static void
		Check_KeyPressed(struct Lua_Code_Editor *LCE);
void
		Check_Scrolled(struct Lua_Code_Editor *LCE);

void Check_Input(struct Lua_Code_Editor *LCE)
{
	Check_Scrolled(LCE);
	Check_CharacterTyped(LCE);
	Check_KeyPressed(LCE);
}

static void Remove_SelectedCheck(struct Lua_Code_Editor *LCE)
{
	struct File_Tab_File *File = ((struct File_Tab_File * ) LCE->File_Tab_List->items) + LCE->File_Tab_Index;

	if (!File->isTextSelected)
		return;
	int StartX = 0, StartY = 0, EndX = 0, EndY = 0;
	if (((File->Selecting_Data.x > File->Selecting_Data.z)
		 & (File->Selecting_Data.y == File->Selecting_Data.w))
		|| File->Selecting_Data.y > File->Selecting_Data.w)
	{
		StartX = (int) File->Selecting_Data.z;
		StartY = (int) File->Selecting_Data.w;
		EndX = (int) File->Selecting_Data.x;
		EndY = (int) File->Selecting_Data.y;
	}
	else
	{
		StartX = (int) File->Selecting_Data.x;
		StartY = (int) File->Selecting_Data.y;
		EndX = (int) File->Selecting_Data.z;
		EndY = (int) File->Selecting_Data.w;
	}
	File->isTextSelected = false;

	struct Lua_Code_Ligne *LCL = File->Editing->Code->items;

	if (StartY == EndY)
	{
		char *String = LCL[StartY].Ligne + StartX;
		int Length_toMove = String_length(String + EndX - StartX);
		String = memmove(String, String + EndX - StartX,
						 sizeof(char) * Length_toMove);
		String += Length_toMove;
		while (*String)
		{
			*String = '\0';
			String++;
		}
		File->Cursor_Pos.x = StartX;
		return;
	}

	int Length_toMove = String_length((LCL + EndY)->Ligne + EndX);
	if(Length_toMove)
	{
		(LCL + EndY)->Ligne = memmove((LCL + EndY)->Ligne,
									  (LCL + EndY)->Ligne + EndX, sizeof(char) * Length_toMove);
		(LCL + EndY)->Ligne[Length_toMove] = '\0';
	} else { // Delete ligne
		free((LCL + EndY)->Ligne);
		vector_erase(File->Editing->Code, EndY);
	}

	for (int i = EndY - 1; i > StartY; i--)
	{
		free((LCL + i)->Ligne);
		vector_erase(File->Editing->Code, i);
	}

	for (int i = StartX; i < CODE_MAX_LENGTH; i++)
		LCL[StartY].Ligne[i] = '\0';

	File->Cursor_Pos.y = StartY;
	File->Cursor_Pos.x = StartX;
	File->Selecting_Data.x = StartX;
	File->Selecting_Data.y = StartY;
	File->Selecting_Data.z = EndX;
	File->Selecting_Data.w = EndY;
}

static void Code_keyboardCopy(struct Lua_Code_Editor *LCE)
{
	struct File_Tab_File *File = ((struct File_Tab_File * ) LCE->File_Tab_List->items) + LCE->File_Tab_Index;

	if (!File->isTextSelected)
		return;

	int StartX = 0, StartY = 0, EndX = 0, EndY = 0;
	if (((File->Selecting_Data.x > File->Selecting_Data.z)
		 & (File->Selecting_Data.y == File->Selecting_Data.w))
		|| File->Selecting_Data.y > File->Selecting_Data.w)
	{
		StartX = (int) File->Selecting_Data.z;
		StartY = (int) File->Selecting_Data.w;
		EndX = (int) File->Selecting_Data.x;
		EndY = (int) File->Selecting_Data.y;
	}
	else
	{
		StartX = (int) File->Selecting_Data.x;
		StartY = (int) File->Selecting_Data.y;
		EndX = (int) File->Selecting_Data.z;
		EndY = (int) File->Selecting_Data.w;
	}

	struct Lua_Code_Ligne *LCL = File->Editing->Code->items;

	int Ligne_Num = EndY - StartY + 1;
	char *String = malloc(sizeof(char) * (Ligne_Num * (CODE_MAX_LENGTH + 1)));
	for(int i = 0; i < Ligne_Num * (CODE_MAX_LENGTH + 1); i++)
		String[i] = '\0';

	int String_On = 0;

	if(Ligne_Num == 1)
	{
		String = memcpy(String, LCL[StartY].Ligne + StartX, sizeof(char) * (EndX - StartX));
		String[EndX] = '\0';
	}
	else
	{
		for (int i = 0; i < Ligne_Num; i++)
		{
			char *S2 = String + String_On;
			if (i == 0)
			{
				S2 = memcpy(S2, LCL[StartY + i].Ligne + StartX,
							sizeof(char)
							* String_length(
									LCL[StartY + i].Ligne + StartX));
				S2[String_length(LCL[StartY + i].Ligne + StartX)] = '\n';
				String_On += String_length(LCL[StartY + i].Ligne + StartX) + 1;
			}
			else if (i + 1 == Ligne_Num)
			{
				S2 = memcpy(S2, LCL[StartY + i].Ligne, sizeof(char) * EndX);
				S2[EndX] = '\0';
				String_On += EndX + 1;
			}
			else
			{
				S2 = memcpy(S2, LCL[StartY + i].Ligne,
							sizeof(char) * String_length(LCL[StartY + i].Ligne));
				S2[String_length(LCL[StartY + i].Ligne)] = '\n';
				String_On += String_length(LCL[StartY + i].Ligne) + 1;
			}
		}
	}

	int Length = (String_length(String) + 1);
	char *String2 = malloc(sizeof(char) * Length);
	String2 = memcpy(String2, String,
					 sizeof(char) * Length);
	Clipboard_set(String2);
	free(String);
	free(String2);
}

static struct Vector2f Lua_Code_Addtext(struct Lua_Code_Editor *LCE,
										const char *String, int Length);
static void Delete_Lua_Code_Key(struct Lua_Code_Editor *LCE);

static void Code_keyboardPaste(struct Lua_Code_Editor *LCE)
{
	char *String = Clipboard_get();
	if (!String)
		return;

	struct File_Tab_File *File = ((struct File_Tab_File * ) LCE->File_Tab_List->items) + LCE->File_Tab_Index;

	if(File->isTextSelected)
		Delete_Lua_Code_Key(LCE);

	int Total_Length = String_length(String), Current_Length = 0;
	while (Current_Length < Total_Length)
	{
		int Length = 0;
		while(String[Current_Length + Length] == '\n')
			Current_Length++;

		while (String[Current_Length + Length] && String[Current_Length + Length] != '\n')
			Length++;

		File->Cursor_Pos = Lua_Code_Addtext(LCE, String + Current_Length, Length);

		Current_Length += Length;
	}
	free(String);
}

void Check_Scrolled(struct Lua_Code_Editor *LCE)
{
	if (!Mouse.justScrolled)
		return;

	if(!Mouse.Faked_Input && !Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), LCE->Ligne_Num) && !Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), LCE->Ligne_Text))
		return;

#ifdef ANDROID
	if (LCE->ScrollBar->IsPressed)
		return;
#endif

	struct File_Tab_File *File = ((struct File_Tab_File * ) LCE->File_Tab_List->items) + LCE->File_Tab_Index;

	if (!File->Editing->Code->size || !LCE->WritingCode)
		return;

	struct Vector2f Dimension = Vector2_Create(LCE->Ligne_Text.v2.y - LCE->Ligne_Height - LCE->Ligne_Height * (int) File->Editing->Code->size
			   - (float) File->ScrollValue / 480.0f * Game_Height,
			   LCE->Ligne_Text.v2.y - LCE->Ligne_Height * (int) File->Editing->Code->size
			   			   - (float) File->ScrollValue / 480.0f * Game_Height);

	if (Dimension.y >= LCE->Ligne_Text.v1.y && Mouse.scrollY < 0)
		return;

	File->ScrollValue += Mouse.scrollY * 4;
	if (File->ScrollValue > 0)
		File->ScrollValue = 0;

	LCE->VScrollBar->BarY = File->ScrollValue;
}

static void Code_Editor_CenterCursor(struct Lua_Code_Editor *LCE, struct File_Tab_File *File)
{
	struct Lua_Code_Ligne *Code =
			(struct Lua_Code_Ligne *) File->Editing->Code->items;
	float Line_DefaultStart = LCE->Ligne_Text.v2.x + (LCE->Ligne_Text.v3.x - LCE->Ligne_Text.v1.x) / 35.0f - LCE->ScrollBar->BarX;
	float Scroll =  File->ScrollValue / 480.0f * (float)Game_Height;

	float X = Font_HeightMaxCharacterPosition(
				DefaultFontManager, Code[(int)File->Cursor_Pos.y].Ligne,
				Line_DefaultStart, LCE->Ligne_Height, LCE->Ligne_Default_Width,
				(int)File->Cursor_Pos.x);
	float Low_Y = LCE->Ligne_Text.v2.y - LCE->Ligne_Height - LCE->Ligne_Height * (int)File->Cursor_Pos.y
			   - Scroll;
	float High_Y = Low_Y + LCE->Ligne_Height;

	/* If cursor is out of window, move the window to it */
	if(X < LCE->Ligne_Text.v1.x)
		LCE->ScrollBar->BarX -= LCE->Ligne_Text.v1.x - X;
	if(X > LCE->Ligne_Text.v3.x)
		LCE->ScrollBar->BarX += X - LCE->Ligne_Text.v3.x;

	if (Low_Y < LCE->Ligne_Text.v1.y)
	{
		float delta = LCE->Ligne_Text.v1.y - Low_Y;
		delta = delta / LCE->Height * 480.0f;
		LCE->VScrollBar->BarY -= delta;
	} else if(High_Y > LCE->Ligne_Text.v2.y)
	{
		float delta = High_Y - LCE->Ligne_Text.v2.y;
		delta = delta / LCE->Height * 480.0f;
		LCE->VScrollBar->BarY += delta;
	}
}

static void Check_CharacterTyped(struct Lua_Code_Editor *LCE)
{
	if(!LCE->WritingCode)
		return;

	struct File_Tab_File *File = ((struct File_Tab_File * ) LCE->File_Tab_List->items) + LCE->File_Tab_Index;

	if (!Keyboard.justTyped
		|| (((struct Lua_Code_Ligne *) File->Editing->Code->items)
			+ (int) File->Cursor_Pos.y)->Static)
		return;

	Remove_SelectedCheck(LCE);

	struct Lua_Code_Ligne *SS =
			((struct Lua_Code_Ligne *) File->Editing->Code->items)
			+ (int) File->Cursor_Pos.y;

	if (String_fulllength(SS->Ligne) == CODE_MAX_LENGTH)
		return;
	if (Keyboard.Character
		== '\t'&& String_fulllength(SS->Ligne) + 1 == CODE_MAX_LENGTH)
		return;

	int Length_toMove = String_length(SS->Ligne + (int ) File->Cursor_Pos.x);

	char *Temp = malloc(sizeof(char) * Length_toMove);
	Temp = memcpy(Temp, SS->Ligne + (int) File->Cursor_Pos.x,
        sizeof(char) * Length_toMove);
	char *Temp2 = SS->Ligne + (int) File->Cursor_Pos.x + 1;
	Temp2 = memcpy(Temp2, Temp, sizeof(char) * Length_toMove);
	free(Temp);
	SS->Ligne[(int) File->Cursor_Pos.x] = Keyboard.Character;
	File->Cursor_Pos.x++;

	Code_Editor_CenterCursor(LCE, File);
}

static void Delete_Lua_Code_Key(struct Lua_Code_Editor *LCE)
{
	struct File_Tab_File *File = ((struct File_Tab_File * ) LCE->File_Tab_List->items) + LCE->File_Tab_Index;

	if (File->isTextSelected)
	{
		struct Lua_Code_Ligne *SS =
				(struct Lua_Code_Ligne *) File->Editing->Code->items;
		int String_Length2 = String_length(
						SS[(int ) (
								File->Selecting_Data.y >= File->Selecting_Data.w ?
								File->Selecting_Data.y :
								File->Selecting_Data.w)].Ligne);
		Remove_SelectedCheck(LCE);

		if(File->Selecting_Data.y == File->Selecting_Data.w)
			return;

		int String_Length = String_length(SS[(int ) File->Cursor_Pos.y].Ligne),
				fullString_Length = String_fulllength(
				SS[(int) File->Cursor_Pos.y].Ligne);

		if (String_Length == CODE_MAX_LENGTH || !File->Selecting_Data.z
			|| String_Length2 == File->Selecting_Data.z)
			return;

		String_Length2 -= File->Selecting_Data.z;
		int String_full2 = String_fulllength(
				SS[(int) File->Cursor_Pos.y + 1].Ligne);

		if (fullString_Length + String_full2 >= CODE_MAX_LENGTH)
		{
			int String_canCopy = CODE_MAX_LENGTH - fullString_Length;
			char *Temp = SS[(int) File->Selecting_Data.y].Ligne + String_Length;
			Temp = memcpy(Temp, SS[(int) File->Selecting_Data.w].Ligne,
						  sizeof(char) * String_canCopy);
			Temp = SS[(int) File->Selecting_Data.w].Ligne;
			Temp = memcpy(Temp,
						  SS[(int) File->Selecting_Data.w + String_canCopy].Ligne,
						  sizeof(char)
						  * String_length(
								  SS[(int )File->Selecting_Data.w
									 + String_canCopy].Ligne));
			Temp[String_Length2 - String_canCopy] = '\0';
		}
		else
		{
			char *Temp = SS[(int) File->Selecting_Data.y].Ligne
						 + (int) File->Selecting_Data.x;
			Temp = memcpy(Temp, SS[(int) File->Selecting_Data.y + 1].Ligne,
						  sizeof(char) * String_Length2);
			vector_erase(File->Editing->Code, File->Selecting_Data.y + 1);
		}
		return;
	}

	struct Lua_Code_Ligne *SS =
			(struct Lua_Code_Ligne *) File->Editing->Code->items;
	int String_Length = String_length(SS[(int ) File->Cursor_Pos.y].Ligne),
			fullString_Length = String_fulllength(
			SS[(int) File->Cursor_Pos.y].Ligne);

	if (String_Length && File->Cursor_Pos.x != String_Length)
	{
		if (SS[(int) File->Cursor_Pos.y].Static)
			return;
		int Ligne_toCopy = String_length(
				SS[(int ) File->Cursor_Pos.y].Ligne + (int ) File->Cursor_Pos.x
				+ 1);
		char *Temp = malloc(sizeof(char) * Ligne_toCopy);
		Temp = memcpy(Temp,
					  SS[(int) File->Cursor_Pos.y].Ligne + (int) File->Cursor_Pos.x + 1,
					  sizeof(char) * Ligne_toCopy);
		char *Temp2 = SS[(int) File->Cursor_Pos.y].Ligne
					  + (int) File->Cursor_Pos.x;
		Temp2 = memcpy(Temp2, Temp, sizeof(char) * Ligne_toCopy);
		SS[(int) File->Cursor_Pos.y].Ligne[(int) File->Cursor_Pos.x + Ligne_toCopy] =
				'\0';
		free(Temp);
	}
	else if (!String_Length
			 && (int) File->Cursor_Pos.y < File->Editing->Code->size - 1)
		vector_erase(File->Editing->Code, (int) File->Cursor_Pos.y);
	else if (File->Cursor_Pos.x == String_Length
			 && fullString_Length != CODE_MAX_LENGTH && String_Length)
	{
		int Other_Length = String_length(
				(SS + (int ) File->Cursor_Pos.y + 1)->Ligne);
		if (SS[(int) File->Cursor_Pos.y].Static && Other_Length)
			return;
		if (SS[(int) File->Cursor_Pos.y + 1].Static)
			return;
		int String_canCopy = CODE_MAX_LENGTH - fullString_Length;
		if (String_canCopy > Other_Length)
			String_canCopy = Other_Length;

		char *Temp = malloc(sizeof(char) * String_canCopy);
		Temp = memcpy(Temp, SS[(int) File->Cursor_Pos.y + 1].Ligne,
					  sizeof(char) * String_canCopy);
		char *Temp2 = SS[(int) File->Cursor_Pos.y].Ligne + String_Length;
		Temp2 = memcpy(Temp2, Temp, sizeof(char) * String_canCopy);
		free(Temp);
		int oString_Length = String_length(
				(SS[(int ) File->Cursor_Pos.y + 1]).Ligne + String_canCopy);
		Temp = malloc(sizeof(char) * oString_Length);
		Temp = memcpy(Temp,
					  (SS[(int) File->Cursor_Pos.y + 1]).Ligne + String_canCopy,
					  oString_Length);
		(SS[(int) File->Cursor_Pos.y + 1]).Ligne = memcpy(
				(SS[(int) File->Cursor_Pos.y + 1]).Ligne, Temp,
				sizeof(char) * oString_Length);
		(SS[(int) File->Cursor_Pos.y + 1]).Ligne[oString_Length] = '\0';
		free(Temp);
		if (!String_length((SS[(int ) File->Cursor_Pos.y + 1]).Ligne)
			&& (int) File->Cursor_Pos.y < File->Editing->Code->size - 1)
			vector_erase(File->Editing->Code, (int) File->Cursor_Pos.y + 1);
	}
}

// Use by text paste feature
static struct Vector2f Lua_Code_Addtext(struct Lua_Code_Editor *LCE,
										const char * restrict String, int Length)
{
	struct File_Tab_File *File = ((struct File_Tab_File * ) LCE->File_Tab_List->items) + LCE->File_Tab_Index;

	struct Lua_Code_Ligne *LCL = (struct Lua_Code_Ligne *) File->Editing->Code->items;

	int Length2 = String_fulllength(LCL[(int) File->Cursor_Pos.y].Ligne), CharLength2 = String_length(LCL[(int)File->Cursor_Pos.y].Ligne);

	int FullLength = Length + String_numoftab_Length(String, Length), CharLength = Length;

	if (Length2 + FullLength <= CODE_MAX_LENGTH)
	{
		if((int)File->Cursor_Pos.x != CharLength2)
			memmove(LCL[(int) File->Cursor_Pos.y].Ligne + (int)File->Cursor_Pos.x + CharLength, LCL[(int) File->Cursor_Pos.y].Ligne + (int)File->Cursor_Pos.x, sizeof(char) * (CharLength2 - (int)File->Cursor_Pos.x));

		memcpy(LCL[(int) File->Cursor_Pos.y].Ligne +  (int)File->Cursor_Pos.x, String, sizeof(char) * Length);

		if (String[Length] != '\n')
			return Vector2_Create(File->Cursor_Pos.x + Length, File->Cursor_Pos.y);
		struct Lua_Code_Ligne Ligne;
		Ligne.Ligne = malloc(sizeof(char) * (CODE_MAX_LENGTH + 1));
		Ligne.Static = false;
		for (int i = 0; i < CODE_MAX_LENGTH + 1; i++)
			Ligne.Ligne[i] = '\0';

		memmove(Ligne.Ligne, LCL[(int) File->Cursor_Pos.y].Ligne + (int)File->Cursor_Pos.x + CharLength, sizeof(char) * CharLength);
		for(int i = 0; i < CODE_MAX_LENGTH; i++)
		{
			if(!LCL[(int) File->Cursor_Pos.y].Ligne[(int)File->Cursor_Pos.x + CharLength + i]) break;
			LCL[(int) File->Cursor_Pos.y].Ligne[(int)File->Cursor_Pos.x + CharLength + i] = '\0';
		}
		vector_insert(File->Editing->Code, (int) File->Cursor_Pos.y + 1,
					  &Ligne);
		return Vector2_Create(0, File->Cursor_Pos.y + 1);
	}
	else
	{
		if((int)File->Cursor_Pos.x != CharLength2)
		{
			if((int)File->Cursor_Pos.x + CharLength < CODE_MAX_LENGTH)
			{
				int Text_toKeep = CODE_MAX_LENGTH - ((int)File->Cursor_Pos.x + CharLength) - 1;
				int To_CopyDown = (CharLength + CharLength2) - CODE_MAX_LENGTH + 1;
				struct Lua_Code_Ligne Ligne;
				Ligne.Ligne = malloc(sizeof(char) * (CODE_MAX_LENGTH + 1));
				Ligne.Static = false;
				for (int i = 0; i < CODE_MAX_LENGTH + 1; i++)
					Ligne.Ligne[i] = '\0';
				Ligne.Ligne = memcpy(Ligne.Ligne, LCL[(int)File->Cursor_Pos.y].Ligne + (int)File->Cursor_Pos.x + Text_toKeep, sizeof(char) * To_CopyDown);
				vector_insert(File->Editing->Code, (int) File->Cursor_Pos.y + 1,
							  &Ligne);

				memmove(LCL[(int) File->Cursor_Pos.y ].Ligne + (int)File->Cursor_Pos.x + CharLength, LCL[(int) File->Cursor_Pos.y].Ligne + (int)File->Cursor_Pos.x, sizeof(char) * Text_toKeep);
				memcpy(LCL[(int) File->Cursor_Pos.y].Ligne + (int)File->Cursor_Pos.x, String, sizeof(char) * CharLength);
				return Vector2_Create(File->Cursor_Pos.x + CharLength, File->Cursor_Pos.y);
			} else {
				int Can_copy = CODE_MAX_LENGTH - (int)File->Cursor_Pos.x - 1;
				int Tocopy_after = CharLength - Can_copy;

				struct Lua_Code_Ligne Ligne;
				Ligne.Ligne = malloc(sizeof(char) * (CODE_MAX_LENGTH + 1));
				Ligne.Static = false;
				for (int i = 0; i < CODE_MAX_LENGTH + 1; i++)
					Ligne.Ligne[i] = '\0';

				memcpy(Ligne.Ligne + Tocopy_after, LCL[(int)File->Cursor_Pos.y].Ligne + (int)File->Cursor_Pos.x, sizeof(char) * Can_copy);
				memcpy(Ligne.Ligne, String + Can_copy, sizeof(char) * Tocopy_after);
				memcpy(LCL[(int)File->Cursor_Pos.y].Ligne + (int)File->Cursor_Pos.x, String, sizeof(char) * Can_copy);

				vector_insert(File->Editing->Code, (int) File->Cursor_Pos.y + 1,
							  &Ligne);

				return Vector2_Create(CODE_MAX_LENGTH, File->Cursor_Pos.y);
			}
		} else {
			int Can_copy = CODE_MAX_LENGTH - (int)File->Cursor_Pos.x - 1;
			int Tocopy_after = CharLength - Can_copy;

			struct Lua_Code_Ligne Ligne;
			Ligne.Ligne = malloc(sizeof(char) * (CODE_MAX_LENGTH + 1));
			Ligne.Static = false;
			for (int i = 0; i < CODE_MAX_LENGTH + 1; i++)
				Ligne.Ligne[i] = '\0';

			memcpy(Ligne.Ligne, String + Can_copy, sizeof(char) * Tocopy_after);
			memcpy(LCL[(int)File->Cursor_Pos.y].Ligne + (int)File->Cursor_Pos.x, String, sizeof(char) * Can_copy);

			vector_insert(File->Editing->Code, (int) File->Cursor_Pos.y + 1,
						  &Ligne);

			return Vector2_Create(Tocopy_after, File->Cursor_Pos.y + 1);
		}
	}
	return Vector2_Create(File->Cursor_Pos.x, File->Cursor_Pos.y);
}

static void Check_KeyPressed(struct Lua_Code_Editor *LCE)
{
	if (!Keyboard.justPressed || !LCE->WritingCode)
		return;

	struct File_Tab_File *File = ((struct File_Tab_File * ) LCE->File_Tab_List->items) + LCE->File_Tab_Index;

	if (File->Cursor_Pos.y > File->Editing->Code->size - 1)
		File->Cursor_Pos.y = File->Editing->Code->size - 1;
	if (File->Cursor_Pos.x > String_length( ((struct Lua_Code_Ligne * ) File->Editing->Code->items + (int ) File->Cursor_Pos.y)->Ligne))
	{
		File->Cursor_Pos.x = String_length(((struct Lua_Code_Ligne *) File->Editing->Code->items + (int) File->Cursor_Pos.y)->Ligne) - 1;
	}

	if (Keyboard.Key.key == GLFW_KEY_DOWN && File->Cursor_Pos.y + 1 < File->Editing->Code->size)
	{ // Down
		File->isTextSelected = false;
		File->Cursor_Pos.y++;
		LCE->Cursor_Timer = 100;

		if (LCE->XtoReset == -1)
		{
			LCE->XtoReset = File->Cursor_Pos.x + String_numoftab(((struct Lua_Code_Ligne *) File->Editing->Code->items)[(int) File->Cursor_Pos.y
																					- 1].Ligne);
		}
		File->Cursor_Pos.x = LCE->XtoReset - String_numoftab(((struct Lua_Code_Ligne *) File->Editing->Code->items)[(int) File->Cursor_Pos.y].Ligne);

		int Length = String_length(((struct Lua_Code_Ligne * ) File->Editing->Code->items)[(int ) File->Cursor_Pos.y].Ligne);
		if (File->Cursor_Pos.x > Length)
			File->Cursor_Pos.x = Length;
		else if (File->Cursor_Pos.x < 0)
			File->Cursor_Pos.x = 0;
	}
	else if (Keyboard.Key.key == GLFW_KEY_UP) // Up
	{
		if(!File->Cursor_Pos.y) return;
		File->isTextSelected = false;
		File->Cursor_Pos.y--;
		LCE->Cursor_Timer = 100;

		if (LCE->XtoReset == -1)
		{
			LCE->XtoReset = File->Cursor_Pos.x + String_numoftab(((struct Lua_Code_Ligne *) File->Editing->Code->items)[(int) File->Cursor_Pos.y
																					+ 1].Ligne);
		}
		File->Cursor_Pos.x = LCE->XtoReset - String_numoftab(((struct Lua_Code_Ligne *) File->Editing->Code->items)[(int) File->Cursor_Pos.y].Ligne);

		int Length = String_length(((struct Lua_Code_Ligne * ) File->Editing->Code->items)[(int ) File->Cursor_Pos.y].Ligne);
		if (File->Cursor_Pos.x > Length)
			File->Cursor_Pos.x = Length;
		else if (File->Cursor_Pos.x < 0)
			File->Cursor_Pos.x = 0;
	}
	else if (Keyboard.Key.key == GLFW_KEY_RIGHT)
	{ // Right
		File->isTextSelected = false;
		LCE->Cursor_Timer = 100;
		File->Cursor_Pos.x++;
		LCE->XtoReset = -1;

		char *Text =
				((struct Lua_Code_Ligne *) File->Editing->Code->items)[(int) File->Cursor_Pos.y].Ligne;
		int TextLength = String_length(Text);

		if (File->Cursor_Pos.x > TextLength)
		{
			if (File->Cursor_Pos.y + 1 < File->Editing->Code->size)
			{
				File->Cursor_Pos.x = 0;
				File->Cursor_Pos.y++;
			}
			else
				File->Cursor_Pos.x--;
		}
	}
	else if (Keyboard.Key.key == GLFW_KEY_LEFT) // Left
	{
		File->isTextSelected = false;
		LCE->Cursor_Timer = 100;
		File->Cursor_Pos.x--;
		LCE->XtoReset = -1;

		if (File->Cursor_Pos.x < 0)
		{
			if (!File->Cursor_Pos.y)
			{
				File->Cursor_Pos.x = 0;
			}
			else
			{
				File->Cursor_Pos.y--;
				char *Text =
						((struct Lua_Code_Ligne *) File->Editing->Code->items)[(int) File->Cursor_Pos.y].Ligne;
				int TextLength = 0;
				while (Text[TextLength])
					TextLength++;
				File->Cursor_Pos.x = TextLength;
			}
		}
	}

	else if (Keyboard.Key.key == GLFW_KEY_BACKSPACE)
	{ // Backspace
		LCE->Cursor_Timer = 100;
		if (File->isTextSelected)
		{
			Delete_Lua_Code_Key(LCE);
		}
		else
		{
			struct Lua_Code_Ligne *SS =
					(struct Lua_Code_Ligne *) File->Editing->Code->items;
			int Total_String = String_length(
					SS[(int ) File->Cursor_Pos.y].Ligne);
			if (Total_String && File->Cursor_Pos.x)
			{
				if (SS[(int) File->Cursor_Pos.y].Static)
					return;
				// Delete character if Cursor is in ligne and ligne has characters to delete
				int Length = String_length(
						SS[(int ) File->Cursor_Pos.y].Ligne
						+ (int ) File->Cursor_Pos.x);
				char *TempString = malloc(sizeof(char) * Length);
				TempString = memcpy(TempString,
									SS[(int) File->Cursor_Pos.y].Ligne
									+ (int) File->Cursor_Pos.x,
									sizeof(char) * Length);
				SS[(int) File->Cursor_Pos.y].Ligne[String_length(
														  SS[(int) File->Cursor_Pos.y].Ligne) - 1] = '\0';
				File->Cursor_Pos.x--;
				char *ReTempString =
						&SS[(int) File->Cursor_Pos.y].Ligne[(int) File->Cursor_Pos.x];
				ReTempString = memcpy(ReTempString, TempString,
									  sizeof(char) * Length);
				free(TempString);
			}
			else if (!File->Cursor_Pos.x && Total_String && File->Cursor_Pos.y)
			{
				if ((SS + (int) File->Cursor_Pos.y - 1)->Static)
					return;
				if ((SS + (int) File->Cursor_Pos.y)->Static
					&& String_length(
							(SS + (int ) File->Cursor_Pos.y - 1)->Ligne))
					return;
				int Other_Ligne = String_length(
						(SS + (int ) File->Cursor_Pos.y - 1)->Ligne);
				if (String_fulllength((SS + (int) File->Cursor_Pos.y - 1)->Ligne)
					+ String_fulllength(
						SS[(int) File->Cursor_Pos.y].Ligne) > CODE_MAX_LENGTH)
				{
					int Length_To_Copy =
							CODE_MAX_LENGTH
							- String_fulllength(
									(SS + (int) File->Cursor_Pos.y - 1)->Ligne);
					char *String = (SS + (int) File->Cursor_Pos.y - 1)->Ligne
								   + Other_Ligne;
					String = memcpy(String,
									(SS + (int) File->Cursor_Pos.y)->Ligne,
									sizeof(char) * Length_To_Copy);
					char *Temp = malloc(
							sizeof(char) * (Total_String - Length_To_Copy));
					Temp = memcpy(Temp,
								  (SS + (int) File->Cursor_Pos.y)->Ligne
								  + Length_To_Copy,
								  sizeof(char) * (Total_String - Length_To_Copy));
					(SS + (int) File->Cursor_Pos.y)->Ligne = memcpy(
							(SS + (int) File->Cursor_Pos.y)->Ligne, Temp,
							sizeof(char) * (Total_String - Length_To_Copy));
					(SS + (int) File->Cursor_Pos.y)->Ligne[(Total_String
														   - Length_To_Copy)] = '\0';
					free(Temp);
					File->Cursor_Pos.y--;
					File->Cursor_Pos.x = Other_Ligne;
				}
				else
				{
					char *String = (SS + (int) File->Cursor_Pos.y - 1)->Ligne
								   + Other_Ligne;
					String = memcpy(String,
									(SS + (int) File->Cursor_Pos.y)->Ligne,
									sizeof(char) * Total_String);
					(SS + (int) File->Cursor_Pos.y - 1)->Static = (SS
																  + (int) File->Cursor_Pos.y)->Static;
					free((SS + (int) File->Cursor_Pos.y)->Ligne);
					vector_erase(File->Editing->Code, (int) File->Cursor_Pos.y);
					File->Cursor_Pos.x = Other_Ligne;
					File->Cursor_Pos.y--;
				}
			}
			else if (!File->Cursor_Pos.x && !Total_String && File->Cursor_Pos.y)
			{
				free((SS + (int) File->Cursor_Pos.y)->Ligne);
				vector_erase(File->Editing->Code, (int) File->Cursor_Pos.y);
				File->Cursor_Pos.x = String_length(
						(SS + (int ) --File->Cursor_Pos.y)->Ligne);
			}
		}
	}

	else if (Keyboard.Key.key == GLFW_KEY_DELETE) // Delete
	{
		LCE->Cursor_Timer = 100;
		Delete_Lua_Code_Key(LCE);
	}

	else if (Keyboard.Key.key == GLFW_KEY_ENTER) // Enter
	{
		LCE->Cursor_Timer = 100;
		if (File->isTextSelected)
		{
			Remove_SelectedCheck(LCE);
			File->Cursor_Pos.x = 0;
			File->Cursor_Pos.y++;
		}
		else
		{
			struct Lua_Code_Ligne *SS =
					(struct Lua_Code_Ligne *) File->Editing->Code->items;
			struct Lua_Code_Ligne *Ligne = SS + (int) File->Cursor_Pos.y;
			int Code_Length = String_length(Ligne->Ligne);

			if ((int) File->Cursor_Pos.x == Code_Length || !Code_Length)
			{
				if ((int)File->Cursor_Pos.y + 1 != File->Editing->Code->size &&SS[(int) File->Cursor_Pos.y + 1].Static)
					return;
				struct Lua_Code_Ligne *SS2 = malloc(
						sizeof(struct Lua_Code_Ligne));
				SS2->Ligne = malloc(sizeof(char) * (CODE_MAX_LENGTH + 1));
				for (int i = 0; i < (CODE_MAX_LENGTH + 1); i++)
				{
					SS2->Ligne[i] = '\0';
				}
				SS2->Static = false;
				vector_insert(File->Editing->Code, (int) File->Cursor_Pos.y + 1,
							  SS2);
				free(SS2);
				File->Cursor_Pos.y++;
				File->Cursor_Pos.x = 0;
			}
			else
			{
				if (SS[(int) File->Cursor_Pos.y].Static)
					return;
				int Length_Copy = String_length(
						Ligne->Ligne + (int ) File->Cursor_Pos.x);
				struct Lua_Code_Ligne *SS2 = malloc(
						sizeof(struct Lua_Code_Ligne));
				SS2->Ligne = malloc(sizeof(char) * (CODE_MAX_LENGTH + 1));
				for (int i = 0; i < (CODE_MAX_LENGTH + 1); i++)
				{
					SS2->Ligne[i] = '\0';
				}
				SS2->Ligne = memcpy(SS2->Ligne,
									(Ligne->Ligne + (int) File->Cursor_Pos.x),
									sizeof(char) * Length_Copy);
				for(int i = (int) File->Cursor_Pos.x; i < CODE_MAX_LENGTH; i++)
				{
					if(!Ligne->Ligne[i])
						break;
					Ligne->Ligne[i] = '\0';
				}
				SS2->Static = Ligne->Static;
				Ligne->Static = false;
				vector_insert(File->Editing->Code, (int) File->Cursor_Pos.y + 1,
							  SS2);
				free(SS2);
				File->Cursor_Pos.x = 0;
				File->Cursor_Pos.y++;
			}
		}
	}

	else if (Keyboard.Key.key == GLFW_KEY_TAB) // Tab
	{
		Keyboard.justTyped = true;
		Keyboard.Character = '\t';
		Check_CharacterTyped(LCE);
		Keyboard.justTyped = false;
	}

	else if (Keyboard.Key.key == GLFW_KEY_C && Keyboard.Key.mods == 2) // Control C
	{
		Code_keyboardCopy(LCE);
		return;
	}

	else if (Keyboard.Key.key == GLFW_KEY_X && Keyboard.Key.mods == 2) // Control X
	{
		Code_keyboardCopy(LCE);
		Delete_Lua_Code_Key(LCE);
		return;
	}

	else if (Keyboard.Key.key == GLFW_KEY_V && Keyboard.Key.mods == 2) // Control V
	{
		Code_keyboardPaste(LCE);

		return;
	}

	else if (Keyboard.Key.key == GLFW_KEY_D && Keyboard.Key.mods == 2) // Control D
	{
		log_info("Debug : ScrollValue : %f", File->ScrollValue);
		log_info("Debug : %f", (LCE->Ligne_Text.v2.y - LCE->Ligne_Text.v1.y) / LCE->Ligne_Height);
		log_info("Debug : %f", LCE->Ligne_Height);
		log_info("Debug : %f", (LCE->Ligne_Text.v2.y - LCE->Ligne_Text.v1.y) / (float)LCE->LignesInWindow);
		return;
	}

	else
	{
		log_info("%i %i", Keyboard.Key.key, Keyboard.Key.mods);
		return;
	}

	Code_Editor_CenterCursor(LCE, File);
}

void Lua_Code_Editor_Free(struct Lua_Code_Editor **LCE)
{
	check_mem((*LCE));

	Image_Free((*LCE)->Confirm_Text);
	Image_Free((*LCE)->Close_Text);
	Image_Free((*LCE)->Launch_Text);
	Image_Free((*LCE)->Save_Text);
	Image_Free((*LCE)->New_Text);
	Image_Free((*LCE)->CopyPaste_Text);
	Image_Free((*LCE)->Load_Text);

	Gui_Horizontal_ScrollBar_Free(&(*LCE)->ScrollBar);
	Gui_Vertical_ScrollBar_Free(&(*LCE)->VScrollBar);

	for(int i = 0; i < (*LCE)->File_Tab_List->size; i++)
	{
		struct File_Tab_File *File = ((struct File_Tab_File * ) (*LCE)->File_Tab_List->items) + i;
		struct Lua_Code_Ligne *SS =
				(struct Lua_Code_Ligne *) File->Editing->Code->items;
		for (int i = 0; i < File->Editing->Code->size; i++)
			free(SS[i].Ligne);
	}
	vector_delete((*LCE)->File_Tab_List);

	for (int i = 0; i < (*LCE)->ReserveWords_Size; i++)
	{
		free((*LCE)->ReserveWords[i].String);
	}
	free((*LCE)->ReserveWords);

	free((*LCE));
	(*LCE) = NULL;

	return;
	error: return;
}

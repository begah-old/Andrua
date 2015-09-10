/*
 * Sprite_Editor.c
 *
 *  Created on: Jun 12, 2015
 *      Author: begah
 */

#include "Image_Manager.h"

#define STATE_DEFAULT 0
#define STATE_NEWIMAGE 1
#define STATE_OPENIMAGE 2
#define STATE_CONFIRMCLOSE 3
#define STATE_CONFIRMDELETE 4

#define IMAGE_TAB_MAXWIDTH (IE->Editor_Dimensions.z / 4.0f)

struct Image_Tab *Image_Tab_Load(const char *Name)
{
	struct Image_Tab *Image = malloc(sizeof(struct Image_Tab));
	Image->Name_Edit = malloc(sizeof(char) * (String_length(Name) + 1));
	Image->Name_Original = malloc(sizeof(char) * (String_length(Name) + 1));

	Image->Name_Edit = memcpy(Image->Name_Edit, Name, sizeof(char) * (String_length(Name) + 1));
	Image->Name_Original = memcpy(Image->Name_Original, Name, sizeof(char) * (String_length(Name) + 1));

	Image->Image_Original = Image_loadPNG(Name);

	Image->Image_Edit = malloc(sizeof(struct Image_RawData));
	Image->Image_Edit->Data = malloc(sizeof(unsigned char) * Image->Image_Original->Width * Image->Image_Original->Height * 4);
	Image->Image_Edit->Data = memcpy(Image->Image_Edit->Data, Image->Image_Original->Data, sizeof(unsigned char) * Image->Image_Original->Width * Image->Image_Original->Height * 4);
	Image->Image_Edit->Width = Image->Image_Original->Width;
	Image->Image_Edit->Height = Image->Image_Original->Height;

	Image->BarY = 0;
	Image->BarValue = MIN(Image->Image_Edit->Width, Image->Image_Edit->Height) / 100.0f;
	Image->X = Image->Y = 0;
	/*Image->View.x = Image->View.y = 0;
	Image->View.z = Image->Image_Original->Width;
	Image->View.w = Image->Image_Original->Height;*/

	return Image;
}

void Image_Tab_Save(struct Image_Tab *ITL)
{
	if(strcmp(ITL->Name_Edit, ITL->Name_Original))
	{
		// Renamed file
		char *C = FileExternal_GetFullPath(ITL->Name_Original);
		remove(C);
		free(C);
		free(ITL->Name_Original);
		ITL->Name_Original = malloc(sizeof(char) * (String_length(ITL->Name_Edit) + 1));
		ITL->Name_Original = memcpy(ITL->Name_Original, ITL->Name_Edit, sizeof(char) * (String_length(ITL->Name_Edit) + 1));
	}

	char *c = FileExternal_GetFullPath(ITL->Name_Edit);

	SOIL_save_image(c, SOIL_SAVE_TYPE_PNG, ITL->Image_Edit->Width,
						ITL->Image_Edit->Height, SOIL_LOAD_RGBA,
						ITL->Image_Edit->Data);
	free(c);
	free(ITL->Image_Original->Data);
	free(ITL->Image_Original);

	ITL->Image_Original = malloc(sizeof(struct Image_RawData));
	ITL->Image_Original->Data = malloc(sizeof(unsigned char) * ITL->Image_Edit->Width * ITL->Image_Edit->Height * 4);
	ITL->Image_Original->Data = memcpy(ITL->Image_Original->Data, ITL->Image_Edit->Data, sizeof(unsigned char) * ITL->Image_Edit->Width * ITL->Image_Edit->Height * 4);
	ITL->Image_Original->Width = ITL->Image_Edit->Width;
	ITL->Image_Original->Height = ITL->Image_Edit->Height;
}

_Bool Image_Tab_isDirty(struct Image_Tab *ITL)
{
	int cmp = strcmp(ITL->Name_Edit, ITL->Name_Original);
	if(cmp)
		return true;

	if(ITL->Image_Edit->Height != ITL->Image_Original->Height)
		return true;
	if(ITL->Image_Edit->Width != ITL->Image_Original->Width)
		return true;

	for(int i = 0; i < ITL->Image_Edit->Width * ITL->Image_Edit->Height * 4; i++)
		if(ITL->Image_Edit->Data[i] != ITL->Image_Original->Data[i])
			return true;

	return false;
}

void Image_Tab_Close(struct Image_Tab *ITL)
{
	free(ITL->Name_Edit);
	free(ITL->Name_Original);
	free(ITL->Image_Edit->Data);
	free(ITL->Image_Original->Data);
	free(ITL->Image_Edit);
	free(ITL->Image_Original);
}

static void Image_ScrollBar_Scrolling(void *Data, float Y)
{
	struct Image_Editor *IE = Data;
	struct Image_Tab *IT = ((struct Image_Tab *)IE->Image_Tab_List->items) + IE->Image_Tab_Index;

	Y = Y / (IE->Image_Scroll->ScrollBar.v2.y - IE->Image_Scroll->ScrollBar.v1.y) * IE->Image_Scroll->TotalValue;

	if (Y < 0)
	{
		Y = -Y;
		float oWidth = (IT->Image_Edit->Width / 100.0f) * IT->BarY, oHeight = (IT->Image_Edit->Height / 100.0f) * IT->BarY;
		float nWidth = (IT->Image_Edit->Width / 100.0f) * (IT->BarY - Y), nHeight = (IT->Image_Edit->Height / 100.0f) * (IT->BarY - Y);

		IT->X += (oWidth - nWidth) / 2.0f;
		IT->Y += (oHeight - nHeight) / 2.0f;
	} else if (Y > 0) {
		float oWidth = (IT->Image_Edit->Width / 100.0f) * IT->BarY, oHeight = (IT->Image_Edit->Height / 100.0f) * IT->BarY;
		float nWidth = (IT->Image_Edit->Width / 100.0f) * (IT->BarY + Y), nHeight = (IT->Image_Edit->Height / 100.0f) * (IT->BarY + Y);

		IT->X += (oWidth - nWidth) / 2.0f;
		IT->Y += (oHeight - nHeight) / 2.0f;
	}
}

static void Image_Editor_Resize(struct Image_Editor *IE)
{
	IE->Image_Editor_View = Quad_Create(
			IE->Editor_Dimensions.z / 10 + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.z / 10 + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w / 10 * 9 + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.z / 10 * 8 + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w / 10 * 9 + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.z / 10 * 8 + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.y);

	IE->Change_Confirm = Quad_Create(IE->Editor_Dimensions.z / 10.0f * 0.0f + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w / 20.0f * 19.0f + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.z / 10.0f * 0.0f + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.z / 10.0f * 1.0f + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.z / 10.0f * 1.0f + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w / 20.0f * 19.0f + IE->Editor_Dimensions.y);

	IE->Button_Save = Quad_Create(IE->Editor_Dimensions.z / 10.0f * 1.50f + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w / 20.0f * 19.0f + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.z / 10.0f * 1.50f + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.z / 10.0f * 2.0f + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.z / 10.0f * 2.0f + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w / 20.0f * 19.0f + IE->Editor_Dimensions.y);

	IE->Button_Load = Quad_Create(IE->Editor_Dimensions.z / 10.0f * 2.0f + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w / 20.0f * 19.0f + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.z / 10.0f * 2.0f + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.z / 10.0f * 3.0f + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.z / 10.0f * 3.0f + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w / 20.0f * 19.0f + IE->Editor_Dimensions.y);

	IE->Close = Quad_Create(
			IE->Editor_Dimensions.z / 10 * 9 + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w / 20.0f * 19.0f + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.z / 10 * 9 + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.z + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.z + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w / 20.0f * 19.0f + IE->Editor_Dimensions.y);

	IE->Delete = Quad_Create(
			IE->Editor_Dimensions.z / 10 * 8 + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w / 20.0f * 19.0f + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.z / 10 * 8 + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.z / 10 * 9 + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.z / 10 * 9 + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w / 20.0f * 19.0f + IE->Editor_Dimensions.y);

	IE->New = Quad_Create(
			IE->Editor_Dimensions.z / 10 + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w / 20.0f * 19.0f + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.z / 10 + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.z / 10 * 1.5f + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.z / 10 * 1.5f + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w / 20.0f * 19.0f + IE->Editor_Dimensions.y);

	IE->Image_Tab = Quad_Create(IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w / 20.0f * 18.0f + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w / 20.0f * 19.0f + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.z + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w / 20.0f * 19.0f + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.z + IE->Editor_Dimensions.x,
			IE->Editor_Dimensions.w / 20.0f * 18.0f + IE->Editor_Dimensions.y);

	IE->Button_PanView = Quad_Create(IE->Editor_Dimensions.x,
			(float) IE->Editor_Dimensions.w / 10 * 6 + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.x,
			(float) IE->Editor_Dimensions.w / 10 * 7 + IE->Editor_Dimensions.y,
			(float) IE->Editor_Dimensions.z / 10 + IE->Editor_Dimensions.x,
			(float) IE->Editor_Dimensions.w / 10 * 7 + IE->Editor_Dimensions.y,
			(float) IE->Editor_Dimensions.z / 10 + IE->Editor_Dimensions.x,
			(float) IE->Editor_Dimensions.w / 10 * 6 + IE->Editor_Dimensions.y);

	IE->Button_Pencil = Quad_Create(IE->Editor_Dimensions.x,
			(float) IE->Editor_Dimensions.w / 10 * 5 + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.x,
			(float) IE->Editor_Dimensions.w / 10 * 6 + IE->Editor_Dimensions.y,
			(float) IE->Editor_Dimensions.z / 10 + IE->Editor_Dimensions.x,
			(float) IE->Editor_Dimensions.w / 10 * 6 + IE->Editor_Dimensions.y,
			(float) IE->Editor_Dimensions.z / 10 + IE->Editor_Dimensions.x,
			(float) IE->Editor_Dimensions.w / 10 * 5 + IE->Editor_Dimensions.y);

	IE->Button_PickColor = Quad_Create(IE->Editor_Dimensions.x,
			(float) IE->Editor_Dimensions.w / 10 * 4 + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.x,
			(float) IE->Editor_Dimensions.w / 10 * 5 + IE->Editor_Dimensions.y,
			(float) IE->Editor_Dimensions.z / 10 + IE->Editor_Dimensions.x,
			(float) IE->Editor_Dimensions.w / 10 * 5 + IE->Editor_Dimensions.y,
			(float) IE->Editor_Dimensions.z / 10 + IE->Editor_Dimensions.x,
			(float) IE->Editor_Dimensions.w / 10 * 4 + IE->Editor_Dimensions.y);

	IE->Button_Fill = Quad_Create(IE->Editor_Dimensions.x,
			(float) IE->Editor_Dimensions.w / 10 * 3 + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.x,
			(float) IE->Editor_Dimensions.w / 10 * 4 + IE->Editor_Dimensions.y,
			(float) IE->Editor_Dimensions.z / 10 + IE->Editor_Dimensions.x,
			(float) IE->Editor_Dimensions.w / 10 * 4 + IE->Editor_Dimensions.y,
			(float) IE->Editor_Dimensions.z / 10 + IE->Editor_Dimensions.x,
			(float) IE->Editor_Dimensions.w / 10 * 3 + IE->Editor_Dimensions.y);

	Gui_TextBox_Resize(IE->Color_Red, (float) IE->Editor_Dimensions.z / 10 * 8 + IE->Editor_Dimensions.x,
				(float) IE->Editor_Dimensions.w / 10 * 4 + IE->Editor_Dimensions.y, (float) IE->Editor_Dimensions.z / 10 * 2,
				(float) IE->Editor_Dimensions.w / 10.0f);
	check_mem(IE->Color_Red);
	Gui_TextBox_Resize(IE->Color_Green, (float) IE->Editor_Dimensions.z / 10 * 8 + IE->Editor_Dimensions.x,
				(float) IE->Editor_Dimensions.w / 10 * 3 + IE->Editor_Dimensions.y, (float) IE->Editor_Dimensions.z / 10 * 2,
				(float) IE->Editor_Dimensions.w / 10.0f);
	check_mem(IE->Color_Green);
	Gui_TextBox_Resize(IE->Color_Blue, (float) IE->Editor_Dimensions.z / 10 * 8 + IE->Editor_Dimensions.x,
				(float) IE->Editor_Dimensions.w / 10 * 2 + IE->Editor_Dimensions.y, (float) IE->Editor_Dimensions.z / 10 * 2,
				(float) IE->Editor_Dimensions.w / 10.0f);
	check_mem(IE->Color_Blue);
	Gui_TextBox_Resize(IE->Color_Alpha, (float) IE->Editor_Dimensions.z / 10 * 8 + IE->Editor_Dimensions.x,
				(float) IE->Editor_Dimensions.w / 10 * 1 + IE->Editor_Dimensions.y, (float) IE->Editor_Dimensions.z / 10 * 2,
				(float) IE->Editor_Dimensions.w / 10.0f);
	check_mem(IE->Color_Alpha);

	IE->Color_Red->TextHeight = IE->Color_Green->TextHeight = IE->Color_Blue->TextHeight = IE->Color_Alpha->TextHeight = (float) IE->Editor_Dimensions.w / 10.0f;
	IE->Color_Red->TextMaxWidth = IE->Color_Green->TextMaxWidth = IE->Color_Blue->TextMaxWidth = IE->Color_Alpha->TextMaxWidth = (float) IE->Editor_Dimensions.z / 10.0f * 2.0f;
	IE->Color_Red->Quad_takeMouse = IE->Color_Blue->Quad_takeMouse = IE->Color_Green->Quad_takeMouse = IE->Color_Alpha->Quad_takeMouse = IE->Name->Quad_takeMouse = Quad_Create(IE->Editor_Dimensions.x, IE->Editor_Dimensions.y, IE->Editor_Dimensions.x, IE->Editor_Dimensions.y + IE->Editor_Dimensions.w, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z, IE->Editor_Dimensions.y + IE->Editor_Dimensions.w, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z, IE->Editor_Dimensions.y);

	Gui_TextBox_Resize(IE->Name, IE->Editor_Dimensions.z / 6 * 2.4f + IE->Editor_Dimensions.x, IE->Editor_Dimensions.w / 20.0f * 19.0f + IE->Editor_Dimensions.y,
			IE->Editor_Dimensions.z / 6 * 2.4f, IE->Editor_Dimensions.w / 20.0f);
	check_mem(IE->Name);
	IE->Name->TextHeight = IE->Editor_Dimensions.w / 20.0f;
	IE->Name->TextMaxWidth = IE->Editor_Dimensions.z / 6.0f * 2.4f;

	IE->Keyboard_Show = Quad_Create(IE->Editor_Dimensions.x, IE->Editor_Dimensions.y, IE->Editor_Dimensions.x, IE->Editor_Dimensions.y + IE->Editor_Dimensions.z / 20.0f, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 10.0f, IE->Editor_Dimensions.y + IE->Editor_Dimensions.z / 20.0f, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 10.0f, IE->Editor_Dimensions.y);

	if(!IE->Image_Scroll) {
		IE->Image_Scroll = Gui_Vertical_ScrollBar_Create((float) IE->Editor_Dimensions.z / 10.0f * 8.5f + IE->Editor_Dimensions.x,
				(float) IE->Editor_Dimensions.w / 10.0f * 5.5f + IE->Editor_Dimensions.y, IE->Editor_Dimensions.z / 20.0f, IE->Editor_Dimensions.w / 10.0f * 3.0f, Vector4_Create(0.0f, 0.8f, 0.8f, 1.0f),
				Vector4_Create(0.7f, 0.7f, 0.7f, 1.0f),
				Vector4_Create(0.6f, 0.6f, 0.6f, 1.0f), 1, 100, Image_ScrollBar_Scrolling);
		check_mem(IE->Image_Scroll);
		IE->Image_Scroll->Data = IE;
		Gui_Vertical_ScrollBar_Inverse(IE->Image_Scroll);
	} else {
		 Gui_Vertical_ScrollBar_Resize(IE->Image_Scroll, (float) IE->Editor_Dimensions.z / 10.0f * 8.5f + IE->Editor_Dimensions.x,
				(float) IE->Editor_Dimensions.w / 10.0f * 5.5f + IE->Editor_Dimensions.y, IE->Editor_Dimensions.z / 20.0f, IE->Editor_Dimensions.w / 10.0f * 3.0f, 1, 100);

		if(IE->Image_Tab_Index != -1)
		{
			struct Image_Tab *IT = ((struct Image_Tab *)IE->Image_Tab_List->items) + IE->Image_Tab_Index;
			float min = IT->Image_Edit->Width < IT->Image_Edit->Height ? IT->Image_Edit->Width : IT->Image_Edit->Height;
			IE->Image_Scroll->BarValue = min / 100.0f;
		}
	}

	return;
	error:exit(1);
}

static void Image_Set(struct Image_Editor *IE, int INDEX)
{
	if(INDEX >= (int)IE->Image_Tab_List->size)
		return;

	if(IE->Image_Tab_Index != -1)
	{
		struct Image_Tab *IT = ((struct Image_Tab *)IE->Image_Tab_List->items) + IE->Image_Tab_Index;
		free(IT->Name_Edit);
		IT->Name_Edit = malloc(sizeof(char) * (String_length(IE->Name->Value) + 1));
		IT->Name_Edit = memcpy(IT->Name_Edit, IE->Name->Value, sizeof(char) * (String_length(IE->Name->Value) + 1));
	}

	IE->Image_Tab_Index = INDEX;

	if(INDEX != -1)
	{
		struct Image_Tab *IT = ((struct Image_Tab *)IE->Image_Tab_List->items) + IE->Image_Tab_Index;

		for(int i = 0; i < IE->Name->ValueLength; i++)
			IE->Name->Value[i] = '\0';
		memcpy(IE->Name->Value, IT->Name_Edit, sizeof(char) * String_length(IT->Name_Edit));

		Image_Free(IE->Image_Texture);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &IE->Image_Texture);
		glBindTexture(GL_TEXTURE_2D, IE->Image_Texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, IT->Image_Edit->Width,
				IT->Image_Edit->Height, 0, GL_RGBA,
				GL_UNSIGNED_BYTE, IT->Image_Edit->Data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		/*IT->View.x = IT->View.y = 0;
		IT->View.z = IT->Image_Edit->Width;
		IT->View.w = IT->Image_Edit->Height;*/
		IE->Image_Scroll->BarY = -IT->BarY;
		IE->Image_Scroll->BarValue = IT->BarValue;
		IE->Image_Scroll->TotalValue = 100.0f + IE->Image_Scroll->BarValue;
	}
}

static void Clear_State(struct Image_Editor *IE, int STATE_TO)
{
	if(IE->State == STATE_TO) return;

	if(IE->State == STATE_CONFIRMCLOSE || IE->State == STATE_CONFIRMDELETE)
	{
		Gui_Button_Free(&IE->Confirm_Button);
		Gui_Button_Free(&IE->Cancel_Button);
	} else if(IE->State != STATE_DEFAULT)
	{
		if(IE->Buttons)
		{
			struct Gui_Button *B = IE->Buttons->items;
			for(int i = 0; i < IE->Buttons->size; i++)
				Gui_Button_Free_Simple(B + i);
			vector_delete(IE->Buttons);
			IE->Buttons = NULL;
		}
		if(IE->TextBoxs)
		{
			struct Gui_TextBox *T = IE->TextBoxs->items;
			for(int i = 0; i < IE->TextBoxs->size; i++)
				Gui_TextBox_Free_Simple(T + i);
			vector_delete(IE->TextBoxs);
			IE->TextBoxs = NULL;
		}
	}

	IE->State = STATE_TO;
}

struct Image_Editor *Image_Editor_Init(float x, float y, float width, float height)
{
	struct Image_Editor *IE = malloc(sizeof(struct Image_Editor));
	check_mem(IE);

	IE->Editor_Dimensions.x = x;
	IE->Editor_Dimensions.y = y;
	IE->Editor_Dimensions.z = width;
	IE->Editor_Dimensions.w = height;

	IE->Image_Tab_Scroll = 0.0f;

	IE->Image_Scroll = NULL;

	IE->Hover_addColor = Vector4_Create(-0.3f, -0.3f, -0.3f, 0.0f);

	IE->Button_PencilTexture = Image_Load("Image_Editor/Pencil.png");
	check(IE->Button_PencilTexture != -1, "Couldn't load PencilTexture");

	IE->Button_PickColorTexture = Image_Load("Image_Editor/PickColor.png");
	check(IE->Button_PickColorTexture != -1, "Couldn't load PickColorTexture");

	IE->Button_FillTexture = Image_Load("Image_Editor/Bucket.png");
	check(IE->Button_FillTexture != -1, "Couldn't load BucketTexture");

	IE->Button_PanViewTexture = Image_Load("Image_Editor/PanView.png");
	check(IE->Button_PanViewTexture != -1, "Couldn't load PanViewTexture");

	IE->Button_SaveTexture = Image_Load("Image_Editor/Save.png");
	check(IE->Button_SaveTexture != -1, "Couldn't load SaveTexture");

	IE->Button_LoadTexture = Image_Load("Image_Editor/Load.png");
	check(IE->Button_LoadTexture != -1, "Couldn't load LoadTexture");

	IE->Change_Confirm_Texture = Image_Load("Image_Editor/Confirm.png");
	check(IE->Change_Confirm_Texture != -1, "Couldn't load ConfirmTexture");

	IE->Close_Texture = Image_Load("Image_Editor/Close.png");
	check(IE->Close_Texture != -1, "Couldn't load CloseTexture");

	IE->Delete_Texture = Image_Load("Image_Editor/Bin.png");
	check(IE->Delete_Texture != -1, "Couldn't load DeleteTexture");

	IE->New_Texture = Image_Load("Image_Editor/NewFile.png");
	check(IE->New_Texture != -1, "Couldn't load NewFileTexture");

	IE->State = STATE_DEFAULT;

	IE->CurrentColor = Vector4_Create(255.0f, 0.0f, 1.0f, 255.0f);

	IE->Name = Gui_TextBox_Create(width / 6 * 2.4f + x, height / 20.0f * 19.0f + y,
			width / 6 * 2.4f, height / 20.0f, "Name : ", 15, 0, 0, height / 20.0f,
			width / 6 * 2.4f, 0.5f, 0.5f, 0.5f,
			1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
	check_mem(IE->Name);
	IE->Name->AcceptLetters = IE->Name->AcceptNumbers = IE->Name->AcceptDot = true;

	IE->Change_Confirm_Hover = false;
	IE->Close_Hover = false;
	IE->Delete_Hover = false;
	IE->New_Hover = false;

	IE->Color_Red = Gui_TextBox_Create((float) width / 10 * 8 + x,
			(float) height / 10 * 4 + y, (float) width / 10 * 2,
			(float) height / 10, "Red : ", 3, 0, 0, height / 10,
			(float) width / 10 * 2, 0.5f, 0.5f,
			0.5f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
	check_mem(IE->Color_Red);
	IE->Color_Green = Gui_TextBox_Create((float) width / 10 * 8 + x,
			(float) height / 10 * 3 + y, (float) width / 10 * 2,
			(float) height / 10, "Green : ", 3, 0, 0, height / 10,
			(float) width / 10 * 2, 0.5f, 0.5f,
			0.5f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
	check_mem(IE->Color_Green);
	IE->Color_Blue = Gui_TextBox_Create((float) width / 10 * 8 + x,
			(float) height / 10 * 2 + y, (float) width / 10 * 2,
			(float) height / 10, "Blue : ", 3, 0, 0, height / 10,
			(float) width / 10 * 2, 0.5f, 0.5f,
			0.5f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
	check_mem(IE->Color_Blue);
	IE->Color_Alpha = Gui_TextBox_Create((float) width / 10 * 8 + x,
			(float) height / 10 + y, (float) width / 10 * 2,
			(float) height / 10, "Alpha : ", 3, 0, 0, height / 10,
			(float) width / 10 * 2, 0.5f, 0.5f,
			0.5f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
	check_mem(IE->Color_Alpha);
	IE->Color_Red->AcceptNumbers = IE->Color_Green->AcceptNumbers =
			IE->Color_Blue->AcceptNumbers = IE->Color_Alpha->AcceptNumbers =
			true;
	IE->Color_Red->Value[0] = '2';
	IE->Color_Red->Value[1] = '5';
	IE->Color_Red->Value[2] = '5';
	IE->Color_Green->Value[0] = '2';
	IE->Color_Green->Value[1] = '5';
	IE->Color_Green->Value[2] = '5';
	IE->Color_Blue->Value[0] = '2';
	IE->Color_Blue->Value[1] = '5';
	IE->Color_Blue->Value[2] = '5';
	IE->Color_Alpha->Value[0] = '2';
	IE->Color_Alpha->Value[1] = '5';
	IE->Color_Alpha->Value[2] = '5';

	printOpenGLError();

	Image_Editor_Resize(IE);

	IE->Button_PencilHover = IE->Button_PickColorHover = IE->Button_FillHover =
			IE->Button_PanViewHover = false;
	IE->Button_PencilOn = IE->Button_PickColorOn = IE->Button_FillOn =
			IE->Button_PanViewOn = false;
	IE->PanningView = false;

	IE->Button_SaveHover = IE->Button_LoadHover = false;

	IE->ScrollingSpeed = 8;

	IE->Image_Tab_List = vector_new(sizeof(struct Image_Tab));
	check_mem(IE->Image_Tab_List);
	IE->Image_Tab_Index = -1;

	IE->Buttons = NULL;
	IE->TextBoxs = NULL;

	return IE;
	printOpenGLError();
	error: if (IE)
	{
		if (IE->Change_Confirm_Texture)
			Image_Free(IE->Change_Confirm_Texture);
		if (IE->Name)
			Gui_TextBox_Free(&IE->Name);
		if (IE->Close_Texture)
			Image_Free(IE->Close_Texture);
		if (IE->New_Texture)
			Image_Free(IE->New_Texture);
		if (IE->Delete_Texture)
			Image_Free(IE->Delete_Texture);
		if (IE->Color_Alpha)
			Gui_TextBox_Free(&IE->Color_Alpha);
		if (IE->Color_Red)
			Gui_TextBox_Free(&IE->Color_Red);
		if (IE->Color_Green)
			Gui_TextBox_Free(&IE->Color_Green);
		if (IE->Color_Blue)
			Gui_TextBox_Free(&IE->Color_Blue);
		if (IE->Button_PencilTexture >= 0)
			Image_Free(IE->Button_PencilTexture);
		if (IE->Button_PickColorTexture >= 0)
			Image_Free(IE->Button_PickColorTexture);
		if (IE->Button_FillTexture >= 0)
			Image_Free(IE->Button_FillTexture);
		if (IE->Button_PanViewTexture >= 0)
			Image_Free(IE->Button_PanViewTexture);
		if (IE->Button_SaveTexture >= 0)
			Image_Free(IE->Button_SaveTexture);
		if (IE->Button_LoadTexture >= 0)
			Image_Free(IE->Button_LoadTexture);
		if (IE->Image_Texture >= 0)
			Image_Free(IE->Image_Texture);
		free(IE);
	}
	return NULL;
}

// Bucket Fill algirutgn
static inline _Bool COMP_COLOR(struct Image_RawData *I, int x, int y, struct Vector4f Col)
{
	return I->Data[(x + y * I->Width) * 4] == Col.x && I->Data[(x + y * I->Width) * 4 + 1] == Col.y && I->Data[(x + y * I->Width) * 4 + 2] == Col.z && I->Data[(x + y * I->Width) * 4 + 3] == Col.w;
}
static inline void SET_COLOR(struct Image_RawData *I, int x, int y, struct Vector4f Col)
{
	I->Data[(x + y * I->Width) * 4] = (unsigned char)Col.x;
	I->Data[(x + y * I->Width) * 4 + 1] = (unsigned char)Col.y;
	I->Data[(x + y * I->Width) * 4 + 2] = (unsigned char)Col.z;
	I->Data[(x + y * I->Width) * 4 + 3] = (unsigned char)Col.w;
}

static struct Image_RawData *BF_IT;
static int BF_Orig_X, BF_Orig_Y;
static struct Vector4f BF_Color_To, BF_Color_From;

/* The following Flood fill algorithm was not written by the owner of this project and can be found at :
 * http://lodev.org/cgtutor/files/floodfill.cpp */
#define BF_stackSize 16777216
static int BF_stack[BF_stackSize];
static int BF_stackPointer;

////////////////////////////////////////////////////////////////////////////////
//Stack Functions                                                             //
////////////////////////////////////////////////////////////////////////////////

static _Bool BF_pop(int *x, int *y)
{
    if(BF_stackPointer > 0)
    {
        int p = BF_stack[BF_stackPointer];
        *x = p / BF_IT->Height;
        *y = p % BF_IT->Height;
        BF_stackPointer--;
        return true;
    }
    else
    {
        return false;
    }
}

static _Bool BF_push(int x, int y)
{
    if(BF_stackPointer < BF_stackSize - 1)
    {
    	BF_stackPointer++;
    	BF_stack[BF_stackPointer] = BF_IT->Height * x + y;
        return true;
    }
    else
    {
        return false;
    }
}

static void BF_emptyStack()
{
    int x, y;
    while(BF_pop(&x, &y));
}

static void BF_Bucket_Fill(struct Image_RawData *IT, int x, int y,
		struct Vector4f From, struct Vector4f To)
{
	if(To.x == From.x && To.y == From.y && To.z == From.z && To.w == From.w) return; //avoid infinite loop
	BF_emptyStack();

	if(!BF_push(x, y)) return;
	while(BF_pop(&x, &y))
	{
		SET_COLOR(IT, x, y, To);
	    if(x + 1 < IT->Width && COMP_COLOR(IT, x + 1, y, From))
	    {
	    	if(!BF_push(x + 1, y)) return;
	    }
	    if(x - 1 >= 0 && COMP_COLOR(IT, x - 1, y, From))
	    {
	    	if(!BF_push(x - 1, y)) return;
	    }
	    if(y + 1 < IT->Height && COMP_COLOR(IT, x, y + 1, From))
	    {
	    	if(!BF_push(x, y + 1)) return;
	    }
	    if(y - 1 >= 0 && COMP_COLOR(IT, x, y - 1, From))
	    {
	    	if(!BF_push(x, y - 1)) return;
	    }
	}
}

static _Bool BF_Thread_Running = false;

static int Bucket_Fill(void *Args)
{
	(void)Args;
	BF_Thread_Running = true;
	BF_Bucket_Fill(BF_IT, BF_Orig_X, BF_Orig_Y, BF_Color_From, BF_Color_To);
	BF_Thread_Running = false;
	return 0;
}

void Image_Editor_Header(struct Image_Editor *IE)
{
	if (IE->Close.v3.x != IE->Editor_Dimensions.x + IE->Editor_Dimensions.z
			|| IE->Close.v3.y != IE->Editor_Dimensions.y + IE->Editor_Dimensions.w || IE->Keyboard_Show.v1.x != IE->Editor_Dimensions.x || IE->Keyboard_Show.v1.y != IE->Editor_Dimensions.y)
	{
		log_info("Image Editor Resizing");
		Image_Editor_Resize(IE);
	}

	IE->Change_Confirm_Hover = Point_inQuad(
			Vector2_Create(Mouse.x, Mouse.y), IE->Change_Confirm);

	IE->Close_Hover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y),
			IE->Close);

	IE->Delete_Hover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y),
			IE->Delete);

	IE->New_Hover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), IE->New);

	if (IE->Change_Confirm_Hover && Mouse.justPressed && IE->Image_Tab_Index != -1)
	{
		struct Image_Tab *IT = ((struct Image_Tab *)IE->Image_Tab_List->items) + IE->Image_Tab_Index;
		free(IT->Name_Edit);
		IT->Name_Edit = malloc(sizeof(char) * (String_length(IE->Name->Value) + 1));
		IT->Name_Edit = memcpy(IT->Name_Edit, IE->Name->Value, sizeof(char) * (String_length(IE->Name->Value) + 1));
		Image_Tab_Save(IT);
		Image_Tab_Close(IT);
		vector_erase(IE->Image_Tab_List, IE->Image_Tab_Index);

		int TEMP = IE->Image_Tab_Index;
		IE->Image_Tab_Index = -1;
		if(IE->Image_Tab_List->size > 0 && TEMP > 0)
			Image_Set(IE, TEMP - 1);
		else
			Image_Set(IE, -1);
	}

	if (IE->Close_Hover && IE->State == STATE_DEFAULT && Mouse.justPressed && IE->Image_Tab_Index != -1)
	{
		struct Image_Tab *IT = ((struct Image_Tab *)IE->Image_Tab_List->items) + IE->Image_Tab_Index;
		if(Image_Tab_isDirty(IT))
		{
			Clear_State(IE, STATE_CONFIRMCLOSE);
		} else {
			Image_Tab_Close(IT);
			vector_erase(IE->Image_Tab_List, IE->Image_Tab_Index);

			int TEMP = IE->Image_Tab_Index;
			IE->Image_Tab_Index = -1;
			if(IE->Image_Tab_List->size > 0 && TEMP > 0)
				Image_Set(IE, TEMP - 1);
			else
				Image_Set(IE, -1);
		}
	}

	if (IE->Delete_Hover && Mouse.justPressed && IE->Image_Tab_Index != -1)
	{
		Clear_State(IE, STATE_CONFIRMDELETE);
	}

	// Save/Load Buttons
	if (Point_inQuad(Vector2_Create((float) Mouse.x, (float) Mouse.y),
			IE->Button_Save))
	{
		IE->Button_SaveHover = true;

		if(Mouse.justPressed && IE->Image_Tab_Index != -1) {
			struct Image_Tab *IT = ((struct Image_Tab *)IE->Image_Tab_List->items) + IE->Image_Tab_Index;
			free(IT->Name_Edit);
			IT->Name_Edit = malloc(sizeof(char) * (String_length(IE->Name->Value) + 1));
			IT->Name_Edit = memcpy(IT->Name_Edit, IE->Name->Value, sizeof(char) * (String_length(IE->Name->Value) + 1));
			Image_Tab_Save(IT);
		}
	}
	else
	{
		IE->Button_SaveHover = false;
	}

	if (Point_inQuad(Vector2_Create((float) Mouse.x, (float) Mouse.y),
			IE->Button_Load))
	{
		IE->Button_LoadHover = true;
		if(Mouse.justPressed) Clear_State(IE, STATE_OPENIMAGE);
	}
	else
	{
		IE->Button_LoadHover = false;
	}

	if (IE->New_Hover && Mouse.justPressed)
	{
		Clear_State(IE, STATE_NEWIMAGE);
	}

	if (IE->Change_Confirm_Hover)
		Image_Shader.pushQuad(IE->Change_Confirm,
				Quad_Create(0, 0, 0, 1, 1, 1, 1, 0), IE->Change_Confirm_Texture,
				IE->Hover_addColor);
	else
		Image_Shader.pushQuad(IE->Change_Confirm,
				Quad_Create(0, 0, 0, 1, 1, 1, 1, 0), IE->Change_Confirm_Texture,
				Vector4_Create(0, 0, 0, 0));

	if (IE->Close_Hover)
		Image_Shader.pushQuad(IE->Close, Quad_Create(0, 0, 0, 1, 1, 1, 1, 0),
				IE->Close_Texture, IE->Hover_addColor);
	else
		Image_Shader.pushQuad(IE->Close, Quad_Create(0, 0, 0, 1, 1, 1, 1, 0),
				IE->Close_Texture, Vector4_Create(0, 0, 0, 0));

	if (IE->Delete_Hover)
		Image_Shader.pushQuad(IE->Delete, Quad_Create(0, 0, 0, 1, 1, 1, 1, 0),
				IE->Delete_Texture, IE->Hover_addColor);
	else
		Image_Shader.pushQuad(IE->Delete, Quad_Create(0, 0, 0, 1, 1, 1, 1, 0),
				IE->Delete_Texture, Vector4_Create(0.0f, 0.0f, 0.0f, 0.0f));

	if (IE->Button_SaveHover)
		Image_Shader.pushQuad(IE->Button_Save,
				Quad_Create(0, 0, 0, 1, 1, 1, 1, 0), IE->Button_SaveTexture,
				IE->Hover_addColor);
	else
		Image_Shader.pushQuad(IE->Button_Save,
				Quad_Create(0, 0, 0, 1, 1, 1, 1, 0), IE->Button_SaveTexture,
				Vector4_Create(0.0f, 0.0f, 0.0f, 0.0f));

	if (IE->Button_LoadHover)
		Image_Shader.pushQuad(IE->Button_Load,
				Quad_Create(0, 0, 0, 1, 1, 1, 1, 0), IE->Button_LoadTexture,
				IE->Hover_addColor);
	else
		Image_Shader.pushQuad(IE->Button_Load,
				Quad_Create(0, 0, 0, 1, 1, 1, 1, 0), IE->Button_LoadTexture,
				Vector4_Create(0.0f, 0.0f, 0.0f, 0.0f));

	if (IE->New_Hover)
		Image_Shader.pushQuad(IE->New, Quad_Create(0, 0, 0, 1, 1, 1, 1, 0),
				IE->New_Texture, IE->Hover_addColor);
	else
		Image_Shader.pushQuad(IE->New, Quad_Create(0, 0, 0, 1, 1, 1, 1, 0),
				IE->New_Texture, Vector4_Create(0.0f, 0.0f, 0.0f, 0.0f));

	Default_Shader.pushQuad(IE->Image_Tab, Vector4_Create(0.4f, 0.4f, 0.4f, 1.0f));

	float FileTab_Width[IE->Image_Tab_List->size];
	float AllWidth = 0;
	double Next_FileTab = View_TranslateTo(IE->Image_Tab_Scroll, 640, Game_Width);

	for(int i = 0; i < IE->Image_Tab_List->size; i++)
	{
		FileTab_Width[i] = Font_HeightLength(DefaultFontManager, (((struct Image_Tab *)IE->Image_Tab_List->items) + i)->Name_Edit, IE->Image_Tab.v2.y - IE->Image_Tab.v1.y, 1.0f);
		FileTab_Width[i] = FileTab_Width[i] > IMAGE_TAB_MAXWIDTH ? IMAGE_TAB_MAXWIDTH : FileTab_Width[i];
		AllWidth += FileTab_Width[i];
	}

	if(IE->Image_Tab_isScrolling)
	{
		double X = View_TranslateTo(Mouse.x, Game_Width, 640);
		double preX = View_TranslateTo(Mouse.prevX, Game_Width, 640);

		double diffX = X - preX;

		if(((Next_FileTab + AllWidth > IE->Image_Tab.v3.x )&( diffX < 0 ))|| (((View_TranslateTo(IE->Image_Tab_Scroll, 640, Game_Width) + IE->Editor_Dimensions.x) < 0) & ( diffX > 0 )))
			IE->Image_Tab_Scroll  += diffX;

		if(IE->Image_Tab_Scroll > 0)
			IE->Image_Tab_Scroll = 0;
		if(Next_FileTab + AllWidth < IE->Image_Tab.v3.x && IE->Image_Tab_Scroll < 0.0f)
		{
			double Temp = IE->Image_Tab.v3.x - (Next_FileTab + AllWidth);
			IE->Image_Tab_Scroll += View_TranslateTo(Temp, Game_Width, 640);
		}
	}

	for(int i = 0; i < IE->Image_Tab_List->size; i++)
	{
		struct Quad Quad = Quad_Create(Next_FileTab,
				IE->Image_Tab.v1.y, Next_FileTab,
				IE->Image_Tab.v2.y, Next_FileTab + FileTab_Width[i],
				IE->Image_Tab.v3.y, Next_FileTab + FileTab_Width[i], IE->Image_Tab.v4.y);

		_Bool Hover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), Quad);

		if(IE->Image_Tab_Index == i || Hover)
			Default_Shader.pushQuad(Quad, Vector4_Create(0.90f, 0.90f, 0.90f, 1.0f));

		struct Image_Tab *Image = ((struct Image_Tab *)IE->Image_Tab_List->items) + i;
		Font_HeightRenderRenderConstraint(DefaultFontManager, Image->Name_Edit, Quad.v1.x + FileTab_Width[i] / 10.0f, Quad.v1.y, Quad.v2.y - Quad.v1.y, 1.0f, COLOR_RED, Vector2_Create(Quad.v1.x, Quad.v3.x));
		Image = ((struct Image_Tab *)IE->Image_Tab_List->items) + IE->Image_Tab_Index;
		if(Mouse.justQuickPressed && Hover)
		{
			if(i == IE->Image_Tab_Index)
				Image_Set(IE, -1);
			else
				Image_Set(IE, i);
		}

		Next_FileTab += FileTab_Width[i];
	}

	// Render Keyboard show
	if(Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), IE->Keyboard_Show))
	{
		Default_Shader.pushQuad(IE->Keyboard_Show, Vector4_Create(0.50f, 0.50f, 0.50f, 1.0f));

		if(Mouse.justReleased)
		{
			if(!OnScreen_Keyboard)Engine_requestKeyboard(0, 0, Game_Width, Game_Height / 20.0f * 9.0f);
			else Engine_requestCloseKeyboard();
		}
	} else
		Default_Shader.pushQuad(IE->Keyboard_Show, Vector4_Create(1.0f, 1.0f, 1.0f, 1.0f));

	Font_FixedRender(DefaultFontManager, "Keyboard", IE->Keyboard_Show.v1.x, IE->Keyboard_Show.v1.y, IE->Keyboard_Show.v2.y - IE->Keyboard_Show.v1.y, IE->Keyboard_Show.v3.x - IE->Keyboard_Show.v1.x, 1.0f, Vector4_Create(1.0f, 0.0f, 0.0f, 1.0f));
}

static void Image_Render_OpenImage(struct Image_Editor *IE)
{
	static _Bool Error = false;
	static int Error_Timer = 0;

	if(IE->TextBoxs && IE->TextBoxs->size != 1)
	{
		struct Gui_Button *Button = IE->Buttons->items;
		struct Gui_TextBox *TextBox = IE->TextBoxs->items;

		for(int i = IE->Buttons->size - 1; i >= 0; i--)
			Gui_Button_Free_Simple(Button + i);
		for(int i = IE->TextBoxs->size - 1; i >= 0; i--)
			Gui_TextBox_Free_Simple(TextBox + i);
		vector_delete(IE->Buttons);
		vector_delete(IE->TextBoxs);
		IE->Buttons = NULL;
		IE->TextBoxs = NULL;
	}
	if(!IE->Buttons)
	{
		Error = false;
		IE->Buttons = vector_new(sizeof(struct Gui_Button));
		struct Gui_Button *B = Gui_Button_Create(IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 2.0f,
				IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 7.0f, IE->Editor_Dimensions.z / 20.0f * 6.0f, IE->Editor_Dimensions.w / 20.0f, "Open", 0, 0, IE->Editor_Dimensions.z / 20.0f * 4.0f, IE->Editor_Dimensions.w / 20.0f,
				0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
		B->CenterText = true;

		vector_push_back(IE->Buttons, B);
		free(B);

		B = Gui_Button_Create(IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 11.0f,
				IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 7.0f, IE->Editor_Dimensions.z / 20.0f * 6.0f, IE->Editor_Dimensions.w / 20.0f, "Cancel", 0, 0, IE->Editor_Dimensions.z / 20.0f * 4.0f, IE->Editor_Dimensions.w / 20.0f,
				0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
		B->CenterText = true;
		vector_push_back(IE->Buttons, B);
		free(B);

		IE->TextBoxs = vector_new(sizeof(struct Gui_TextBox));

		struct Gui_TextBox *T = Gui_TextBox_Create(IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 5.0f,
				IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 10.0f, IE->Editor_Dimensions.z / 20.0f * 10.0f, IE->Editor_Dimensions.w / 20.0f, "Name : ", 15, 0, 0, IE->Editor_Dimensions.w / 20.0f, IE->Editor_Dimensions.z / 20.0f * 10.0f,
				0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
		T->AcceptNumbers = T->AcceptLetters = T->AcceptDot = true;
		T->Quad_takeMouse = Quad_Create(IE->Editor_Dimensions.x, IE->Keyboard_Show.v1.y, IE->Editor_Dimensions.x, IE->Editor_Dimensions.y + IE->Editor_Dimensions.w, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z, IE->Editor_Dimensions.y + IE->Editor_Dimensions.w, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z, IE->Keyboard_Show.v1.y);
		vector_push_back(IE->TextBoxs, T);
		free(T);
	}

	{
		struct Gui_Button *Button = IE->Buttons->items;
		if(Button->Y != IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 7.0f)
		{
			Gui_Button_Resize(Button, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 2.0f,
				IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 7.0f, IE->Editor_Dimensions.z / 20.0f * 6.0f, IE->Editor_Dimensions.w / 20.0f);
			Button->TextWidth = IE->Editor_Dimensions.z / 20.0f * 4.0f;
			Button->TextHeight = IE->Editor_Dimensions.w / 20.0f;

			Gui_Button_Resize(Button + 1, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 11.0f,
					IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 7.0f, IE->Editor_Dimensions.z / 20.0f * 6.0f, IE->Editor_Dimensions.w / 20.0f);
			Button[1].TextWidth = IE->Editor_Dimensions.z / 20.0f * 4.0f;
			Button[1].TextHeight = IE->Editor_Dimensions.w / 20.0f;

			struct Gui_TextBox *TextBox = IE->TextBoxs->items;

			Gui_TextBox_Resize(TextBox, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 5.0f,
					IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 10.0f, IE->Editor_Dimensions.z / 20.0f * 10.0f, IE->Editor_Dimensions.w / 20.0f);
			TextBox->TextMaxWidth = IE->Editor_Dimensions.z / 20.0f * 10.0f;
			TextBox->TextHeight = IE->Editor_Dimensions.w / 20.0f;
			TextBox->Quad_takeMouse = Quad_Create(IE->Editor_Dimensions.x, IE->Keyboard_Show.v1.y, IE->Editor_Dimensions.x, IE->Editor_Dimensions.y + IE->Editor_Dimensions.w, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z, IE->Editor_Dimensions.y + IE->Editor_Dimensions.w, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z, IE->Keyboard_Show.v1.y);
		}
	}

	if(Error)
	{
		Error_Timer--;

		Font_HeightMaxRenderCA(DefaultFontManager, "File doesn't exists", IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 10.0f,
				IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 8.5f, IE->Editor_Dimensions.w / 20.0f, IE->Editor_Dimensions.z / 20.0f * 6.0f, 1.0f, COLOR_RED);

		if(Error_Timer < 0)
			Error = false;
	}

	_Bool GOTO_DEFAULT = false;
	struct Gui_Button *Button = IE->Buttons->items;
	for (int i = 0; i < IE->Buttons->size; i++) {
		_Bool Pressed = Gui_Button_Render(Button + i);
		if (Pressed && !i) {
			// Open
			struct Gui_TextBox *TB =
							((struct Gui_TextBox *) IE->TextBoxs->items);

			char *Path = FileExternal_GetFullPath(TB->Value);
			FILE *f = fopen(Path, "rb");

			if(f)
			{
				fclose(f);
				struct Image_Tab *Image = Image_Tab_Load(TB->Value);
				vector_push_back(IE->Image_Tab_List, Image);
				free(Image);
				GOTO_DEFAULT = true;

				Image_Set(IE, IE->Image_Tab_List->size - 1);
			} else {
				Error = true;
				Error_Timer = Game_FPS * 6;
			}
			free(Path);
		} else if (Pressed) {
			// Cancel
			GOTO_DEFAULT = true;
		}
	}

	struct Gui_TextBox *TextBox = IE->TextBoxs->items;
	for (int i = 0; i < IE->TextBoxs->size; i++)
		Gui_TextBox_Render(TextBox + i);

	if (GOTO_DEFAULT) {
		Clear_State(IE, STATE_DEFAULT);
	}
}

static void Image_Render_NewImage(struct Image_Editor *IE)
{
	static _Bool Error = false;
	static int Error_Timer = 0;

	if(IE->TextBoxs && IE->TextBoxs->size != 3)
	{
		struct Gui_Button *Button = IE->Buttons->items;
		struct Gui_TextBox *TextBox = IE->TextBoxs->items;

		for(int i = IE->Buttons->size - 1; i >= 0; i--)
			Gui_Button_Free_Simple(Button + i);
		for(int i = IE->TextBoxs->size - 1; i >= 0; i--)
			Gui_TextBox_Free_Simple(TextBox + i);
		vector_delete(IE->Buttons);
		vector_delete(IE->TextBoxs);
		IE->Buttons = NULL;
		IE->TextBoxs = NULL;
	}
	if(!IE->Buttons)
	{
		Error = false;
		IE->Buttons = vector_new(sizeof(struct Gui_Button));
		struct Gui_Button *B = Gui_Button_Create(IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 3.0f,
				IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 7.0f, IE->Editor_Dimensions.z / 20.0f * 6.0f, IE->Editor_Dimensions.w / 20.0f, "New", 0, 0, IE->Editor_Dimensions.z / 20.0f * 4.0f, IE->Editor_Dimensions.w / 20.0f,
				0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
		B->CenterText = true;
		vector_push_back(IE->Buttons, B);
		free(B);

		B = Gui_Button_Create(IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 12.0f,
				IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 7.0f, IE->Editor_Dimensions.z / 20.0f * 6.0f, IE->Editor_Dimensions.w / 20.0f, "Cancel", 0, 0, IE->Editor_Dimensions.z / 20.0f * 4.0f, IE->Editor_Dimensions.w / 20.0f,
				0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
		B->CenterText = true;
		vector_push_back(IE->Buttons, B);
		free(B);

		IE->TextBoxs = vector_new(sizeof(struct Gui_TextBox));

		struct Gui_TextBox *T = Gui_TextBox_Create(IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 5.0f,
				IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 13.0f, IE->Editor_Dimensions.z / 20.0f * 10.0f, IE->Editor_Dimensions.w / 20.0f, "Name : ", 15, 0, 0, IE->Editor_Dimensions.w / 20.0f, IE->Editor_Dimensions.z / 20.0f * 10.0f,
				0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
		T->AcceptNumbers = T->AcceptLetters = T->AcceptDot = true;
		T->Quad_takeMouse = Quad_Create(IE->Editor_Dimensions.x, IE->Keyboard_Show.v1.y, IE->Editor_Dimensions.x, IE->Editor_Dimensions.y + IE->Editor_Dimensions.w, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z, IE->Editor_Dimensions.y + IE->Editor_Dimensions.w, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z, IE->Keyboard_Show.v1.y);
		vector_push_back(IE->TextBoxs, T);
		free(T);

		T = Gui_TextBox_Create(IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 2.0f,
				IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 10.0f, IE->Editor_Dimensions.z / 20.0f * 8.0f, IE->Editor_Dimensions.w / 20.0f, "Width : ", 3, 0, 0, IE->Editor_Dimensions.w / 20.0f, IE->Editor_Dimensions.z / 20.0f * 8.0f,
				0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
		T->AcceptNumbers = true;
		T->Quad_takeMouse = Quad_Create(IE->Editor_Dimensions.x, IE->Keyboard_Show.v1.y, IE->Editor_Dimensions.x, IE->Editor_Dimensions.y + IE->Editor_Dimensions.w, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z, IE->Editor_Dimensions.y + IE->Editor_Dimensions.w, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z, IE->Keyboard_Show.v1.y);
		vector_push_back(IE->TextBoxs, T);
		free(T);

		T = Gui_TextBox_Create(IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 11.0f,
				IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 10.0f, IE->Editor_Dimensions.z / 20.0f * 8.0f, IE->Editor_Dimensions.w / 20.0f, "Height : ", 3, 0, 0, IE->Editor_Dimensions.w / 20.0f, IE->Editor_Dimensions.z / 20.0f * 8.0f,
				0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
		T->AcceptNumbers = true;
		T->Quad_takeMouse = Quad_Create(IE->Editor_Dimensions.x, IE->Keyboard_Show.v1.y, IE->Editor_Dimensions.x, IE->Editor_Dimensions.y + IE->Editor_Dimensions.w, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z, IE->Editor_Dimensions.y + IE->Editor_Dimensions.w, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z, IE->Keyboard_Show.v1.y);
		vector_push_back(IE->TextBoxs, T);
		free(T);
	}

	{
		struct Gui_Button *Button = IE->Buttons->items;
		if(Button->Y != IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 7.0f)
		{
			Gui_Button_Resize(Button, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 3.0f,
				IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 7.0f, IE->Editor_Dimensions.z / 20.0f * 6.0f, IE->Editor_Dimensions.w / 20.0f);
			Button->TextWidth = IE->Editor_Dimensions.z / 20.0f * 4.0f;
			Button->TextHeight = IE->Editor_Dimensions.w / 20.0f;

			Gui_Button_Resize(Button + 1, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 12.0f,
					IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 7.0f, IE->Editor_Dimensions.z / 20.0f * 6.0f, IE->Editor_Dimensions.w / 20.0f);
			Button[1].TextWidth = IE->Editor_Dimensions.z / 20.0f * 4.0f;
			Button[1].TextHeight = IE->Editor_Dimensions.w / 20.0f;

			struct Gui_TextBox *TextBox = IE->TextBoxs->items;

			Gui_TextBox_Resize(TextBox, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 5.0f,
					IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 13.0f, IE->Editor_Dimensions.z / 20.0f * 10.0f, IE->Editor_Dimensions.w / 20.0f);
			TextBox->TextMaxWidth = IE->Editor_Dimensions.z / 20.0f * 10.0f;
			TextBox->TextHeight = IE->Editor_Dimensions.w / 20.0f;

			Gui_TextBox_Resize(TextBox + 1, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 2.0f,
					IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 10.0f, IE->Editor_Dimensions.z / 20.0f * 8.0f, IE->Editor_Dimensions.w / 20.0f);
			TextBox[1].TextMaxWidth = IE->Editor_Dimensions.z / 20.0f * 8.0f;
			TextBox[1].TextHeight = IE->Editor_Dimensions.w / 20.0f;

			Gui_TextBox_Resize(TextBox + 2, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 11.0f,
					IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 10.0f, IE->Editor_Dimensions.z / 20.0f * 8.0f, IE->Editor_Dimensions.w / 20.0f);
			TextBox[2].TextMaxWidth = IE->Editor_Dimensions.z / 20.0f * 8.0f;
			TextBox[2].TextHeight = IE->Editor_Dimensions.w / 20.0f;

			TextBox->Quad_takeMouse = TextBox[1].Quad_takeMouse = TextBox[2].Quad_takeMouse = Quad_Create(IE->Editor_Dimensions.x, IE->Keyboard_Show.v1.y, IE->Editor_Dimensions.x, IE->Editor_Dimensions.y + IE->Editor_Dimensions.w, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z, IE->Editor_Dimensions.y + IE->Editor_Dimensions.w, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z, IE->Keyboard_Show.v1.y);
		}
	}

	if(Error)
	{
		Error_Timer--;

		Font_HeightMaxRenderCA(DefaultFontManager, "File already exists", IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 10.5f,
				IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 8.25f, IE->Editor_Dimensions.w / 15.0f, IE->Editor_Dimensions.z / 20.0f * 6.0f, 1.0f, COLOR_RED);

		if(Error_Timer < 0)
			Error = false;
	}

	_Bool GOTO_DEFAULT = false;
	struct Gui_Button *Button = IE->Buttons->items;
	for(int i = 0; i < IE->Buttons->size; i++)
	{
		_Bool Pressed = Gui_Button_Render(Button + i);
		if(Pressed && !i)
		{
			struct Gui_TextBox *TB = ((struct Gui_TextBox *)IE->TextBoxs->items);

			char *Path = FileExternal_GetFullPath(TB->Value);
			FILE *Test = fopen(Path, "rb");
			if(Test)
			{
				fclose(Test);
				Error = true;
				Error_Timer = Game_FPS * 6;

			} else if(String_toInt(TB[1].Value) > 0 && String_toInt(TB[2].Value) > 0) {
				// New
				struct Image_Tab *Image = malloc(sizeof(struct Image_Tab));

				Image->Name_Original = malloc(sizeof(char) * (String_length(TB->Value) + 1));
				Image->Name_Original = memcpy(Image->Name_Original, TB->Value, sizeof(char) * (String_length(TB->Value) + 1));
				Image->Name_Edit = malloc(sizeof(char) * (String_length(TB->Value) + 1));
				Image->Name_Edit = memcpy(Image->Name_Edit, TB->Value, sizeof(char) * (String_length(TB->Value) + 1));

				Image->Image_Original = malloc(sizeof(struct Image_RawData));
				Image->Image_Original->Width = String_toInt(TB[1].Value);
				Image->Image_Original->Height = String_toInt(TB[2].Value);
				Image->Image_Original->Data = malloc(sizeof(unsigned char) * (Image->Image_Original->Width * Image->Image_Original->Height * 4));
				for(int i = 0; i < Image->Image_Original->Width * Image->Image_Original->Height * 4; i++)
					Image->Image_Original->Data[i] = 255;

				Image->Image_Edit = malloc(sizeof(struct Image_RawData));
				Image->Image_Edit->Data = malloc(sizeof(unsigned char) * Image->Image_Original->Width * Image->Image_Original->Height * 4);
				Image->Image_Edit->Data = memcpy(Image->Image_Edit->Data, Image->Image_Original->Data, sizeof(unsigned char) * Image->Image_Original->Width * Image->Image_Original->Height * 4);
				Image->Image_Edit->Width = Image->Image_Original->Width;
				Image->Image_Edit->Height = Image->Image_Original->Height;

				Image->BarY = 0;
				Image->BarValue = MIN(Image->Image_Edit->Width, Image->Image_Edit->Height) / 100.0f;
				Image->X = Image->Y = 0;

				vector_push_back(IE->Image_Tab_List, Image);
				free(Image);
				GOTO_DEFAULT = true;

				Image_Set(IE, IE->Image_Tab_List->size - 1);
			}
			if(Path) free(Path);
		} else if(Pressed) {
			// Cancel
			GOTO_DEFAULT = true;
		}
	}

	struct Gui_TextBox *TextBox = IE->TextBoxs->items;
	for(int i = 0; i < IE->TextBoxs->size; i++)
		Gui_TextBox_Render(TextBox + i);

	if(GOTO_DEFAULT)
	{
		Clear_State(IE, STATE_DEFAULT);
	}
}

static void Image_Render_ConfirmClose(struct Image_Editor *IE)
{
	if(!IE->Confirm_Button)
	{
		IE->Confirm_Button = Gui_Button_Create(IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 3.0f,
				IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 8.0f, IE->Editor_Dimensions.z / 20.0f * 6.0f, IE->Editor_Dimensions.w / 20.0f, "Confirm", 0, 0, IE->Editor_Dimensions.z / 20.0f * 4.0f, IE->Editor_Dimensions.w / 20.0f,
				0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
		IE->Confirm_Button->CenterText = true;

		IE->Cancel_Button = Gui_Button_Create(IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 12.0f,
				IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 8.0f, IE->Editor_Dimensions.z / 20.0f * 6.0f, IE->Editor_Dimensions.w / 20.0f, "Cancel", 0, 0, IE->Editor_Dimensions.z / 20.0f * 4.0f, IE->Editor_Dimensions.w / 20.0f,
				0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
		IE->Cancel_Button->CenterText = true;
	}

	if(IE->Confirm_Button->Y != IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 8.0f){
		Gui_Button_Resize(IE->Confirm_Button, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 3.0f,
				IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 8.0f, IE->Editor_Dimensions.z / 20.0f * 6.0f, IE->Editor_Dimensions.w / 20.0f);
		IE->Confirm_Button->TextWidth = IE->Editor_Dimensions.z / 20.0f * 4.0f;
		IE->Confirm_Button->TextHeight = IE->Editor_Dimensions.w / 20.0f;

		Gui_Button_Resize(IE->Cancel_Button, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 12.0f,
				IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 8.0f, IE->Editor_Dimensions.z / 20.0f * 6.0f, IE->Editor_Dimensions.w / 20.0f);
		IE->Cancel_Button->TextWidth = IE->Editor_Dimensions.z / 20.0f * 4.0f;
		IE->Cancel_Button->TextHeight = IE->Editor_Dimensions.w / 20.0f;
	}

	Font_HeightMaxRenderCA(DefaultFontManager, "Image not saved, do you want to close?", IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 10.0f, IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 10.0f, IE->Editor_Dimensions.w / 15.0f, IE->Editor_Dimensions.z, 1.0f, COLOR_RED);

	_Bool GOTO_DEFAULT = false;
	if(Gui_Button_Render(IE->Confirm_Button))
	{
		struct Image_Tab *IT = ((struct Image_Tab *)IE->Image_Tab_List->items) + IE->Image_Tab_Index;
		Image_Tab_Close(IT);
		vector_erase(IE->Image_Tab_List, IE->Image_Tab_Index);

		int TEMP = IE->Image_Tab_Index;
		IE->Image_Tab_Index = -1;
		if(IE->Image_Tab_List->size > 0 && TEMP > 0)
			Image_Set(IE, TEMP - 1);
		else
			Image_Set(IE, -1);
		GOTO_DEFAULT = true;
	} else if(Gui_Button_Render(IE->Cancel_Button))
	{
		GOTO_DEFAULT = true;
	}

	if(GOTO_DEFAULT)
	{
		Clear_State(IE, STATE_DEFAULT);
	}
}

static void Image_Render_ConfirmDelete(struct Image_Editor *IE)
{
	if(!IE->Confirm_Button)
	{
		IE->Confirm_Button = Gui_Button_Create(IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 3.0f,
				IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 8.0f, IE->Editor_Dimensions.z / 20.0f * 6.0f, IE->Editor_Dimensions.w / 20.0f, "Confirm", 0, 0, IE->Editor_Dimensions.z / 20.0f * 4.0f, IE->Editor_Dimensions.w / 20.0f,
				0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
		IE->Confirm_Button->CenterText = true;

		IE->Cancel_Button = Gui_Button_Create(IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 12.0f,
				IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 8.0f, IE->Editor_Dimensions.z / 20.0f * 6.0f, IE->Editor_Dimensions.w / 20.0f, "Cancel", 0, 0, IE->Editor_Dimensions.z / 20.0f * 4.0f, IE->Editor_Dimensions.w / 20.0f,
				0.6f, 0.6f, 0.6f, 1.0f, 0.4f, 0.4f, 0.4f, 1.0f);
		IE->Cancel_Button->CenterText = true;
	}

	if(IE->Confirm_Button->Y != IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 8.0f){
		Gui_Button_Resize(IE->Confirm_Button, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 3.0f,
				IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 8.0f, IE->Editor_Dimensions.z / 20.0f * 6.0f, IE->Editor_Dimensions.w / 20.0f);
		IE->Confirm_Button->TextWidth = IE->Editor_Dimensions.z / 20.0f * 4.0f;
		IE->Confirm_Button->TextHeight = IE->Editor_Dimensions.w / 20.0f;

		Gui_Button_Resize(IE->Cancel_Button, IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 12.0f,
				IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 8.0f, IE->Editor_Dimensions.z / 20.0f * 6.0f, IE->Editor_Dimensions.w / 20.0f);
		IE->Cancel_Button->TextWidth = IE->Editor_Dimensions.z / 20.0f * 4.0f;
		IE->Cancel_Button->TextHeight = IE->Editor_Dimensions.w / 20.0f;
	}

	Font_HeightMaxRenderCA(DefaultFontManager, "Confirm you want to delete the image", IE->Editor_Dimensions.x + IE->Editor_Dimensions.z / 20.0f * 10.0f, IE->Editor_Dimensions.y + IE->Editor_Dimensions.w / 20.0f * 10.0f, IE->Editor_Dimensions.w / 15.0f, IE->Editor_Dimensions.z, 1.0f, COLOR_RED);

	_Bool GOTO_DEFAULT = false;
	if(Gui_Button_Render(IE->Confirm_Button))
	{
		struct Image_Tab *IT = ((struct Image_Tab *)IE->Image_Tab_List->items) + IE->Image_Tab_Index;
		char *C = FileExternal_GetFullPath(IT->Name_Original);
		FILE *TestExists = fopen(C, "rb");
		if(TestExists)
		{
			fclose(TestExists);
			remove(C);
		}
		free(C);
		Image_Tab_Close(IT);
		vector_erase(IE->Image_Tab_List, IE->Image_Tab_Index);

		int TEMP = IE->Image_Tab_Index;
		IE->Image_Tab_Index = -1;
		if (IE->Image_Tab_List->size > 0 && TEMP > 0)
			Image_Set(IE, TEMP - 1);
		else
			Image_Set(IE, -1);

		GOTO_DEFAULT = true;
	} else if(Gui_Button_Render(IE->Cancel_Button))
	{
		GOTO_DEFAULT = true;
	}

	if(GOTO_DEFAULT)
	{
		Clear_State(IE, STATE_DEFAULT);
	}
}

void Image_Editor_Render(struct Image_Editor *IE)
{
	Image_Editor_Header(IE);

	if(IE->State == STATE_NEWIMAGE) {
		Image_Render_NewImage(IE);
		return;
	} else if(IE->State == STATE_OPENIMAGE) {
		Image_Render_OpenImage(IE);
		return;
	} else if(IE->State == STATE_CONFIRMCLOSE) {
		Image_Render_ConfirmClose(IE);
		return;
	} else if(IE->State == STATE_CONFIRMDELETE) {
		Image_Render_ConfirmDelete(IE);
		return;
	} else if(IE->Image_Tab_Index == -1 && IE->State == STATE_DEFAULT)
		return;

	struct Image_Tab *IT = ((struct Image_Tab *)IE->Image_Tab_List->items) + IE->Image_Tab_Index;

	//Image_Editor_CheckScroll(IE, IT);
	Gui_Vertical_ScrollBar_Render(IE->Image_Scroll);

	IT->BarY = 100.0f + IE->Image_Scroll->BarY;
	if(IT->X < 0)
		IT->X = 0;
	if(IT->Y < 0)
		IT->Y = 0;
	if(IT->X + (float)IT->Image_Edit->Width / 100.0f * IT->BarY > IT->Image_Edit->Width)
		IT->X = IT->Image_Edit->Width - (float)IT->Image_Edit->Width / 100.0f * IT->BarY;
	if(IT->Y + (float)IT->Image_Edit->Height / 100.0f * IT->BarY > IT->Image_Edit->Height)
		IT->Y = IT->Image_Edit->Height - (float)IT->Image_Edit->Height / 100.0f * IT->BarY;

	// Drawing Visible sprite to screen
	IE->Image_View = Vector4_Create(IT->X, IT->Y, (float)IT->Image_Edit->Width / 100.0f * IT->BarY, (float)IT->Image_Edit->Height / 100.0f * IT->BarY);

	glBindTexture(GL_TEXTURE_2D, IE->Image_Texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, IT->Image_Edit->Width,
			IT->Image_Edit->Height, GL_RGBA, GL_UNSIGNED_BYTE,
			(GLvoid*) IT->Image_Edit->Data);

	Image_Shader.pushQuad(IE->Image_Editor_View,
			Quad_Create(IE->Image_View.x / IT->Image_Edit->Width,
					IE->Image_View.y / IT->Image_Edit->Height
							+ IE->Image_View.w / IT->Image_Edit->Height,
							IE->Image_View.x / IT->Image_Edit->Width,
							IE->Image_View.y / IT->Image_Edit->Height,
							IE->Image_View.x / IT->Image_Edit->Width
							+ IE->Image_View.z / IT->Image_Edit->Width,
							IE->Image_View.y / IT->Image_Edit->Height,
							IE->Image_View.x / IT->Image_Edit->Width
							+ IE->Image_View.z / IT->Image_Edit->Width,
							IE->Image_View.y / IT->Image_Edit->Height
						+ IE->Image_View.w / IT->Image_Edit->Height), IE->Image_Texture,
			Vector4_Create(0.0f, 0.0f, 0.0f, 0.0f));

	// Drawing Pencil and PickColor Buttons
	IE->Button_PencilHover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), IE->Button_Pencil);

	IE->Button_PickColorHover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), IE->Button_PickColor);

	IE->Button_FillHover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), IE->Button_Fill);

	IE->Button_PanViewHover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), IE->Button_PanView);

	if (IE->Button_PencilHover && Mouse.justPressed) {
		IE->Button_PencilOn = true;
		IE->Button_PickColorOn = false;
		IE->Button_FillOn = false;
		IE->Button_PanViewOn = false;
	} else if (IE->Button_PickColorHover && Mouse.justPressed) {
		IE->Button_PickColorOn = true;
		IE->Button_PencilOn = false;
		IE->Button_FillOn = false;
		IE->Button_PanViewOn = false;
	} else if (IE->Button_FillHover && Mouse.justPressed) {
		IE->Button_FillOn = true;
		IE->Button_PickColorOn = false;
		IE->Button_PencilOn = false;
		IE->Button_PanViewOn = false;
	} else if (IE->Button_PanViewHover && Mouse.justPressed) {
		IE->Button_PanViewOn = true;
		IE->Button_FillOn = false;
		IE->Button_PickColorOn = false;
		IE->Button_PencilOn = false;
	}

	if (IE->Button_PencilHover || IE->Button_PencilOn)
		Image_Shader.pushQuad(IE->Button_Pencil,
				Quad_Create(0, 0, 0, 1, 1, 1, 1, 0), IE->Button_PencilTexture,
				Vector4_Create(-0.20f, -0.20f, -0.20f, 0.0f));
	else
		Image_Shader.pushQuad(IE->Button_Pencil,
				Quad_Create(0, 0, 0, 1, 1, 1, 1, 0), IE->Button_PencilTexture,
				Vector4_Create(0.0f, 0.0f, 0.0f, 0.0f));

	if (IE->Button_PickColorHover || IE->Button_PickColorOn)
		Image_Shader.pushQuad(IE->Button_PickColor,
				Quad_Create(0, 0, 0, 1, 1, 1, 1, 0),
				IE->Button_PickColorTexture,
				Vector4_Create(-0.20f, -0.20f, -0.20f, 0.0f));
	else
		Image_Shader.pushQuad(IE->Button_PickColor,
				Quad_Create(0, 0, 0, 1, 1, 1, 1, 0),
				IE->Button_PickColorTexture,
				Vector4_Create(0.0f, 0.0f, 0.0f, 0.0f));

	if (IE->Button_FillHover || IE->Button_FillOn)
		Image_Shader.pushQuad(IE->Button_Fill,
				Quad_Create(0, 0, 0, 1, 1, 1, 1, 0), IE->Button_FillTexture,
				Vector4_Create(-0.20f, -0.20f, -0.20f, 0.0f));
	else
		Image_Shader.pushQuad(IE->Button_Fill,
				Quad_Create(0, 0, 0, 1, 1, 1, 1, 0), IE->Button_FillTexture,
				Vector4_Create(0.0f, 0.0f, 0.0f, 0.0f));

	if (IE->Button_PanViewHover || IE->Button_PanViewOn)
		Image_Shader.pushQuad(IE->Button_PanView,
				Quad_Create(0, 0, 0, 1, 1, 1, 1, 0), IE->Button_PanViewTexture,
				Vector4_Create(-0.20f, -0.20f, -0.20f, 0.0f));
	else
		Image_Shader.pushQuad(IE->Button_PanView,
				Quad_Create(0, 0, 0, 1, 1, 1, 1, 0), IE->Button_PanViewTexture,
				Vector4_Create(0.0f, 0.0f, 0.0f, 0.0f));

	int ColorR, ColorB, ColorG, ColorA;
	ColorR = String_toInt(IE->Color_Red->Value);
	ColorG = String_toInt(IE->Color_Green->Value);
	ColorB = String_toInt(IE->Color_Blue->Value);
	ColorA = String_toInt(IE->Color_Alpha->Value);
	if (ColorR >= 0 && ColorR <= 255 && ColorG >= 0 && ColorG <= 255
			&& ColorB >= 0 && ColorB <= 255 && ColorA >= 0 && ColorA <= 255)
	{
		IE->CurrentColor.x = ColorR;
		IE->CurrentColor.y = ColorG;
		IE->CurrentColor.z = ColorB;
		IE->CurrentColor.w = ColorA;
	}

	if (Point_inQuad(Vector2_Create(Mouse.x, Mouse.y),
			IE->Image_Editor_View))
	{
		int X = View_TranslateTo((float) Mouse.x - IE->Image_Editor_View.v1.x,
				IE->Image_Editor_View.v4.x - IE->Image_Editor_View.v1.x,
				IE->Image_View.z);
		int Y = View_TranslateTo((float) IE->Image_Editor_View.v2.y - Mouse.y,
				IE->Image_Editor_View.v2.y - IE->Image_Editor_View.v1.y,
				IE->Image_View.w);
		X += IE->Image_View.x;
		Y += IE->Image_View.y;

		if (IE->Button_PencilOn && Mouse.isPressed)
		{
			IT->Image_Edit->Data[(X + IT->Image_Edit->Width * Y) * 4] =
					(char) IE->CurrentColor.x;
			IT->Image_Edit->Data[(X + IT->Image_Edit->Width * Y) * 4 + 1] =
					(char) IE->CurrentColor.y;
			IT->Image_Edit->Data[(X + IT->Image_Edit->Width * Y) * 4 + 2] =
					(char) IE->CurrentColor.z;
			IT->Image_Edit->Data[(X + IT->Image_Edit->Width * Y) * 4 + 3] =
					(char) IE->CurrentColor.w;
		}
		else if (IE->Button_PickColorOn && Mouse.justPressed)
		{
			Gui_TextBox_ClearText(IE->Color_Red);
			Gui_TextBox_ClearText(IE->Color_Green);
			Gui_TextBox_ClearText(IE->Color_Blue);
			Gui_TextBox_ClearText(IE->Color_Alpha);

			unsigned char Red =
					IT->Image_Edit->Data[(X + IT->Image_Edit->Width * Y) * 4];
			unsigned char Green = IT->Image_Edit->Data[(X
					+ IT->Image_Edit->Width * Y) * 4 + 1];
			unsigned char Blue = IT->Image_Edit->Data[(X
					+ IT->Image_Edit->Width * Y) * 4 + 2];
			unsigned char Alpha = IT->Image_Edit->Data[(X
					+ IT->Image_Edit->Width * Y) * 4 + 3];

			char *Text = Integer_toString((int) Red);
			IE->Color_Red->Value = memcpy(IE->Color_Red->Value, Text,
					String_length(Text));
			free(Text);

			Text = Integer_toString((int) Green);
			IE->Color_Green->Value = memcpy(IE->Color_Green->Value, Text,
					String_length(Text));
			free(Text);

			Text = Integer_toString((int) Blue);
			IE->Color_Blue->Value = memcpy(IE->Color_Blue->Value, Text,
					String_length(Text));
			free(Text);

			Text = Integer_toString((int) Alpha);
			IE->Color_Alpha->Value = memcpy(IE->Color_Alpha->Value, Text,
					String_length(Text));
			free(Text);
		}
		else if (IE->Button_FillOn && Mouse.justPressed && !BF_Thread_Running)
		{
			BF_IT = IT->Image_Edit;
			BF_Orig_X = X;
			BF_Orig_Y = Y;
			BF_Color_From = Vector4_Create(
					IT->Image_Edit->Data[(X + IT->Image_Edit->Width * Y) * 4],
					IT->Image_Edit->Data[(X + IT->Image_Edit->Width * Y) * 4 + 1],
					IT->Image_Edit->Data[(X + IT->Image_Edit->Width * Y) * 4 + 2],
					IT->Image_Edit->Data[(X + IT->Image_Edit->Width * Y) * 4 + 3]);
			BF_Color_To = IE->CurrentColor;

			/* Flood filling algorithm can take a couple of milliseconds on large images, so a thread is created to run it */
			thrd_t Thread;
			thrd_create(&Thread, Bucket_Fill, NULL);
		}
		else if (IE->Button_PanViewOn && Mouse.justPressed)
		{
			IE->PanningView = true;
		}
	}
	if (IE->PanningView)
	{
		double X = Mouse.x - Mouse.prevX;
		double Y = Mouse.y - Mouse.prevY;
		X = View_TranslateTo(X,
				IE->Image_Editor_View.v4.x - IE->Image_Editor_View.v1.x,
				IE->Image_View.z);
		Y = View_TranslateTo(Y,
				IE->Image_Editor_View.v2.y - IE->Image_Editor_View.v1.y,
				IE->Image_View.w);
		if (Mouse.x > Mouse.prevX && !X)
			X = 1;
		if (Mouse.x < Mouse.prevX && !X)
			X = -1;
		if (Mouse.y > Mouse.prevY && !Y)
			Y = 1;
		if (Mouse.y < Mouse.prevY && !Y)
			Y = -1;
		IT->X -= X;
		IT->Y += Y;

		if (Mouse.justReleased)
			IE->PanningView = false;
	}

	Gui_TextBox_Render(IE->Name);
	Gui_TextBox_Render(IE->Color_Red);
	Gui_TextBox_Render(IE->Color_Green);
	Gui_TextBox_Render(IE->Color_Blue);
	Gui_TextBox_Render(IE->Color_Alpha);
}

void Image_Editor_Free(struct Image_Editor **IE)
{
	Image_Free((*IE)->Image_Texture);
	Image_Free((*IE)->Button_PencilTexture);
	Image_Free((*IE)->Button_PickColorTexture);
	Image_Free((*IE)->Button_PanViewTexture);
	Image_Free((*IE)->Button_LoadTexture);
	Image_Free((*IE)->Button_SaveTexture);
	Image_Free((*IE)->Change_Confirm_Texture);
	Image_Free((*IE)->Close_Texture);
	Image_Free((*IE)->Delete_Texture);
	Image_Free((*IE)->New_Texture);

	Gui_TextBox_Free(&(*IE)->Color_Red);
	Gui_TextBox_Free(&(*IE)->Color_Green);
	Gui_TextBox_Free(&(*IE)->Color_Blue);
	Gui_TextBox_Free(&(*IE)->Color_Alpha);
	Gui_TextBox_Free(&(*IE)->Name);

	struct Image_Tab *IM = (*IE)->Image_Tab_List->items;
	for(int i = 0; i < (*IE)->Image_Tab_List->size; i++)
	{
		Image_Tab_Close(IM + i);
	}
	vector_delete((*IE)->Image_Tab_List);

	free((*IE));
	(*IE) = NULL;
}

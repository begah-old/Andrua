/*
 * Util_Gui.c
 *
 *  Created on: May 31, 2015
 *      Author: begah
 */

#include "Util.h"

struct Gui_TextBox *Gui_TextBox_Create(float X, float Y, float Width,
									   float Height, const char *Name, GLint MaxTextLength, float TextOffsetX,
									   float TextOffsetY, float TextHeight, float MaxTextWidth,
									   float ColorR, float ColorG,
									   float ColorB, float ColorA, float HoverColorR, float HoverColorG,
									   float HoverColorB, float HoverColorA)
{
	struct Gui_TextBox *TB = malloc(sizeof(struct Gui_TextBox));

	TB->X = X;
	TB->Y = Y;
	TB->Width = Width;
	TB->Height = Height;

	TB->Original_Width = Game_Width;
	TB->Original_Height = Game_Height;

	int Len = strlen(Name);
	TB->Text = malloc(sizeof(char) * (Len + MaxTextLength + 1));
	TB->Text[Len + MaxTextLength] = '\0';
	String_Copy(TB->Text, Name);
	TB->Value = TB->Text + Len;
	for (int Char = 0; Char < MaxTextLength; Char++)
	{
		TB->Value[Char] = '\0';
	}

	TB->ValueLength = MaxTextLength;

	TB->NotCentered = false;

	TB->Writing = false;

	TB->TextOffSetX = TextOffsetX;
	TB->TextOffSetY = TextOffsetY;
	TB->TextHeight = TextHeight;
	TB->TextMaxWidth = MaxTextWidth;

	TB->Color = Vector4_Create(ColorR, ColorG, ColorB, ColorA);
	TB->HoverColor = Vector4_Create(HoverColorR, HoverColorG, HoverColorB,
									HoverColorA);

	TB->Hover = false;

	TB->AcceptDot = TB->AcceptLetters = TB->AcceptNumbers = TB->AcceptSymbols =
			false;

	TB->Font_Color = COLOR_WHITE;

	TB->Quad_takeMouse = Quad_Create(0, 0, 0, Game_Height, Game_Width, Game_Height, Game_Width, 0);

	return TB;
}

static void Gui_TextBox_Check_Key(struct Gui_TextBox *TB);

void Gui_TextBox_Render(struct Gui_TextBox *TB) {
	struct Quad Quad = Quad_Create(
			(float) TB->X / TB->Original_Width * Game_Width,
			(float) TB->Y / TB->Original_Height * Game_Height,
			(float) TB->X / TB->Original_Width * Game_Width,
			(float) TB->Y / TB->Original_Height * Game_Height
			+ (float) TB->Height / TB->Original_Height * Game_Height,
			(float) TB->X / TB->Original_Width * Game_Width
			+ (float) TB->Width / TB->Original_Width * Game_Width,
			(float) TB->Y / TB->Original_Height * Game_Height
			+ (float) TB->Height / TB->Original_Height * Game_Height,
			(float) TB->X / TB->Original_Width * Game_Width
			+ (float) TB->Width / TB->Original_Width * Game_Width,
			(float) TB->Y / TB->Original_Height * Game_Height);

	TB->Hover = Point_inQuad(Vector2_Create((float) Mouse.x, (float) Mouse.y),
							 Quad);
	if (Mouse.justPressed && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), TB->Quad_takeMouse)) {
		TB->Writing = TB->Hover;
	}
	Default_Shader.pushQuad(Quad, TB->Hover ? TB->HoverColor : TB->Color);

	if (TB->NotCentered)
		Font_HeightMaxRender(DefaultFontManager, TB->Text,
				(TB->X + TB->TextOffSetX) / (float) TB->Original_Width
						* Game_Width,
				(TB->Y + TB->TextOffSetY) / (float) TB->Original_Height
						* Game_Height,
				TB->TextHeight / (float) TB->Original_Height * Game_Height,
				TB->TextMaxWidth / (float) TB->Original_Width * Game_Width,
				1.0f, TB->Font_Color);
	else
		Font_HeightMaxRenderCA(DefaultFontManager, TB->Text, TB->X + TB->Width / 2.0f,
				(TB->Y + TB->TextOffSetY) / (float) TB->Original_Height
						* Game_Height,
				TB->TextHeight / (float) TB->Original_Height * Game_Height,
				TB->TextMaxWidth / (float) TB->Original_Width * Game_Width,
				1.0f, TB->Font_Color);

	Gui_TextBox_Check_Key(TB);
}

static void Gui_TextBox_Check_Key(struct Gui_TextBox *TB)
{
	if (!TB->Writing)
		return;

	unsigned int Key;
	if (Keyboard.justPressed && Keyboard.Key.key == GLFW_KEY_BACKSPACE)
		Key = -1;
	else if (Keyboard.justTyped)
		Key = Keyboard.Character;
	else
		return;

	char C = (char) Key;
	if (Key == -1)
	{
		for (int Char = TB->ValueLength; Char >= 0; Char--)
		{
			if (TB->Value[Char] != 0)
			{
				TB->Value[Char] = 0;
				return;
			}
		}
	}
	else
	{
		_Bool Read = false;
		if (TB->AcceptLetters && (((C >= 'A') & (C <= 'z')) | (C == ' ')))
			Read = true;
		else if (TB->AcceptDot && C == '.')
			Read = true;
		else if (TB->AcceptNumbers && C >= '0' && C <= '9')
			Read = true;
		else if (TB->AcceptSymbols && ((C < 'a') | (C > 'Z'))
				 && ((C < '0') | (C > '9')) && C != '.')
			Read = true;

		if (!Read)
			return;
		for (int Char = 0; Char < TB->ValueLength; Char++)
		{
			if (TB->Value[Char] == 0)
			{
				TB->Value[Char] = C;
				return;
			}
		}
	}
}

void Gui_TextBox_ClearText(struct Gui_TextBox *TB)
{
	for (int i = 0; i < TB->ValueLength; i++)
	{
		if (!TB->Value[i])
			return;
		TB->Value[i] = '\0';
	}
}

void Gui_TextBox_ResizeText(struct Gui_TextBox *TB, float TextOffsetX,
							float TextOffsetY, float TextWidth, float TextHeight)
{
	TB->TextOffSetX = TextOffsetX;
	TB->TextOffSetY = TextOffsetY;
	TB->TextMaxWidth = TextWidth;
	TB->TextHeight = TextHeight;
}

void Gui_TextBox_Resize(struct Gui_TextBox *TB, float X, float Y, float Width,
						float Height)
{
	TB->X = X;
	TB->Y = Y;
	TB->Width = Width;
	TB->Height = Height;
	TB->Original_Width = Game_Width;
	TB->Original_Height = Game_Height;
}

void Gui_TextBox_Free(struct Gui_TextBox **TB)
{
	free((*TB)->Text);
	free((*TB));
	(*TB) = NULL;
}

void Gui_TextBox_Free_Simple(struct Gui_TextBox *TB)
{
	free(TB->Text);
}

struct Gui_Button *Gui_Button_Create(float X, float Y, float Width,
									 float Height, const char *Name, float TextOffsetX, float TextOffsetY,
									 float TextWidth, float TextHeight, float ColorR, float ColorG, float ColorB, float ColorA,
									 float HoverColorR, float HoverColorG, float HoverColorB,
									 float HoverColorA)
{
	struct Gui_Button *TB = malloc(sizeof(struct Gui_Button));

	TB->X = X;
	TB->Y = Y;
	TB->Width = Width;
	TB->Height = Height;

	TB->Text = malloc(sizeof(char) * (String_length(Name) + 1));
	TB->Text = memcpy(TB->Text, Name, sizeof(char) * (String_length(Name) + 1));

	TB->TextOffSetX = TextOffsetX;
	TB->TextOffSetY = TextOffsetY;
	TB->TextHeight = TextHeight;
	TB->TextWidth = TextWidth;
	TB->Color = Vector4_Create(ColorR, ColorG, ColorB, ColorA);
	TB->HoverColor = Vector4_Create(HoverColorR, HoverColorG, HoverColorB,
									HoverColorA);

	TB->Original_Width = Game_Width;
	TB->Original_Height = Game_Height;

	TB->Hover = false;
	TB->CenterText = false;

	TB->Font_Color = COLOR_WHITE;

	return TB;
}

_Bool Gui_Button_Render(struct Gui_Button *TB)
{
	struct Quad Quad = Quad_Create(
			(float) TB->X,
			(float) TB->Y,
			(float) TB->X,
			(float) TB->Y + (float) TB->Height,
			(float) TB->X + (float) TB->Width,
			(float) TB->Y + (float) TB->Height,
			(float) TB->X + (float) TB->Width,
			(float) TB->Y);

	TB->Hover = Point_inQuad(Vector2_Create((float) Mouse.x, (float) Mouse.y),
							 Quad);

	Default_Shader.pushQuad(Quad, TB->Hover ? TB->HoverColor : TB->Color);

	if(TB->CenterText)
		Font_HeightMaxRenderCA(DefaultFontManager, TB->Text, TB->X + TB->Width / 2.0f, TB->Y, TB->TextHeight, TB->TextWidth, 1.0f, TB->Font_Color);
	else
		Font_HeightMaxRender(DefaultFontManager, TB->Text,
						 (TB->X + TB->TextOffSetX),
						 (TB->Y + TB->TextOffSetY),
						 TB->TextHeight,
						 TB->TextWidth, 1.0f,
						 TB->Font_Color);
	return Mouse.justPressed && TB->Hover;
}

void Gui_Button_Resize(struct Gui_Button *TB, float X, float Y, float Width,
					   float Height)
{
	TB->X = X;
	TB->Y = Y;
	TB->Width = Width;
	TB->Height = Height;
	TB->Original_Width = Game_Width;
	TB->Original_Height = Game_Height;
}

void Gui_Button_ResizeText(struct Gui_Button *TB, float TextOffsetX,
						   float TextOffsetY, float TextWidth, float TextHeight)
{
	TB->TextOffSetX = TextOffsetX;
	TB->TextOffSetY = TextOffsetY;
	TB->TextWidth = TextWidth;
	TB->TextHeight = TextHeight;
}

void Gui_Button_Free(struct Gui_Button **TB)
{
	free((*TB)->Text);
	free((*TB));
	(*TB) = NULL;
}

void Gui_Button_Free_Simple(struct Gui_Button *TB)
{
	free(TB->Text);
}

struct Gui_Tab *Gui_Tab_Create(char *Name, float X, float Y, float Width,
							   float Height, float ItemHeight, float ItemsInView,
							   struct FontManager *FM, GLint FontID, void *DataToPass, _Bool CopyName)
{
	struct Gui_Tab *Tab = malloc(sizeof(struct Gui_Tab));
	Tab->X = X;
	Tab->Y = Y;
	Tab->Width = Width;
	Tab->Height = Height;
	Tab->ItemHeight = ItemHeight;
	Tab->DataToPass = DataToPass;
	Tab->Hover = false;
	Tab->ScrollValue = 0;
	Tab->FM = FM;
	Tab->FontID = FontID;
	Tab->ItemsInView = ItemsInView;
	Tab->CopyName = CopyName;

	Tab->Name = malloc(sizeof(char) * (String_length(Name) + 1));
	Tab->Name = memcpy(Tab->Name, Name, sizeof(char) * String_length(Name));
	Tab->Name[String_length(Name)] = '\0';

	Tab->Item_Selected = -1;

	float RestHeight = Height - (ItemHeight * ItemsInView);

	Tab->Tab = Quad_Create(X, Y, X, Y + Height, X + Width, Y + Height,
						   X + Width, Y);
	Tab->Tab_Color = Vector4_Create(0.8f, 0.8f, 0.8f, 1.0f);

	Tab->Create = Gui_Button_Create(X, Y + Height - RestHeight, Width,
									RestHeight / 2.0f, "Create", 0, 0,
									(Width - 2) / 640.0f * Game_Width,
									(RestHeight / 2.0) / 480.0f * Game_Height, 1.0f, 1.0f,
									1.0f, 1.0f, 0.90f, 0.90f, 0.90f, 1.0f);

	Tab->ItemColor = Vector4_Create(0.90f, 0.90f, 0.90f, 1.0f);
	Tab->ItemColorHover = Vector4_Create(0.80f, 0.80f, 0.80f, 1.0f);

	Tab->Items = vector_new(sizeof(struct String_Struct));

	Tab->Font_Color = COLOR_RED;
	Tab->onItemClick = NULL;
	Tab->onItemCreate = NULL;
	return Tab;
}

void Gui_Tab_Add(struct Gui_Tab *Tab, char *Name)
{
	struct String_Struct *SS = malloc(sizeof(struct String_Struct));
	if (Tab->CopyName)
	{
		SS->String = malloc(sizeof(char) * (String_length(Name) + 1));
		SS->String = memcpy(SS->String, Name,
							sizeof(char) * String_length(Name));
		SS->String[String_length(Name)] = '\0';
	}
	else
		SS->String = Name;
	vector_push_back(Tab->Items, SS);
	free(SS);
}

void Gui_Tab_NewString(struct Gui_Tab *Tab, int ID, char *String)
{
	if (!Tab->CopyName)
	{
		((struct String_Struct *) Tab->Items->items + ID)->String = String;
		return;
	}
	struct String_Struct *SS = (struct String_Struct *) Tab->Items->items + ID;
	free(SS->String);
	SS->String = malloc(sizeof(char) * (String_length(String) + 1));
	SS->String = memcpy(SS->String, String,
						sizeof(char) * String_length(String));
	SS->String[String_length(String)] = '\0';
}

void Gui_Tab_CheckScroll(struct Gui_Tab *Tab)
{
	if (!Tab->Items->size || !Tab->Hover || !Mouse.justScrolled)
		return;
	struct Quad Quad = Quad_Create(Tab->X,
								   Tab->Create->Y - Tab->ItemHeight
								   - Tab->ItemHeight * ((int) Tab->Items->size - 1)
								   - (float) Tab->ScrollValue / 480 * Game_Height,
								   Game_Width / 6 * 5,
								   Tab->Create->Y - Tab->ItemHeight * ((int) Tab->Items->size - 1)
								   - (float) Tab->ScrollValue / 480 * Game_Height, Game_Width,
								   Tab->Create->Y - Tab->ItemHeight * ((int) Tab->Items->size - 1)
								   - (float) Tab->ScrollValue / 480 * Game_Height, Game_Width,
								   (float) Tab->Create->Y - Tab->ItemHeight
								   - (float) Tab->ItemHeight * ((int) Tab->Items->size - 1)
								   - (float) Tab->ScrollValue / 480 * Game_Height);
	if (Quad.v4.y / Game_Height * 480 >= Tab->Y && (int) Mouse.scrollY < 0)
		return;

	Tab->ScrollValue += (int) Mouse.scrollY * 4;
	if (Tab->ScrollValue > 0)
		Tab->ScrollValue = 0;
}

void Gui_Tab_Render(struct Gui_Tab *Tab)
{
	Tab->Hover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), Tab->Tab);
	// Drawing Tab
	Default_Shader.pushQuad(Tab->Tab, Tab->Tab_Color);

	// Drawing Items
	_Bool ItemPressed = false;

	struct String_Struct *Names = (struct String_Struct *) Tab->Items->items;
	for (int i = 0; i < Tab->Items->size; i++)
	{
		struct Quad Quad = Quad_Create(Tab->X,
									   Tab->Create->Y - Tab->ItemHeight - Tab->ItemHeight * i
									   - (float) Tab->ScrollValue / 480 * Game_Height, Tab->X,
									   Tab->Create->Y - Tab->ItemHeight * i
									   - (float) Tab->ScrollValue / 480 * Game_Height,
									   Tab->X + Tab->Width,
									   Tab->Create->Y - Tab->ItemHeight * i
									   - (float) Tab->ScrollValue / 480 * Game_Height,
									   Tab->X + Tab->Width,
									   Tab->Create->Y - Tab->ItemHeight - Tab->ItemHeight * i
									   - (float) Tab->ScrollValue / 480 * Game_Height);
		if (Quad.v4.y > Tab->Create->Y)
			continue;
		else if (Quad.v3.y < Tab->Y)
			continue;
		if (Quad.v4.y < Tab->Y)
			Quad.v4.y = Quad.v1.y = Tab->Y;
		if (Quad.v3.y > Tab->Create->Y)
			Quad.v2.y = Quad.v3.y = Tab->Create->Y;

		_Bool Hover = Point_inQuad(
				Vector2_Create((float) Mouse.x, (float) Mouse.y), Quad);

		if (Tab->Item_Selected == i
			|| Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), Quad))
			Default_Shader.pushQuad(Quad, Tab->ItemColorHover);
		else
			Default_Shader.pushQuad(Quad, Tab->ItemColor);

		Font_HeightMaxRender(DefaultFontManager, Names[i].String,
							 Tab->X + 0.05f, Quad.v4.y, Quad.v3.y - Quad.v4.y, Tab->Width,
							 1.0f, COLOR_WHITE);

		if (Hover && Mouse.justPressed)
		{
			ItemPressed = true;
			Tab->Item_Selected = i;
		}
	}

	_Bool CreatePressed = Gui_Button_Render(Tab->Create);

	Font_Use(Tab->FM, Tab->FontID);
	Font_FixedRender(Tab->FM, Tab->Name, Tab->X,
					 Tab->Create->Y + Tab->Create->Height, Tab->Create->Height,
					 Tab->Width, 1.0f, Tab->Font_Color);

	if (ItemPressed && Tab->onItemClick)
		Tab->onItemClick(Tab->DataToPass, Tab->Item_Selected);
	if (CreatePressed && Tab->onItemCreate)
		Tab->onItemCreate(Tab->DataToPass);

	Gui_Tab_CheckScroll(Tab);
}

int Gui_Tab_SimpleRender(struct Gui_Tab *Tab)
{
	Tab->Hover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), Tab->Tab);
	// Drawing Tab
	Default_Shader.pushQuad(Tab->Tab, Tab->Tab_Color);

	struct String_Struct *Names = (struct String_Struct *) Tab->Items->items;
	for (int i = 0; i < Tab->Items->size; i++)
	{
		struct Quad Quad = Quad_Create(Tab->X,
									   Tab->Y + Tab->Height - Tab->ItemHeight - Tab->ItemHeight * i
									   - (float) Tab->ScrollValue / 480 * Game_Height, Tab->X,
									   Tab->Y + Tab->Height - Tab->ItemHeight * i
									   - (float) Tab->ScrollValue / 480 * Game_Height,
									   Tab->X + Tab->Width,
									   Tab->Y + Tab->Height - Tab->ItemHeight * i
									   - (float) Tab->ScrollValue / 480 * Game_Height,
									   Tab->X + Tab->Width,
									   Tab->Y + Tab->Height - Tab->ItemHeight - Tab->ItemHeight * i
									   - (float) Tab->ScrollValue / 480 * Game_Height);
		if (Quad.v4.y > Tab->Y + Tab->Height)
			continue;
		else if (Quad.v3.y < Tab->Y)
			continue;
		if (Quad.v4.y < Tab->Y)
			Quad.v4.y = Quad.v1.y = Tab->Y;
		if (Quad.v3.y > Tab->Y + Tab->Height)
			Quad.v2.y = Quad.v3.y = Tab->Y + Tab->Height;

		_Bool Hover = Point_inQuad(
				Vector2_Create((float) Mouse.x, (float) Mouse.y), Quad);

		if (Tab->Item_Selected == i
			|| Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), Quad))
			Default_Shader.pushQuad(Quad, Tab->ItemColorHover);
		else
			Default_Shader.pushQuad(Quad, Tab->ItemColor);

		Font_HeightMaxRender(DefaultFontManager, Names[i].String,
							 Tab->X + 0.05f, Quad.v4.y, Quad.v3.y - Quad.v4.y, Tab->Width,
							 1.0f, COLOR_WHITE);

		if (Hover && Mouse.justPressed)
		{
			Tab->Item_Selected = i;
		}
	}

	Gui_Tab_CheckScroll(Tab);
	return Tab->Item_Selected;
}

void Gui_Tab_Resize(struct Gui_Tab *Tab, float X, float Y, float Width,
					float Height, float ItemHeight)
{
	Tab->X = X;
	Tab->Y = Y;
	Tab->Width = Width;
	Tab->Height = Height;
	Tab->ItemHeight = ItemHeight;

	float RestHeight = Height - (ItemHeight * Tab->ItemsInView);

	Tab->Tab = Quad_Create(X, Y, X, Y + Height, X + Width, Y + Height,
						   X + Width, Y);
	Gui_Button_Resize(Tab->Create, X, Y + Height - RestHeight, Width,
					  RestHeight / 2.0f);
	Gui_Button_ResizeText(Tab->Create, 2.0f / 640 * Game_Width,
						  2.0f / 480 * Game_Height, Width - (2 / 640.0f * Game_Width),
						  RestHeight / 2.0);
}

void Gui_Tab_Free(struct Gui_Tab **TB)
{
	free((*TB)->Name);

	struct String_Struct *Names = (struct String_Struct *) (*TB)->Items->items;
	if ((*TB)->CopyName)
		for (int i = 0; i < (*TB)->Items->size; i++)
			free(Names[i].String);
	vector_delete((*TB)->Items);

	Gui_Button_Free(&(*TB)->Create);

	free((*TB));
	(*TB) = NULL;
}

struct Gui_Horizontal_ScrollBar *Gui_Horizontal_ScrollBar_Create(float x,
																 float y, float width, float height, struct Vector4f Color,
																 struct Vector4f Bar_Color, struct Vector4f Bar_Color_Hover,
																 float BarValue, float TotalValue, void (*onBarScrolling)(void *))
{
	struct Gui_Horizontal_ScrollBar *ScrollBar = malloc(
			sizeof(struct Gui_Horizontal_ScrollBar));
	ScrollBar->ScrollBar = Quad_Create(x, y, x, y + height, x + width,
									   y + height, x + width, y);

	ScrollBar->Color = Color;
	ScrollBar->Bar_Color = Bar_Color;
	ScrollBar->Bar_Color_Hover = Bar_Color_Hover;

	ScrollBar->BarValue = BarValue;
	ScrollBar->TotalValue = TotalValue;

	ScrollBar->BarX = 0;
	ScrollBar->IsPressed = false;

	ScrollBar->Data = NULL;
	ScrollBar->onBarScrolling = onBarScrolling;
	return ScrollBar;
}

void Gui_Horizontal_ScrollBar_Render(struct Gui_Horizontal_ScrollBar *ScrollBar)
{
	Default_Shader.pushQuad(ScrollBar->ScrollBar, ScrollBar->Color);

	// Make sure ScrollBar isn't out of bounds
	if(ScrollBar->BarX > ScrollBar->TotalValue)
		ScrollBar->BarX = ScrollBar->TotalValue;
	else if(ScrollBar->BarX < 0)
		ScrollBar->BarX = 0;

	ScrollBar->BarX = View_TranslateTo(ScrollBar->BarX, ScrollBar->TotalValue,
									   ScrollBar->ScrollBar.v3.x - ScrollBar->ScrollBar.v1.x);

	if (ScrollBar->BarValue <= ScrollBar->TotalValue)
	{
		float Ratio = ScrollBar->BarValue / (ScrollBar->TotalValue);
		float Width = ScrollBar->ScrollBar.v3.x - ScrollBar->ScrollBar.v1.x;
		Width *= Ratio;

		struct Quad BarQuad = Quad_Create(
				ScrollBar->BarX + ScrollBar->ScrollBar.v1.x,
				ScrollBar->ScrollBar.v1.y,
				ScrollBar->BarX + ScrollBar->ScrollBar.v1.x,
				ScrollBar->ScrollBar.v2.y,
				ScrollBar->BarX + ScrollBar->ScrollBar.v1.x + Width,
				ScrollBar->ScrollBar.v3.y,
				ScrollBar->BarX + ScrollBar->ScrollBar.v1.x + Width,
				ScrollBar->ScrollBar.v4.y);

		_Bool Hover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), BarQuad);
		if (!Hover && ScrollBar->IsPressed)
			Hover = true;

		if (Hover)
			Default_Shader.pushQuad(BarQuad, ScrollBar->Bar_Color_Hover);
		else
			Default_Shader.pushQuad(BarQuad, ScrollBar->Bar_Color);

		if (ScrollBar->IsPressed && Mouse.x != Mouse.prevX)
		{
			float XX = (Mouse.x - Mouse.prevX);
			ScrollBar->BarX += XX;
			if(ScrollBar->BarX < 0)
				ScrollBar->BarX = 0;
			if(BarQuad.v3.x > ScrollBar->ScrollBar.v3.x)
				ScrollBar->BarX = ScrollBar->ScrollBar.v3.x - Width;

			if (ScrollBar->onBarScrolling)
				ScrollBar->onBarScrolling(ScrollBar->Data);
		}

		if (Mouse.justReleased)
			ScrollBar->IsPressed = false;
		else if (Mouse.justPressed && Hover)
			ScrollBar->IsPressed = true;
		else if (Mouse.justPressed
				 && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y),
								 ScrollBar->ScrollBar))
		{
			ScrollBar->BarX = Mouse.x - ScrollBar->ScrollBar.v1.x - Width / 2;
			ScrollBar->IsPressed = true;
		}

		if (ScrollBar->BarX + Width
			> ScrollBar->ScrollBar.v3.x - ScrollBar->ScrollBar.v1.x)
			ScrollBar->BarX = (ScrollBar->ScrollBar.v3.x
							   - ScrollBar->ScrollBar.v1.x) - Width;
		if (ScrollBar->BarX < 0)
			ScrollBar->BarX = 0;
	}
	else
	{
		_Bool Hover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y),
								   ScrollBar->ScrollBar);
		if (Hover)
			Default_Shader.pushQuad(ScrollBar->ScrollBar,
									ScrollBar->Bar_Color_Hover);
		else
			Default_Shader.pushQuad(ScrollBar->ScrollBar, ScrollBar->Bar_Color);
	}
	ScrollBar->BarX = View_TranslateTo(ScrollBar->BarX,
									   ScrollBar->ScrollBar.v3.x - ScrollBar->ScrollBar.v1.x,
									   ScrollBar->TotalValue);
}

void Gui_Horizontal_ScrollBar_Resize(struct Gui_Horizontal_ScrollBar *ScrollBar,
									 float x, float y, float w, float h, float barValue, float totalValue)
{
	ScrollBar->ScrollBar = Quad_Create(x, y, x, y + h, x + w,
									   y + h, x + w, y);
	ScrollBar->BarX = (ScrollBar->BarX / ScrollBar->TotalValue) * totalValue;
	ScrollBar->BarValue = barValue;
	ScrollBar->TotalValue = totalValue;
}

void Gui_Horizontal_ScrollBar_Free(struct Gui_Horizontal_ScrollBar **ScrollTab)
{
	free((*ScrollTab));
	(*ScrollTab) = NULL;
}

struct Gui_Vertical_ScrollBar *Gui_Vertical_ScrollBar_Create(float x,
																 float y, float width, float height, struct Vector4f Color,
																 struct Vector4f Bar_Color, struct Vector4f Bar_Color_Hover,
																 float BarValue, float TotalValue, void (*onBarScrolling)(void *, float))
{
	struct Gui_Vertical_ScrollBar *ScrollBar = malloc(
			sizeof(struct Gui_Vertical_ScrollBar));
	ScrollBar->ScrollBar = Quad_Create(x, y, x, y + height, x + width,
									   y + height, x + width, y);

	ScrollBar->Color = Color;
	ScrollBar->Bar_Color = Bar_Color;
	ScrollBar->Bar_Color_Hover = Bar_Color_Hover;

	ScrollBar->BarValue = BarValue;
	ScrollBar->TotalValue = TotalValue;

	ScrollBar->BarY = 0;
	ScrollBar->IsPressed = false;

	ScrollBar->Inversed = false;

	ScrollBar->Data = NULL;
	ScrollBar->onBarScrolling = onBarScrolling;
	return ScrollBar;
}

void Gui_Vertical_ScrollBar_Inverse(struct Gui_Vertical_ScrollBar *ScrollBar)
{
	ScrollBar->Inversed = !ScrollBar->Inversed;
	ScrollBar->BarY = -ScrollBar->BarY;
}

void Gui_Vertical_ScrollBar_Render(struct Gui_Vertical_ScrollBar *ScrollBar)
{
	Default_Shader.pushQuad(ScrollBar->ScrollBar, ScrollBar->Color);

	if(ScrollBar->Inversed)
		ScrollBar->BarY = -ScrollBar->BarY;

	// Make sure ScrollBar isn't out of bounds
	ScrollBar->BarY = View_TranslateTo(ScrollBar->BarY, ScrollBar->TotalValue,
			ScrollBar->ScrollBar.v2.y - ScrollBar->ScrollBar.v1.y);

	if(ScrollBar->Inversed)
	{

	} else
	{
		if(ScrollBar->BarY > ScrollBar->TotalValue)
			ScrollBar->BarY = ScrollBar->TotalValue;
		else if(ScrollBar->BarY < 0)
			ScrollBar->BarY = 0;
	}

	if (ScrollBar->BarValue <= ScrollBar->TotalValue)
	{
		float Ratio = ScrollBar->BarValue / (ScrollBar->TotalValue);
		float Height = ScrollBar->ScrollBar.v2.y - ScrollBar->ScrollBar.v1.y;
		Height *= Ratio;

		struct Quad BarQuad;
		if(ScrollBar->Inversed)
			BarQuad = Quad_Create(ScrollBar->ScrollBar.v1.x,
							ScrollBar->ScrollBar.v2.y - ScrollBar->BarY,
							ScrollBar->ScrollBar.v2.x,
							ScrollBar->ScrollBar.v2.y - ScrollBar->BarY - Height,
							ScrollBar->ScrollBar.v3.x,
							ScrollBar->ScrollBar.v2.y - ScrollBar->BarY - Height,
							ScrollBar->ScrollBar.v4.x,
							ScrollBar->ScrollBar.v2.y - ScrollBar->BarY);
		else
			BarQuad = Quad_Create(ScrollBar->ScrollBar.v1.x,
					ScrollBar->BarY + ScrollBar->ScrollBar.v1.y,
					ScrollBar->ScrollBar.v2.x,
					ScrollBar->BarY + ScrollBar->ScrollBar.v1.y + Height,
					ScrollBar->ScrollBar.v3.x,
					ScrollBar->BarY + ScrollBar->ScrollBar.v1.y + Height,
					ScrollBar->ScrollBar.v4.x,
					ScrollBar->BarY + ScrollBar->ScrollBar.v1.y);

		_Bool Hover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), BarQuad);
		if (!Hover && ScrollBar->IsPressed)
			Hover = true;

		if (Hover)
			Default_Shader.pushQuad(BarQuad, ScrollBar->Bar_Color_Hover);
		else
			Default_Shader.pushQuad(BarQuad, ScrollBar->Bar_Color);

		if (ScrollBar->IsPressed && Mouse.y != Mouse.prevY)
		{
			float YY = (Mouse.y - Mouse.prevY);

			if(ScrollBar->Inversed) {
				if(YY < 0 && ScrollBar->ScrollBar.v2.y - (ScrollBar->BarY + YY * -1 + Height)
						< ScrollBar->ScrollBar.v1.y)
					goto Dont_Scroll;
			} else {

			}

			if (ScrollBar->onBarScrolling)
				ScrollBar->onBarScrolling(ScrollBar->Data, YY);

			if(ScrollBar->Inversed)
				ScrollBar->BarY -= YY;
			else
				ScrollBar->BarY += YY;

			/*if(ScrollBar->BarY < 0)
				ScrollBar->BarY = 0;
			if(BarQuad.v2.y > ScrollBar->ScrollBar.v2.y)
				ScrollBar->BarY = ScrollBar->ScrollBar.v2.y - Height;*/

			Dont_Scroll:;
		}

		if (Mouse.justReleased)
			ScrollBar->IsPressed = false;
		else if (Mouse.justPressed && Hover)
			ScrollBar->IsPressed = true;
		else if (Mouse.justPressed && Point_inQuad(Vector2_Create(Mouse.x, Mouse.y), ScrollBar->ScrollBar))
		{
			if(ScrollBar->Inversed)
				ScrollBar->BarY = ScrollBar->ScrollBar.v2.y - Mouse.y - Height / 2;
			else
				ScrollBar->BarY = Mouse.y - ScrollBar->ScrollBar.v2.y - Height / 2;
			ScrollBar->IsPressed = true;
		}

		if (!ScrollBar->Inversed && ScrollBar->BarY + Height
			> ScrollBar->ScrollBar.v2.y - ScrollBar->ScrollBar.v1.y)
			ScrollBar->BarY = (ScrollBar->ScrollBar.v2.y
							   - ScrollBar->ScrollBar.v1.y) - Height;
		if (ScrollBar->Inversed && ScrollBar->ScrollBar.v2.y - ScrollBar->BarY - Height < ScrollBar->ScrollBar.v1.y)
			ScrollBar->BarY = ScrollBar->ScrollBar.v2.y - ScrollBar->ScrollBar.v1.y - Height;

		if (ScrollBar->BarY < 0)
			ScrollBar->BarY = 0;
	}
	else
	{
		_Bool Hover = Point_inQuad(Vector2_Create(Mouse.x, Mouse.y),
								   ScrollBar->ScrollBar);
		if (Hover)
			Default_Shader.pushQuad(ScrollBar->ScrollBar,
									ScrollBar->Bar_Color_Hover);
		else
			Default_Shader.pushQuad(ScrollBar->ScrollBar, ScrollBar->Bar_Color);
	}

	ScrollBar->BarY = View_TranslateTo(ScrollBar->BarY,
									   ScrollBar->ScrollBar.v2.y - ScrollBar->ScrollBar.v1.y,
									   ScrollBar->TotalValue);

	if(ScrollBar->Inversed)
		ScrollBar->BarY = -ScrollBar->BarY;
}

void Gui_Vertical_ScrollBar_Resize(struct Gui_Vertical_ScrollBar *ScrollBar,
									 float x, float y, float w, float h, float barValue, float totalValue)
{
	ScrollBar->ScrollBar = Quad_Create(x, y, x, y + h, x + w,
									   y + h, x + w, y);
	ScrollBar->BarY = (ScrollBar->BarY / ScrollBar->TotalValue) * totalValue;
	ScrollBar->BarValue = barValue;
	ScrollBar->TotalValue = totalValue;
}

void Gui_Vertical_ScrollBar_Free(struct Gui_Vertical_ScrollBar **ScrollTab)
{
	free((*ScrollTab));
	(*ScrollTab) = NULL;
}

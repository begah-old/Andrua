/*
 * Util_Font.c
 *
 *  Created on: May 23, 2015
 *      Author: begah
 */

#include "Util.h"

struct FontManager *Font_Init()
{

	COLOR_RED = Vector4_Create(1.0f, 0.0f, 0.0f, 1.0f);
	COLOR_GREEN = Vector4_Create(0.0f, 1.0f, 0.0f, 1.0f);
	COLOR_BLUE = Vector4_Create(0.0f, 0.0f, 1.0f, 1.0f);
	COLOR_BLACK = Vector4_Create(0.0f, 0.0f, 0.0f, 1.0f);
	COLOR_WHITE = Vector4_Create(1.0f, 1.0f, 1.0f, 1.0f);

	struct FontManager *Manager = malloc(sizeof(struct FontManager));
	check_mem(Manager);

	Manager->CurrentFont = NULL;

	Manager->Fonts = vector_new(sizeof(struct Font));

	Manager->FontNum = 0;
	printOpenGLError();
	return Manager;

	error: if (Manager)
		free(Manager);
	printOpenGLError();
	return NULL;
}

static void Font_AddTextureAtlas(struct Font *Font, FT_Face face, GLuint Width,
		GLuint Heigth)
{
	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned char *DataTemp = malloc(sizeof(unsigned char) * Width * Heigth);
	for (int X = 0; X < Width * Heigth; X++)
		DataTemp[X] = 0;
	glGenTextures(1, &Font->Texture);
	glBindTexture(GL_TEXTURE_2D, Font->Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, Width, Heigth, 0, GL_RED,
	GL_UNSIGNED_BYTE, DataTemp);

	free(DataTemp);

	// Set texture options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	struct PackAtlas *Atlas = PackAtlas_Init(Width, Heigth);

	for (GLubyte c = 0; c < 128; c++)
	{
		if (c != ' ' && c != '\t' && c != '\n')
		{
			// Load character glyph
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				log_err("FREETYTPE: Failed to load Glyph : %c value : %i", c, c);
				continue;
			}

			int x = 0, y = 0;
			if (!PackAtlas_Add(Atlas, face->glyph->bitmap.width + 2,
					face->glyph->bitmap.rows + 2, &x, &y))
			{
				Texture_Free(Font->Texture);
				PackAtlas_Free(Atlas);
				if (Width == 512 && Heigth == 512)
					Font_AddTextureAtlas(Font, face, 1024, 512);
				else if (Width == 1024 && Heigth == 512)
					Font_AddTextureAtlas(Font, face, 1024, 1024);
				else if (Width == 1024 && Heigth == 1024)
					Font_AddTextureAtlas(Font, face, 2048, 1024);
				else if (Width == 2048 && Heigth == 1024)
					Font_AddTextureAtlas(Font, face, 2048, 2048);
				else if (Width == 2048 && Heigth == 2048)
					Font_AddTextureAtlas(Font, face, 4096, 2048);
				else if (Width == 4096 && Heigth == 2048)
					Font_AddTextureAtlas(Font, face, 4096, 4096);
				else {
					log_err("Cannot load font, insufficient room");
					exit(1);
				}
				return;
			}

			glBindTexture(GL_TEXTURE_2D, Font->Texture);
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, face->glyph->bitmap.width,
					face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE,
					face->glyph->bitmap.buffer);

			Font->Characters[c].TexturePos.x = (float) x / Width;
			Font->Characters[c].TexturePos.y = (float) y / Heigth;
			Font->Characters[c].TexturePos.z = Font->Characters[c].TexturePos.x
					+ Font->Characters[c].Size.x / (float) Width;
			Font->Characters[c].TexturePos.w = Font->Characters[c].TexturePos.y
					+ Font->Characters[c].Size.y / (float) Heigth;
		}
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	PackAtlas_Free(Atlas);
}

GLint Font_Add(struct FontManager *Manager, char *Path, unsigned int Size)
{
	if (!Manager)
		return -1;
	struct Font *Font = malloc(sizeof(struct Font));
	if (!Font)
		return -1;

	// FreeType
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
	{
		log_err("Font_Add : Could not init FreeType Library");
		return -1;
	}

	// Load font as face
	FT_Face face;
	if (FT_New_Face(ft, Path, 0, &face))
	{
		log_err("Font_Add : Failed to load font : %s", Path);
		FT_Done_FreeType(ft);
		return -1;
	}

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, Size);

	int LowestCharacter = 0, HighestCharacter = 0;

	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph
		FT_Load_Char(face, c, FT_LOAD_RENDER);
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			log_err("FREETYTPE: Failed to load Glyph : %c value : %i",
					c, c);
			continue;
		}

		// Now store character for later use
		Font->Characters[c].Size = Vector2_Create(face->glyph->bitmap.width,
				face->glyph->bitmap.rows);
		Font->Characters[c].Bearing = Vector2_Create(face->glyph->bitmap_left,
				face->glyph->bitmap_top);
		Font->Characters[c].Advance = face->glyph->advance.x;

		HighestCharacter =
				Font->Characters[c].Bearing.y > HighestCharacter ?
						Font->Characters[c].Bearing.y : HighestCharacter;
		LowestCharacter =
				Font->Characters[c].Bearing.y - Font->Characters[c].Size.y
						< LowestCharacter ?
						Font->Characters[c].Bearing.y
								- Font->Characters[c].Size.y :
						LowestCharacter;
	}

	Font_AddTextureAtlas(Font, face, 512, 512);

	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	Font->Characters['\t'].Advance = Font->Characters[' '].Advance * 2;

	Font->ID = Manager->FontNum++;
	Font->TextHeight = HighestCharacter - LowestCharacter;
	Font->UnderTheLigne = -LowestCharacter;

	vector_push_back(Manager->Fonts, Font);

	free(Font);

	return Manager->Fonts->size - 1;
}

int Font_Find(struct FontManager *Manager, GLint ID)
{
	if (!Manager || !Manager->Fonts)
		return -1;

	struct Font *Fonts = (struct Font *) Manager->Fonts->items;
	for (int i = 0; i < Manager->Fonts->size; i++)
	{
		if (Fonts[i].ID == ID)
			return i;
	}
	return -1;
}

static inline void Font_LoadAppropriate(struct FontManager *Manager, float h)
{
	if(h <= 20)
		Font_Use(Manager, ((struct Font *)Manager->Fonts->items)[1].ID);
	else
		Font_Use(Manager, ((struct Font *)Manager->Fonts->items)[0].ID);
}

float Font_Length(struct FontManager *Manager, const char *Text, GLfloat scale)
{
	GLfloat x = 0;
	// Iterate through all characters
	while (*Text)
	{
		struct Font_Character ch = Manager->CurrentFont->Characters[(int) *Text];

		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
		Text++;
	}
	return x;
}

float Font_HeightLength(struct FontManager *Manager, char *Text,
		GLfloat DesiredHeight, GLfloat scale)
{
	GLfloat x = 0;
	// Iterate through all characters
	Font_LoadAppropriate(Manager, DesiredHeight);

	float Height = (float)DesiredHeight / (float) Manager->CurrentFont->TextHeight;
	while (*Text)
	{
		struct Font_Character ch = Manager->CurrentFont->Characters[(int) *Text];

		x += (ch.Advance >> 6) * scale * Height; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
		Text++;
	}

	return x;
}

float Font_HeightLength_SLength(struct FontManager *Manager, int Length,
		GLfloat DesiredHeight, GLfloat scale)
{
	GLfloat x = 0;
	// Iterate through all characters
	Font_LoadAppropriate(Manager, DesiredHeight);

	float Height = DesiredHeight / (float) Manager->CurrentFont->TextHeight;
	for(int i = 0; i < Length; i++)
	{
		struct Font_Character ch = Manager->CurrentFont->Characters['a'];

		x += (ch.Advance >> 6) * scale * Height; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}

	return x;
}

void Font_Use(struct FontManager *Manager, GLint ID)
{
	Manager->CurrentFont = (struct Font *) Manager->Fonts->items
			+ Font_Find(Manager, ID);
}

GLfloat Font_HeightRender(struct FontManager *Manager, char *Text, GLfloat x,
		GLfloat y, GLfloat DesiredHeight, GLfloat scale, struct Vector4f Color)
{
	if (!Manager || !Manager->CurrentFont)
		return 0;

	GLfloat HeightRatio = (GLfloat) DesiredHeight
			/ Manager->CurrentFont->TextHeight;
	GLfloat Width = (float) Font_Length(Manager, Text, scale) * HeightRatio;

	Font_FixedRender(Manager, Text, x, y, DesiredHeight, Width, scale, Color);
	return Width;
}

void Font_HeightMaxRender(struct FontManager *Manager, char *Text, GLfloat x,
		GLfloat y, GLfloat DesiredHeight, GLfloat MaxWidth, GLfloat scale,
		struct Vector4f Color)
{
	if (!Manager || !Manager->CurrentFont)
		return;

	GLfloat HeightRatio = (GLfloat) DesiredHeight
			/ Manager->CurrentFont->TextHeight;
	GLfloat Width = (float) Font_Length(Manager, Text, scale) * HeightRatio;
	if (Width > MaxWidth)
		Width = MaxWidth;

	Font_FixedRender(Manager, Text, x, y, DesiredHeight, Width, scale, Color);
}

void Font_FixedRender(struct FontManager *Manager, const char *Text, GLfloat x,
		GLfloat y, GLfloat DesiredHeight, GLfloat DesiredWidth, GLfloat scale,
		struct Vector4f Color)
{
	if (!Manager || !Manager->CurrentFont)
		return;

	Font_LoadAppropriate(Manager, DesiredHeight);

	if(DesiredHeight <= 20)
		Font_Use(Manager, ((struct Font *)Manager->Fonts->items)[1].ID);
	else
		Font_Use(Manager, ((struct Font *)Manager->Fonts->items)[0].ID);

	GLfloat HeightRatio = (GLfloat) DesiredHeight
			/ Manager->CurrentFont->TextHeight;

	y += Manager->CurrentFont->UnderTheLigne * HeightRatio;

	GLfloat WidthRatio;
	int TextWidth = Font_Length(Manager, Text, scale);
	WidthRatio = (GLfloat) DesiredWidth / TextWidth;

	// Iterate through all characters
	while (*Text && *Text != '\n')
	{
		if (*Text != ' ' && *Text != '\t')
		{
			struct Font_Character ch =
					Manager->CurrentFont->Characters[(int) *Text];

			GLfloat xpos = x + ch.Bearing.x * WidthRatio * scale;
			GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * HeightRatio * scale;

			GLfloat w = ch.Size.x * WidthRatio * scale;
			GLfloat h = ch.Size.y * HeightRatio * scale;
			// Update VBO for each character
			/*GLfloat vertices[6][4] = { { xpos, ypos + h, ch.TexturePos.x,
			 ch.TexturePos.y }, { xpos, ypos, ch.TexturePos.x,
			 ch.TexturePos.w }, { xpos + w, ypos, ch.TexturePos.z,
			 ch.TexturePos.w }, { xpos, ypos + h, ch.TexturePos.x,
			 ch.TexturePos.y }, { xpos + w, ypos, ch.TexturePos.z,
			 ch.TexturePos.w }, { xpos + w, ypos + h, ch.TexturePos.z,
			 ch.TexturePos.y } };*/
			Font_Shader.pushQuad(Manager->CurrentFont->Texture,
					Quad_Create(xpos, ypos, xpos, ypos + h, xpos + w, ypos + h,
							xpos + w, ypos),
					Quad_Create(ch.TexturePos.x, ch.TexturePos.w,
							ch.TexturePos.x, ch.TexturePos.y, ch.TexturePos.z,
							ch.TexturePos.y, ch.TexturePos.z, ch.TexturePos.w),
					Color);
		}
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (Manager->CurrentFont->Characters[(int) *Text].Advance >> 6)
				* WidthRatio * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
		Text++;
	}

	printOpenGLError();
}

GLint Font_HeightRenderCA(struct FontManager *Manager, char *Text, GLfloat x,
		GLfloat y, GLint DesiredHeight, GLfloat scale, struct Vector4f Color) // Center aligned
{
	if (!Manager || !Manager->CurrentFont)
		return 0;

	GLfloat HeightRatio = (GLfloat) DesiredHeight
			/ Manager->CurrentFont->TextHeight;
	GLint Width = (float) Font_Length(Manager, Text, scale) * HeightRatio;

	x -= Width / 2.0f;

	Font_FixedRender(Manager, Text, x, y, DesiredHeight, Width, scale, Color);
	return Width;
}
void Font_HeightMaxRenderCA(struct FontManager *Manager, char *Text, GLfloat x,
		GLfloat y, GLint DesiredHeight, GLint MaxWidth, GLfloat scale,
		struct Vector4f Color) // Center aligned
{
	if (!Manager || !Manager->CurrentFont)
		return;

	GLfloat HeightRatio = (GLfloat) DesiredHeight
			/ Manager->CurrentFont->TextHeight;
	GLint Width = (float) Font_Length(Manager, Text, scale) * HeightRatio;
	if (Width > MaxWidth)
		Width = MaxWidth;

	x -= Width / 2.0f;

	Font_FixedRender(Manager, Text, x, y, DesiredHeight, Width, scale, Color);
}

void Font_FixedRenderCA(struct FontManager *Manager, char *Text, GLfloat x,
		GLfloat y, GLint DesiredHeight, GLint DesiredWidth, GLfloat scale,
		struct Vector4f Color) // Center aligned
{
	x -= DesiredWidth / 2.0f;

	Font_FixedRender(Manager, Text, x, y, DesiredHeight, DesiredWidth, scale,
			Color);
}

void Font_CharacterHeightRenderCA(struct FontManager *Manager, char Character, GLfloat center_x, GLfloat center_y, GLfloat MaxHeight, GLfloat h,
							   struct Vector4f Color)
{
	Font_LoadAppropriate(Manager, MaxHeight);

	float WidthRatio = h / Manager->CurrentFont->Characters[(int)Character].Size.x;
	float Height = Manager->CurrentFont->Characters[(int)Character].Size.y * WidthRatio;
	if(Height > MaxHeight) Height = MaxHeight;

	if(h <= 20)
		Font_Use(Manager, ((struct Font *)Manager->Fonts->items)[1].ID);
	else
		Font_Use(Manager, ((struct Font *)Manager->Fonts->items)[0].ID);

	center_x -= h / 2.0f;
	center_y -= Height / 2.0f;

	Font_Shader.pushQuad(Manager->CurrentFont->Texture, Quad_Create(center_x, center_y, center_x, center_y + Height, center_x + h, center_y + Height, center_x + h, center_y),
						 Quad_Create(Manager->CurrentFont->Characters[(int)Character].TexturePos.x, Manager->CurrentFont->Characters[(int)Character].TexturePos.w,
									 Manager->CurrentFont->Characters[(int)Character].TexturePos.x, Manager->CurrentFont->Characters[(int)Character].TexturePos.y, Manager->CurrentFont->Characters[(int)Character].TexturePos.z,
									 Manager->CurrentFont->Characters[(int)Character].TexturePos.y, Manager->CurrentFont->Characters[(int)Character].TexturePos.z, Manager->CurrentFont->Characters[(int)Character].TexturePos.w), Color);
}

GLfloat Font_HeightRenderRenderConstraint(struct FontManager *Manager, char *Text,
		GLfloat x, GLfloat y, GLfloat DesiredHeight, GLfloat scale,
		struct Vector4f Color, struct Vector2f Constraint) // Center aligned
{
	if (!Manager || !Manager->CurrentFont)
		return 0;

	GLfloat HeightRatio = (GLfloat) DesiredHeight
			/ Manager->CurrentFont->TextHeight;
	GLfloat Width = (float) Font_Length(Manager, Text, scale) * HeightRatio;

	Font_FixedRenderRenderConstraint(Manager, Text, x, y, DesiredHeight, Width,
			scale, Color, Constraint);
	return Width;
}

void Font_HeightMaxRenderRenderConstraint(struct FontManager *Manager,
		char *Text, GLfloat x, GLfloat y, GLfloat DesiredHeight, GLfloat MaxWidth,
		GLfloat scale, struct Vector4f Color, struct Vector2f Constraint) // Center aligned
{
	if (!Manager || !Manager->CurrentFont)
		return;

	GLfloat HeightRatio = (GLfloat) DesiredHeight
			/ Manager->CurrentFont->TextHeight;
	GLfloat Width = (float) Font_Length(Manager, Text, scale) * HeightRatio;
	if (Width > MaxWidth)
		Width = MaxWidth;

	Font_FixedRenderRenderConstraint(Manager, Text, x, y, DesiredHeight, Width,
			scale, Color, Constraint);
}

void Font_FixedRenderRenderConstraint(struct FontManager *Manager, char *Text,
		GLfloat x, GLfloat y, GLfloat DesiredHeight, GLfloat DesiredWidth,
		GLfloat scale, struct Vector4f Color, struct Vector2f Constraint) // Center aligned
{
	if (!Manager || !Manager->CurrentFont)
		return;

	Font_LoadAppropriate(Manager, DesiredHeight);

	GLfloat HeightRatio = (GLfloat) DesiredHeight
			/ Manager->CurrentFont->TextHeight;

	y += Manager->CurrentFont->UnderTheLigne * HeightRatio;

	GLfloat WidthRatio;
	int TextWidth = Font_Length(Manager, Text, scale);
	WidthRatio = (GLfloat) DesiredWidth / TextWidth;

	// Iterate through all characters
	while (*Text && *Text != '\n')
	{
		if (*Text != ' ' && *Text != '\t')
		{
			struct Font_Character ch =
					Manager->CurrentFont->Characters[(int) *Text];

			GLfloat xpos = x + ch.Bearing.x * WidthRatio * scale;
			GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * HeightRatio * scale;

			GLfloat oxpos = xpos;

			GLfloat w = ch.Size.x * WidthRatio * scale;
			GLfloat h = ch.Size.y * HeightRatio * scale;
			// Update VBO for each character
			/*GLfloat vertices[6][4] = { { xpos, ypos + h, ch.TexturePos.x,
			 ch.TexturePos.y }, { xpos, ypos, ch.TexturePos.x,
			 ch.TexturePos.w }, { xpos + w, ypos, ch.TexturePos.z,
			 ch.TexturePos.w }, { xpos, ypos + h, ch.TexturePos.x,
			 ch.TexturePos.y }, { xpos + w, ypos, ch.TexturePos.z,
			 ch.TexturePos.w }, { xpos + w, ypos + h, ch.TexturePos.z,
			 ch.TexturePos.y } };*/
			struct Vector4f TextPos = ch.TexturePos;
			if (xpos + w < Constraint.x)
				goto BreakOut;
			if (xpos > Constraint.y)
				goto BreakOut;
			if (xpos < Constraint.x)
			{
				w -= (Constraint.x - xpos);
				xpos = Constraint.x;
				goto Constraint_Render;
			}
			if (xpos + w > Constraint.y)
			{
				w = Constraint.y - xpos;
				goto Constraint_Render;
			}

			Font_Shader.pushQuad(Manager->CurrentFont->Texture,
				Quad_Create(xpos, ypos, xpos, ypos + h, xpos + w, ypos + h,
					xpos + w, ypos),
				Quad_Create(ch.TexturePos.x, ch.TexturePos.w,
					ch.TexturePos.x, ch.TexturePos.y, ch.TexturePos.z,
					ch.TexturePos.y, ch.TexturePos.z, ch.TexturePos.w),
						Color);

			goto BreakOut;

			Constraint_Render :;
			GLfloat textX = TextPos.x + (xpos - oxpos) / (GLfloat)Game_Width * (TextPos.z - TextPos.x);
			GLfloat textY = TextPos.y;
			GLfloat textW = TextPos.z - textX;
			GLfloat textH = TextPos.w - TextPos.y;
			Font_Shader.pushQuad(Manager->CurrentFont->Texture,
					Quad_Create(xpos, ypos, xpos, ypos + h, xpos + w, ypos + h,
							xpos + w, ypos),
					Quad_Create(textX , textY + textH, textX, textY,
							textX + textW, textY, textX + textW, textY + textH), Color);
		}
		BreakOut:
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (Manager->CurrentFont->Characters[(int) *Text].Advance >> 6)
				* WidthRatio * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
		Text++;
	}

	printOpenGLError();
}

float Font_HeightCharacterPosition(struct FontManager *Manager,
		char *Text, GLfloat x, GLfloat DesiredHeight, GLint Position)
{
	if (!Manager || !Manager->CurrentFont)
		return -1.0f;

	Font_LoadAppropriate(Manager, DesiredHeight);

	GLfloat HeightRatio = (GLfloat) DesiredHeight
			/ Manager->CurrentFont->TextHeight;
	GLfloat Width = Font_Length(Manager, Text, 1.0f) * HeightRatio;

	return Font_GetCharacterPosition(Manager, Text, x, DesiredHeight, Width,
			Position);
}

float Font_HeightMaxCharacterPosition(struct FontManager *Manager,
		char *Text, GLfloat x, GLfloat DesiredHeight, GLfloat MaxWidth,
		GLint Position)
{
	if (!Manager || !Manager->CurrentFont)
		return -1.0f;

	Font_LoadAppropriate(Manager, DesiredHeight);

	GLfloat HeightRatio = (GLfloat) DesiredHeight
			/ (GLfloat) Manager->CurrentFont->TextHeight;
	GLfloat Width = (float) Font_Length(Manager, Text, 1.0f) * HeightRatio;
	if (Width > MaxWidth)
		Width = MaxWidth;

	return Font_GetCharacterPosition(Manager, Text, x, DesiredHeight, Width,
			Position);
}

float Font_GetCharacterPosition(struct FontManager *Manager,
		char *Text, GLfloat x, GLfloat DesiredHeight,
		GLfloat DesiredWidth, GLint Position)
{
	if (!Manager || !Manager->CurrentFont)
		return -1.0f;

	Font_LoadAppropriate(Manager, DesiredHeight);

	GLfloat WidthRatio;
	int TextWidth = Font_Length(Manager, Text, 1.0f);
	WidthRatio = (GLfloat) DesiredWidth / TextWidth;

	char *OText = Text;

	// Iterate through all characters
	while (*Text && *Text != '\n')
	{
		struct Font_Character ch = Manager->CurrentFont->Characters[(int) *Text];

		if (Text - OText == Position)
			return x;

		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * WidthRatio; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))

		Text++;
	}

	return x;
}

int Font_HeightCharacterAt(struct FontManager *Manager, char *Text, GLfloat x, GLfloat DesiredHeight, GLint PosX)
{
	if (!Manager || !Manager->CurrentFont)
		return 0;

	GLfloat HeightRatio = (GLfloat) DesiredHeight
			/ Manager->CurrentFont->TextHeight;
	GLfloat Width = (float) Font_Length(Manager, Text, 1.0f) * HeightRatio;

	return Font_CharacterAt(Manager, Text, x, DesiredHeight, Width, PosX);
}

int Font_HeightMaxCharacterAt(struct FontManager *Manager, char *Text,
		GLfloat x, GLfloat DesiredHeight, GLfloat MaxWidth, GLint PosX)
{
	if (!Manager || !Manager->CurrentFont)
		return 0;

	GLfloat HeightRatio = (GLfloat) DesiredHeight
			/ Manager->CurrentFont->TextHeight;
	GLfloat Width = (float) Font_Length(Manager, Text, 1.0f) * HeightRatio;
	if (Width > MaxWidth)
		Width = MaxWidth;

	return Font_CharacterAt(Manager, Text, x, DesiredHeight, Width, PosX);
}

int Font_CharacterAt(struct FontManager *Manager, char *Text, GLfloat x, GLfloat DesiredHeight, GLfloat DesiredWidth, GLint PosX)
{
	if (!Manager || !Manager->CurrentFont)
		return 0;

	Font_LoadAppropriate(Manager, DesiredHeight);

	GLfloat WidthRatio;
	int TextWidth = Font_Length(Manager, Text, 1.0f);
	WidthRatio = (GLfloat) DesiredWidth / TextWidth;

	char *OText = Text;

	// Iterate through all characters
	while (*Text)
	{
		struct Font_Character ch = Manager->CurrentFont->Characters[(int) *Text];

		GLfloat xpos = x - ch.Bearing.x * WidthRatio * 1.0f;

		GLfloat w = ch.Size.x * WidthRatio * 1.0f;

		if (PosX <= xpos + w)
			return Text - OText;

		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * WidthRatio * 1.0f; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))

		Text++;
	}
	return Text - OText;
}

void Font_Free(struct FontManager *Manager, GLint ID)
{
	int Num = 0;
	if (!Manager || !Manager->Fonts || (Num = Font_Find(Manager, ID)) < 0)
		return;
	struct Font *Font = Manager->Fonts->items + Num;

	Texture_Free(Font->Texture);

	vector_erase(Manager->Fonts, Num);
}

void Font_End(struct FontManager **Manager)
{
	if (!(*Manager) || !(*Manager)->Fonts)
		return;
	struct Font *Font = (*Manager)->Fonts->items;
	for (int Num = 0; Num < (*Manager)->Fonts->size; Num++)
		Texture_Free(Font[Num].Texture);

	vector_delete((*Manager)->Fonts);

	free((*Manager));

	(*Manager) = NULL;
}

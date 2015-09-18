/*
 * Util.h
 *
 *  Created on: Apr 19, 2015
 *      Author: begah
 */

#ifndef UTIL_UTIL_H_
#define UTIL_UTIL_H_

#include "Platform.h"

/* GLFW keys aren't defined on Android, so i define them here */
#ifdef ANDROID

#define GLFW_KEY_0   48
#define GLFW_KEY_1   49
#define GLFW_KEY_2   50
#define GLFW_KEY_3   51
#define GLFW_KEY_4   52
#define GLFW_KEY_5   53
#define GLFW_KEY_6   54
#define GLFW_KEY_7   55
#define GLFW_KEY_8   56
#define GLFW_KEY_9   57
#define GLFW_KEY_A   65
#define GLFW_KEY_APOSTROPHE   39 /* ' */
#define GLFW_KEY_B   66
#define GLFW_KEY_BACKSLASH   92 /* \ */
#define GLFW_KEY_BACKSPACE   259
#define GLFW_KEY_C   67
#define GLFW_KEY_CAPS_LOCK   280
#define GLFW_KEY_COMMA   44 /* , */
#define GLFW_KEY_D   68
#define GLFW_KEY_DELETE   261
#define GLFW_KEY_DOWN   264
#define GLFW_KEY_E   69
#define GLFW_KEY_END   269
#define GLFW_KEY_ENTER   257
#define GLFW_KEY_EQUAL   61 /* = */
#define GLFW_KEY_ESCAPE   256
#define GLFW_KEY_F   70
#define GLFW_KEY_F1   290
#define GLFW_KEY_F10   299
#define GLFW_KEY_F11   300
#define GLFW_KEY_F12   301
#define GLFW_KEY_F13   302
#define GLFW_KEY_F14   303
#define GLFW_KEY_F15   304
#define GLFW_KEY_F16   305
#define GLFW_KEY_F17   306
#define GLFW_KEY_F18   307
#define GLFW_KEY_F19   308
#define GLFW_KEY_F2   291
#define GLFW_KEY_F20   309
#define GLFW_KEY_F21   310
#define GLFW_KEY_F22   311
#define GLFW_KEY_F23   312
#define GLFW_KEY_F24   313
#define GLFW_KEY_F25   314
#define GLFW_KEY_F3   292
#define GLFW_KEY_F4   293
#define GLFW_KEY_F5   294
#define GLFW_KEY_F6   295
#define GLFW_KEY_F7   296
#define GLFW_KEY_F8   297
#define GLFW_KEY_F9   298
#define GLFW_KEY_G   71
#define GLFW_KEY_GRAVE_ACCENT   96 /* ` */
#define GLFW_KEY_H   72
#define GLFW_KEY_HOME   268
#define GLFW_KEY_I   73
#define GLFW_KEY_INSERT   260
#define GLFW_KEY_J   74
#define GLFW_KEY_K   75
#define GLFW_KEY_KP_0   320
#define GLFW_KEY_KP_1   321
#define GLFW_KEY_KP_2   322
#define GLFW_KEY_KP_3   323
#define GLFW_KEY_KP_4   324
#define GLFW_KEY_KP_5   325
#define GLFW_KEY_KP_6   326
#define GLFW_KEY_KP_7   327
#define GLFW_KEY_KP_8   328
#define GLFW_KEY_KP_9   329
#define GLFW_KEY_KP_ADD   334
#define GLFW_KEY_KP_DECIMAL   330
#define GLFW_KEY_KP_DIVIDE   331
#define GLFW_KEY_KP_ENTER   335
#define GLFW_KEY_KP_EQUAL   336
#define GLFW_KEY_KP_MULTIPLY   332
#define GLFW_KEY_KP_SUBTRACT   333
#define GLFW_KEY_L   76
#define GLFW_KEY_LAST   GLFW_KEY_MENU
#define GLFW_KEY_LEFT   263
#define GLFW_KEY_LEFT_ALT   342
#define GLFW_KEY_LEFT_BRACKET   91 /* [ */
#define GLFW_KEY_LEFT_CONTROL   341
#define GLFW_KEY_LEFT_SHIFT   340
#define GLFW_KEY_LEFT_SUPER   343
#define GLFW_KEY_M   77
#define GLFW_KEY_MENU   348
#define GLFW_KEY_MINUS   45 /* - */
#define GLFW_KEY_N   78
#define GLFW_KEY_NUM_LOCK   282
#define GLFW_KEY_O   79
#define GLFW_KEY_P   80
#define GLFW_KEY_PAGE_DOWN   267
#define GLFW_KEY_PAGE_UP   266
#define GLFW_KEY_PAUSE   284
#define GLFW_KEY_PERIOD   46 /* . */
#define GLFW_KEY_PRINT_SCREEN   283
#define GLFW_KEY_Q   81
#define GLFW_KEY_R   82
#define GLFW_KEY_RIGHT   262
#define GLFW_KEY_RIGHT_ALT   346
#define GLFW_KEY_RIGHT_BRACKET   93 /* ] */
#define GLFW_KEY_RIGHT_CONTROL   345
#define GLFW_KEY_RIGHT_SHIFT   344
#define GLFW_KEY_RIGHT_SUPER   347
#define GLFW_KEY_S   83
#define GLFW_KEY_SCROLL_LOCK   281
#define GLFW_KEY_SEMICOLON   59 /* ; */
#define GLFW_KEY_SLASH   47 /* / */
#define GLFW_KEY_SPACE   32
#define GLFW_KEY_T   84
#define GLFW_KEY_TAB   258
#define GLFW_KEY_U   85
#define GLFW_KEY_UNKNOWN   -1
#define GLFW_KEY_UP   265
#define GLFW_KEY_V   86
#define GLFW_KEY_W   87
#define GLFW_KEY_WORLD_1   161 /* non-US #1 */
#define GLFW_KEY_WORLD_2   162 /* non-US #2 */
#define GLFW_KEY_X   88
#define GLFW_KEY_Y   89
#define GLFW_KEY_Z   90

#endif

#ifndef M_PI
#define M_PI 3.14159265359
#endif

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

/* Window structure holding GLFW window on Desktop and the VAO
 * On Android it isn't really necessary but added for compatibility */
#ifndef ANDROID
struct Window
{
	char *WINDOW_TITLE;
	int WINDOW_WIDTH, WINDOW_HEIGHT;
	GLFWwindow *Window;
	_Bool Running;

	GLuint DefaultVAO;
};
#else
struct Window
{
	char *WINDOW_TITLE;
	int WINDOW_WIDTH, WINDOW_HEIGHT;
	_Bool Running;
};

/* Redefining printf to be sent to android log on android */
#undef printf
#define printf(...) __android_log_print(ANDROID_LOG_ERROR, "Native", __VA_ARGS__)
#endif

#define String_length(string) strlen(string)

/* Simple structure to hold all input's potentially information.
 * Only key and mods are used */
struct InputKey
{
	int key, scancode, mods;
};

/* Unused structure, there for potentially future 3D support */
struct Matrix4f
{
	float m[4][4];
};

struct Vector2f
{
	float x, y;
};

struct Vector3f
{
	float x, y, z;
};

struct Vector4f
{
	float x, y, z, w;
};

struct Vector5f
{
	float x, y, z, w, h;
};

struct Vector6f
{
	float x, y, z, w, h, o;
};

struct Vertex
{
	struct Vector3f m_pos;
	struct Vector2f m_tex;
};

/* Quad structure is used to define rectangle and other shapes to add flexibility to the rendering engine */
struct Quad
{
	struct Vector2f v1, v2, v3, v4;
};

/* Packaging structure for the Packaging algorithm defined in Util_Atlas.c */
struct PackAtlas
{
	int width, height;
	int startX, startY;
	_Bool *Atlas;
};

struct Image
{
	GLuint Image;
	float x, y, x2, y2;
	int pX, pY, pW, pH;
};

/* Simple structure to hold a string, used with vector_t */
struct String_Struct
{
	char *String;
};

/* Font structures used to draw text */
struct Font_Character
{
	struct Vector4f TexturePos;
	struct Vector2f Size;    // Size of glyph
	struct Vector2f Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};

struct Font
{
	struct Font_Character Characters[128];

	GLuint Texture;

	GLuint TextHeight, UnderTheLigne;

	GLint ID;
};

struct FontManager
{
	struct vector_t *Fonts;

	struct Font *CurrentFont;

	GLint FontNum;
};

/* Structure containing raw color data of an image */
struct Image_RawData
{
	GLuint Width, Height;
	unsigned char *Data;
};

/* GUI features */
struct Gui_TextBox
{
	float X, Y, Width, Height;
	float TextOffSetX, TextOffSetY, TextMaxWidth, TextHeight;
	_Bool NotCentered;
	GLint Original_Width, Original_Height;
	struct Vector4f Color, HoverColor, Font_Color;
	char *Text, *Value; // Text is for use only by the textbox, Value is the text entered by the user ( it is overlapping with Text but only contains the text entered )

	int ValueLength;
	_Bool Hover, Writing;

	_Bool AcceptSymbols, AcceptNumbers, AcceptLetters, AcceptDot; // Set to false by defaults, determines what can or cannot be writting to the textbox

	struct Quad Quad_takeMouse; // Quad where Button need to take into account mouse inputs
};

struct Gui_Button
{
	float X, Y, Width, Height;
	float TextOffSetX, TextOffSetY, TextWidth, TextHeight;
	GLint Original_Width, Original_Height;

	struct Vector4f Color, HoverColor;
	struct Vector4f Font_Color;
	char *Text;
	_Bool Hover, CenterText;
};

/* Tabs are a vertical list of elements that can be clicked on */
struct Gui_Tab
{
	float X, Y, Width, Height, ItemHeight, ItemsInView;
	struct FontManager *FM;
	GLint FontID;

	GLint ScrollValue;

	struct Vector4f ItemColor, ItemColorHover;

	struct Vector4f Font_Color;
	char *Name;
	_Bool Hover, CopyName;
	struct Gui_Button *Create;

	void *DataToPass;

	int Item_Selected;

	struct vector_t *Items;

	struct Quad Tab;
	struct Vector4f Tab_Color;

	void (*onItemClick)(void *DataToPass, int ID);
	void (*onItemCreate)(void *DataToPass);
};

struct Gui_Horizontal_ScrollBar
{
	struct Quad ScrollBar;
	float BarX;

	float BarValue, TotalValue;

	_Bool IsPressed;

	struct Vector4f Color;
	struct Vector4f Bar_Color, Bar_Color_Hover;

	void *Data;

	void (*onBarScrolling)(void *DataToPass);
};

struct Gui_Vertical_ScrollBar
{
	struct Quad ScrollBar;
	float BarY;

	_Bool Inversed;

	float BarValue, TotalValue;

	_Bool IsPressed;

	struct Vector4f Color;
	struct Vector4f Bar_Color, Bar_Color_Hover;

	void *Data;

	void (*onBarScrolling)(void *DataToPass, float Y);
};

/* Cross platform file engine */
struct F_FileInternal
{
#ifndef ANDROID
	FILE *File;
#elif defined(ANDROID)
	JNIEnv *env;
	AAssetManager* mgr;
	AAsset* asset;
	int isAttached;
#else

#endif
};

struct F_FileExternal
{
	FILE *File;
};

/* Simple colors define in RGBA format */
struct Vector4f COLOR_RED;
struct Vector4f COLOR_GREEN;
struct Vector4f COLOR_BLUE;
struct Vector4f COLOR_BLACK;
struct Vector4f COLOR_WHITE;

/* Cross platform renderer with platform dependent implementations
 * Defined in Util_DesktopRenderer.c and Util_AndroidRenderer.c */
void Renderer_SetUp();
void Renderer_Flush();
void Renderer_CleanUp();

/* platform dependent implementation of Util and Windowing systems */
#ifndef ANDROID
void Util_Init(struct Window *Window,
		void (*error_cb)(int error, const char* description),
		void (*keyboard_cb)(GLFWwindow *window, int key, int scancode,
				int action, int mods),
		void (*character_cb)(GLFWwindow* window, unsigned int codepoint),
		void (*mouse_pos_cb)(GLFWwindow *window, double x, double y),
		void (*mouse_button_cb)(GLFWwindow *Window, int button, int action,
				int mods),
		void (*mouse_scroll_cb)(GLFWwindow *Window, double x, double y));
void Util_Free(struct Window **Window);
void Util_Flush(); // Call Renderer_flush() to draw everything at once

void Window_Create(char *Title, int Width, int Height, int MoniterNum,
		struct Window **Window);
void Window_Refresh(struct Window **Window);
void Window_Free(struct Window **Window); // Call Util_Flush and swaps buffers
#else
void Util_Init(struct Window *Window);
void Util_Free(struct Window **Window);
void Util_Flush(); // Call Renderer_flush to draw everything at once
void Window_Create(struct Window **Window, char *Title, int Width, int Height);
#define Window_Refresh(x)
void Window_Free(struct Window **Window); // Call Util_Flush
#endif

/* Shader functions defined in Util_Shader.c, only used by the engine */
GLuint Shader_Create(char *Path);
void Shader_Free(GLuint Shader);
GLuint Shader_loadCustom(char *VS, char *FS);

GLuint Shader_LoadDefault();
GLuint Shader_LoadImage();

void Texture_Free(GLuint Image);

/* platform independent way to load images */
GLuint ImageEngine_GetAtlas();
struct Image *Image_LoadExternal(const char *Path); // Load Images using the Executable_Path
struct Image *Image_Load(const char *Path); // Load Images using Asset_Path
void Image_Free(struct Image *Image);

/* Use if engine runs into an error, like unable to link shader */
void Application__Error(const char *FunctionName);

/* Macro to get the caller function's name */
#define Application_Error() Application__Error(__FUNCTION__)

/* Packaging algorithm functions used in Util_Font.c to pack characters glyphs */
struct PackAtlas* PackAtlas_Init(int w, int h);
void PackAtlas_Free(struct PackAtlas* atlas);
int PackAtlas_Add(struct PackAtlas* atlas, int rw, int rh, int* rx, int* ry);
void PackAtlas_Remove(struct PackAtlas *PA, int x, int y, int w, int h);

/* Font engine specific functions */
struct FontManager *Font_Init();
GLint Font_Add(struct FontManager *Manager, char *Path, unsigned int Size);
void Font_Use(struct FontManager *Manager, GLint ID);
void Font_Free(struct FontManager *Manager, GLint ID);
void Font_End(struct FontManager **Manager);

/* Font rendering functions */
GLfloat Font_HeightRender(struct FontManager *Manager, char *Text, GLfloat x,
						GLfloat y, GLfloat DesiredHeight, GLfloat scale, struct Vector4f Color);
void Font_HeightMaxRender(struct FontManager *Manager, char *Text, GLfloat x,
						  GLfloat y, GLfloat DesiredHeight, GLfloat MaxWidth, GLfloat scale,
						  struct Vector4f Color);
void Font_FixedRender(struct FontManager *Manager, const char *Text, GLfloat x,
					  GLfloat y, GLfloat DesiredHeight, GLfloat DesiredWidth, GLfloat scale,
					  struct Vector4f Color);
GLint Font_HeightRenderCA(struct FontManager *Manager, char *Text, GLfloat x,
						  GLfloat y, GLint DesiredHeight, GLfloat scale, struct Vector4f Color); // Center aligned
void Font_HeightMaxRenderCA(struct FontManager *Manager, char *Text, GLfloat x,
							GLfloat y, GLint DesiredHeight, GLint MaxWidth, GLfloat scale,
							struct Vector4f Color); // Center aligned
void Font_FixedRenderCA(struct FontManager *Manager, char *Text, GLfloat x,
						GLfloat y, GLint DesiredHeight, GLint DesiredWidth, GLfloat scale,
						struct Vector4f Color); // Center aligned
void Font_CharacterHeightRenderCA(struct FontManager *Manager, char Character, GLfloat center_x, GLfloat center_y, GLfloat MaxHeight, GLfloat h,
								  struct Vector4f Color); // Precise alignment for a single character
GLfloat Font_HeightRenderRenderConstraint(struct FontManager *Manager, char *Text, GLfloat x,
										GLfloat y, GLfloat DesiredHeight, GLfloat scale, struct Vector4f Color, struct Vector2f Constraint); // Center aligned
void Font_HeightMaxRenderRenderConstraint(struct FontManager *Manager, char *Text, GLfloat x,
										  GLfloat y, GLfloat DesiredHeight, GLfloat MaxWidth, GLfloat scale,
										  struct Vector4f Color, struct Vector2f Constraint); // Center aligned
void Font_FixedRenderRenderConstraint(struct FontManager *Manager, char *Text, GLfloat x,
									  GLfloat y, GLfloat DesiredHeight, GLfloat DesiredWidth, GLfloat scale,
									  struct Vector4f Color, struct Vector2f Constraint); // Center aligned
float Font_HeightCharacterPosition(struct FontManager *Manager,
											 char *Text, GLfloat x, GLfloat DesiredHeight, GLint Position);
float Font_HeightMaxCharacterPosition(struct FontManager *Manager,
												char *Text, GLfloat x, GLfloat DesiredHeight, GLfloat MaxWidth, GLint Position);
float Font_GetCharacterPosition(struct FontManager *Manager,
										  char *Text, GLfloat x, GLfloat DesiredHeight, GLfloat DesiredWidth, GLint Position);
int Font_HeightCharacterAt(struct FontManager *Manager, char *Text, GLfloat x, GLfloat DesiredHeight, GLint PosX);
int Font_HeightMaxCharacterAt(struct FontManager *Manager, char *Text,
							  GLfloat x, GLfloat DesiredHeight, GLfloat MaxWidth, GLint PosX);
int Font_CharacterAt(struct FontManager *Manager, char *Text, GLfloat x, GLfloat DesiredHeight, GLfloat DesiredWidth, GLint PosX);
float Font_Length(struct FontManager *Manager, const char *Text, GLfloat scale);
float Font_HeightLength(struct FontManager *Manager, char *Text,
					  float DesiredHeight, GLfloat scale);
float Font_HeightLength_SLength(struct FontManager *Manager, int Length,
		GLfloat DesiredHeight, GLfloat scale);

/* Gui's functions */
struct Gui_TextBox *Gui_TextBox_Create(float X, float Y, float Width,
									   float Height, const char *Name, GLint MaxTextLength, float TextOffsetX,
									   float TextOffsetY, float TextHeight, float MaxTextWidth, float ColorR, float ColorG,
									   float ColorB, float ColorA, float HoverColorR, float HoverColorG,
									   float HoverColorB, float HoverColorA);
void Gui_TextBox_Render(struct Gui_TextBox *TB);
void Gui_TextBox_ClearText(struct Gui_TextBox *TB);
void Gui_TextBox_ResizeText(struct Gui_TextBox *TB, float TextOffsetX,
							float TextOffsetY, float TextWidth, float TextHeight);
void Gui_TextBox_Resize(struct Gui_TextBox *TB, float X, float Y, float Width,
						float Height);
void Gui_TextBox_Free(struct Gui_TextBox **TB); // Free and sets the pointer to NULL
void Gui_TextBox_Free_Simple(struct Gui_TextBox *TB); // Doesn't free the structure

struct Gui_Button *Gui_Button_Create(float X, float Y, float Width,
									 float Height, const char *Name, float TextOffsetX, float TextOffsetY,
									 float TextWidth, float TextHeight, float ColorR, float ColorG, float ColorB, float ColorA,
									 float HoverColorR, float HoverColorG, float HoverColorB,
									 float HoverColorA);
_Bool Gui_Button_Render(struct Gui_Button *TB);
void Gui_Button_ResizeText(struct Gui_Button *TB, float TextOffsetX,
						   float TextOffsetY, float TextWidth, float TextHeight);
void Gui_Button_Resize(struct Gui_Button *TB, float X, float Y, float Width,
					   float Height);
void Gui_Button_Free(struct Gui_Button **TB); // Free and sets the pointer to NULL
void Gui_Button_Free_Simple(struct Gui_Button *TB); // Doesn't free the structure

struct Gui_Tab *Gui_Tab_Create(char *Name, float X, float Y, float Width,
							   float Height, float ItemHeight, float ItemsInView,
							   struct FontManager *FM, GLint FontID, void *DataToPass, _Bool CopyName);
void Gui_Tab_Add(struct Gui_Tab *Tab, char *Name);
void Gui_Tab_NewString(struct Gui_Tab *Tab, int ID, char *String);
void Gui_Tab_Render(struct Gui_Tab *Tab);
int Gui_Tab_SimpleRender(struct Gui_Tab *Tab); // Used to only render the list of items and not the create button and the tab's name
void Gui_Tab_Resize(struct Gui_Tab *Tab, float X, float Y, float Width,
					float Height, float ItemHeight);
void Gui_Tab_Free(struct Gui_Tab **TB);

struct Gui_Horizontal_ScrollBar *Gui_Horizontal_ScrollBar_Create(float x,
																 float y, float width, float height, struct Vector4f Color,
																 struct Vector4f Bar_Color, struct Vector4f Bar_Color_Hover,
																 float BarValue, float TotalValue, void (*onBarScrolling)(void *));
void Gui_Horizontal_ScrollBar_Render(struct Gui_Horizontal_ScrollBar *ScrollBar);
void Gui_Horizontal_ScrollBar_Resize(struct Gui_Horizontal_ScrollBar *ScrollBar, float x, float y, float w, float h, float barValue, float totalValue);
void Gui_Horizontal_ScrollBar_Free(struct Gui_Horizontal_ScrollBar **ScrollTab);

struct Gui_Vertical_ScrollBar *Gui_Vertical_ScrollBar_Create(float x,
																 float y, float width, float height, struct Vector4f Color,
																 struct Vector4f Bar_Color, struct Vector4f Bar_Color_Hover,
																 float BarValue, float TotalValue, void (*onBarScrolling)(void *, float));
void Gui_Vertical_ScrollBar_Inverse(struct Gui_Vertical_ScrollBar *ScrollBar);
void Gui_Vertical_ScrollBar_Render(struct Gui_Vertical_ScrollBar *ScrollBar);
void Gui_Vertical_ScrollBar_Resize(struct Gui_Vertical_ScrollBar *ScrollBar, float x, float y, float w, float h, float barValue, float totalValue);
void Gui_Vertical_ScrollBar_Free(struct Gui_Vertical_ScrollBar **ScrollTab);

void String_Add(char *String1, char *String2); // Concatenates two strings together, the first string must have space for the second
void String_Copy(char *String1, const char *String2); // Copy String2 to another String1
void String_Remove(char *String1, char *String2); // Remove String2 from String1

int String_fulllength(char *String); // Full length counts \t as double
int String_numoftab(char *String); // Return number of \t
int String_numoftab_Length(const char *String, int Length); // Return number of \t in a string of length Length
int String_toInt(char *String); // Converts String to int
struct String_Struct *String_Split(char *String, char Spliter, int *NumOfSplit); // Splits a string into String_Struct's
_Bool String_contain(const char *ori, const char *Con); // Verify if ori contains Con at index
int String_replace(char *Ligne, const char *OldName, const char *NewName); // Replace OldName with NewName in string Ligne
_Bool Character_isValid(char C); // Checks if character that my engine can render

/* Simple math helper functions */
void Random_Init();
_Bool Random_NextBool();
short Random_NextInt(int i);
char *Integer_toString(int Number);
int Integer_Reverse(int Number);
int Integer_FirstDigit(int Number);
double View_TranslateTo(double pos, double OldDimension, double NewDimension);

_Bool Square_Collide(int x, int y, int width, int height, int x2, int y2,
					 int width2, int height2);
_Bool Square_PreciseCollide(float x, float y, float x2, float y2, float xx,
							float yy, float xx2, float yy2);
_Bool Point_inSquare(struct Vector2f point, float x, float y, float width,
					 float height);
_Bool Point_inPreciseSquare(struct Vector2f point, float x, float y, float xx,
							float yy);
_Bool Point_inTriangle(struct Vector2f point, struct Vector2f v1,
					   struct Vector2f v2, struct Vector2f v3);
_Bool Point_inQuad(struct Vector2f point, struct Quad quad); // Call's Point_inTriangle two times

struct Vector2f Vector2_Create(float x, float y);
struct Vector3f Vector3_Create(float x, float y, float z);
struct Vector4f Vector4_Create(float x, float y, float z, float w);
struct Vector6f Vector6_Create(struct Vector2f vec2, struct Vector4f vec4);
struct Vector6f Vector6_Createf(float x, float y, float z, float w, float h, float o);
struct Vertex Vertex_Create(struct Vector3f vec3, struct Vector2f vec2);
struct Quad Quad_Create(float x, float y, float x2, float y2, float x3,
						float y3, float x4, float y4);

/* Function used to Calculate FPS on Windows, and to calculate mouse event's timing */
long int Time_elapsed(struct timeval Start, struct timeval End);

void Dir_Create(char *name);
_Bool Dir_Exists(char *name);

int ImageEngine_Width();
int ImageEngine_Height();

/* Platform dependent implementation to access the platform's clipboard */
void Clipboard_setWindow(struct Window *Windo);
char *Clipboard_get();
void Clipboard_set(const char *String);

/* OnScreen keyboard functions */
void Engine_requestKeyboard(float X, float Y, float Width, float Height);
void Engine_renderKeyboard();
void Engine_requestCloseKeyboard();
void Engine_closeKeyboard();

/* Platform independent file layer */
struct F_FileInternal *FileInternal_Open(const char *FilePath);
long int FileInternal_Length(struct F_FileInternal *file);
void FileInternal_Seek(struct F_FileInternal *file, int offset, int whence);
int FileInternal_Read(void *ptr, size_t Unit_size, int Unit_number, struct F_FileInternal *file);
void FileInternal_Close(struct F_FileInternal *file);

struct F_FileExternal *FileExternal_Open(const char *FilePath);
struct F_FileExternal *FileExternal_OpenWrite(const char *FilePath, _Bool NeedRead, _Bool DontOverride);
_Bool FileExternal_Exists(const char *FilePath);
long int FileExternal_Length(struct F_FileExternal *file);
FILE *FileExternal_getCFile(struct F_FileExternal *file);
char *FileExternal_GetFullPath(const char *FileName);
struct F_FileExternal *FileExternal_fromFile(FILE *file);
void FileExternal_Seek(struct F_FileExternal *file, int offset, int whence);
int FileExternal_Tell(struct F_FileExternal *file);
int FileExternal_Read(void *ptr, size_t Unit_size, int Unit_number, struct F_FileExternal *file);
void FileExternal_Write(void *ptr, size_t Unit_size, int Unit_number, struct F_FileExternal *file);
void FileExternal_Close(struct F_FileExternal *file);

#include "../SharedInterface.h"

// Needed for SOIL2 to be compatible with OpenGL ES
#define SOIL_GLES2
#define SOIL_NO_EGL

#include "Image/SOIL2.h"
#include "Vector/vector.h"
#include "Debug/debug.h"
#include "FreeType/FreeTypeAmalgam.h"
#include "tinycthread/tinycthread.h"

#endif /* UTIL_UTIL_H_ */

/*
 * SharedInterface.h
 *
 *  Created on: Apr 19, 2015
 *      Author: begah
 */

#ifndef SHAREDINTERFACE_H_
#define SHAREDINTERFACE_H_

#include "Util/Platform.h"

#include "Util/Util.h"

/* Executable_Path is originally the location where resides Assets folder on Desktop or in the "Lua Editor" folder in Documents on phones
 * When a project is loaded/created Executable_Path changes to be inside the project's folder.
 * Asset_Path is the path to Assets folder so we can have access to it even when Executable_Path is changed
 */
char *Executable_Path, *Asset_Path;

/* Game_Width and Game_Height are the screen's and OpenGL context size.
 * ( When windowed is minimized, width and height becomes 0 but are not registered in the global variables ) */
int Game_Width, Game_Height;

/* Global mouse structure including all different mouse actions needed by the application.
 * It is available and accessible everywhere in the application.
 */
struct Mouse
{
	_Bool Faked_Input;
	double x, y;
	double prevX, prevY;
	_Bool justPressed, justReleased, justLongPressedReleased, justScrolled, justSuper_longReleased, just_DoubleClicked, isPressed, isLeftPressed, isLongedPressed, isSuper_longPress, justQuickPressed;
	int pressedCooldown;
	double scrollX, scrollY;
} Mouse;

/* Global keyboard structure including all different keyboard actions needed by the application.
 * It is available and accessible everywhere in the application.
 */
struct Keyboard
{
	_Bool justPressed, justReleased, justTyped;
	struct InputKey Key;
	unsigned int Character;
} Keyboard;

/* OnScreen Keyboard global pointer, to test if it's NULL or not */
struct OnScreen_Keyboard_Struct *OnScreen_Keyboard;

/* The different shaders structure only contains pointers to function defined in Util_AndroidRenderer.c or Util_DesktopRenderer.c
 * depending if you use OpenGL ES or OpenGL.
 * The last int in the functions is the Z value.
 */
struct Default_Shader_Functions
{
	void (*pushVertices)(float, float, float, float, float, float, int);
	void (*pushQuad)(struct Quad, struct Vector4f, int);
} Default_Shader;

struct Image_Shader_Functions
{
	void (*pushVertices)(float, float, float, float, GLuint, struct Vector4f, int);
	void (*pushQuad)(struct Quad, struct Quad, GLuint, struct Vector4f, int);
} Image_Shader;

struct Font_Shader_Functions
{
	void (*pushVertices)(GLuint, float, float, float, float, struct Vector4f, int);
	void (*pushQuad)(GLuint, struct Quad, struct Quad, struct Vector4f, int);
} Font_Shader;

/* Global font manager that holds references to every font opened by the engine,
 * By default only two fonts of different sizes are loaded.
 */
struct FontManager *DefaultFontManager;

/* ID of fonts loaded, dynamically changed if text is big or small */
GLint HighDefaultFont, LowDefaultFont;

/* Game FPS, calculated in Lua Editor.c for desktop or in Native.java for Android */
int Game_FPS;
long int Frame_Time_Passed;

/* If set to false stops the main loop */
_Bool GameRunning;

/* JNI variables needed to communicate with Java */
#ifdef ANDROID
JavaVM *Java_VM;
jobject Java_AssetManager;
#endif

#endif /* SharedInterface.h */

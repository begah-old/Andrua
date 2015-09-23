/*
 * Util.c
 *
 *  Created on: Apr 19, 2015
 *      Author: begah
 */

#include "Util.h"

#define EXECUTABLE_NAME 6
#define DEBUG
#define CODE_BLOCKS

static void ImageEngine_SetUp();

struct timeval Frame_TimePassed = {-1, -1};

#ifndef ANDROID

#ifndef _WIN32
/* On Linux i get a warning without declaring the function */
extern ssize_t readlink (const char *__restrict __path,
			 char *__restrict __buf, size_t __len)
     __THROW __nonnull ((1, 2)) __wur;
#endif

void Util_Init(struct Window *Window,
		void (*error_cb)(int error, const char* description),
		void (*keyboard_cb)(GLFWwindow *window, int key, int scancode,
				int action, int mods),
		void (*character_cb)(GLFWwindow* window, unsigned int codepoint),
		void (*mouse_pos_cb)(GLFWwindow *window, double x, double y),
		void (*mouse_button_cb)(GLFWwindow *Window, int button, int action,
				int mods),
		void (*mouse_scroll_cb)(GLFWwindow *Window, double x, double y))
{
	log_info("Initializing Util library");
	Mouse.x = 0;
	Mouse.y = 0;
	Mouse.justReleased = Mouse.isPressed = Mouse.justPressed =
			Mouse.isLongedPressed = false;
	Mouse.isSuper_longPress = Mouse.justSuper_longReleased = false;
	Mouse.justQuickPressed = false;
	Mouse.pressedCooldown = 0;
	Mouse.Faked_Input = false;
	Game_FPS = 0;
	Frame_Time_Passed = 0;

	Keyboard.justPressed = Keyboard.justReleased = Keyboard.justTyped = false;

	glfwSetErrorCallback(error_cb);
	glfwSetKeyCallback(Window->Window, keyboard_cb);
	glfwSetCharCallback(Window->Window, character_cb);
	glfwSetCursorPosCallback(Window->Window, mouse_pos_cb);
	glfwSetMouseButtonCallback(Window->Window, mouse_button_cb);
	glfwSetScrollCallback(Window->Window, mouse_scroll_cb);

	log_info("Done setting up GLFW");

#ifdef _WIN32
	char Path[101];
	int PathLength = 100;
	GetModuleFileName(NULL, &Path[0], PathLength);
	Path[100] = '\0';

	int Length = String_length(&Path[0]) - EXECUTABLE_NAME - 4;

#ifndef DEBUG
	Executable_Path = malloc(sizeof(char) * (Length + 1));
	for(int Index = 0; Index < Length; Index++)
	Executable_Path[Index] = Path[Index];
	Executable_Path[Length] = '\0';
#elseif !defined(CODE_BLOCKS)
	Length -= 6;
	Executable_Path = malloc(sizeof(char) * (Length + 1));
	for (int Index = 0; Index < Length; Index++)
	Executable_Path[Index] = Path[Index];
	Executable_Path[Length] = '\0';
#else
	Length -= 10;
	Executable_Path = malloc(sizeof(char) * (Length + 1));
	for (int Index = 0; Index < Length; Index++)
	Executable_Path[Index] = Path[Index];
	Executable_Path[Length] = '\0';
    printf("%s\n", Executable_Path);
#endif
#else
	char Path[101] =
	{ ' ' };
	int PathLength = 100;
	readlink("/proc/self/exe", &Path[0], PathLength * sizeof(char));
	Path[PathLength] = '\0';
	int Length = String_length(&Path[0]) - EXECUTABLE_NAME;
#ifndef DEBUG
	Executable_Path = malloc(sizeof(char) * (Length + 1));
	for(int Index = 0; Index < Length; Index++)
	Executable_Path[Index] = Path[Index];
	Executable_Path[Length] = '\0';
#else
	Length -= 6;
	Executable_Path = malloc(sizeof(char) * (Length + 1));
	for (int Index = 0; Index < Length; Index++)
		Executable_Path[Index] = Path[Index];
	Executable_Path[Length] = '\0';
#endif
#endif

	Asset_Path = malloc(sizeof(char) * (String_length(Executable_Path) + String_length("Assets/") + 1));
	Asset_Path = memcpy(Asset_Path, Executable_Path, sizeof(char) * (String_length(Executable_Path)));
	memcpy(Asset_Path + String_length(Executable_Path), "Assets/", sizeof(char) * (String_length("Assets/") + 1));

	log_info("Done initializing path");

	DefaultFontManager = NULL;
	DefaultFontManager = Font_Init();
	check_mem(DefaultFontManager);

	HighDefaultFont = Font_Add(DefaultFontManager,
			"Font/Cousine-Regular.ttf", 50);
	check(HighDefaultFont >= 0, "Couldn't load default font : %s",
			"Font/Cousine-Regular.ttf");

	log_info("Done loading high definition font");

	LowDefaultFont = Font_Add(DefaultFontManager,
			"Font/Cousine-Regular.ttf", 25);
	check(LowDefaultFont >= 0, "Couldn't load default font : %s",
			"Font/Cousine-Regular.ttf");

	log_info("Done loading low definition font");

	Font_Use(DefaultFontManager, HighDefaultFont);

	ImageEngine_SetUp();
	Renderer_SetUp();

	gettimeofday(&Frame_TimePassed, NULL);

	log_info("Done initializing util library");

	return;
	error: Application_Error(); return;
}
#else
void Util_Init(struct Window *Window)
{
	Mouse.x = 0;
	Mouse.y = 0;
	Mouse.justReleased = Mouse.isPressed = Mouse.justPressed = Mouse.isLongedPressed = false;
	Mouse.isSuper_longPress = Mouse.justSuper_longReleased = false;
	Mouse.pressedCooldown = 0;
	Mouse.Faked_Input = false;
	Game_FPS = 0;
	Frame_Time_Passed = 0;

	Keyboard.justPressed = Keyboard.justReleased = Keyboard.justTyped = false;

	DefaultFontManager = NULL;
	DefaultFontManager = Font_Init();
	check_mem(DefaultFontManager);

	HighDefaultFont = Font_Add(DefaultFontManager,
			"Font/Cousine-Regular.ttf", 50);
	check(HighDefaultFont >= 0, "Couldn't load default font : %s",
			"Font/Cousine-Regular.ttf");

	log_info("Done loading high definition font");

	LowDefaultFont = Font_Add(DefaultFontManager,
			"Font/Cousine-Regular.ttf", 25);
	check(LowDefaultFont >= 0, "Couldn't load default font : %s",
			"Font/Cousine-Regular.ttf");

	log_info("Done loading low definition font");

	Font_Use(DefaultFontManager, HighDefaultFont);

	ImageEngine_SetUp();
	Renderer_SetUp();

	log_info("Done initializing util library");

	return;
	error: return;
}
#endif

void Util_Flush()
{
    struct timeval Current;
    gettimeofday(&Current, NULL);

    if(Current.tv_usec < Frame_TimePassed.tv_usec)
        Frame_Time_Passed = (Current.tv_usec - (Frame_TimePassed.tv_usec - 1000000)) / 1000;
    else
        Frame_Time_Passed = (Current.tv_usec - Frame_TimePassed.tv_usec) / 1000;
    Frame_TimePassed = Current;

	Renderer_Flush();
}

void Util_Free(struct Window **Window)
{
	Renderer_CleanUp();

	Font_End(&DefaultFontManager);
	Window_Free(Window);
}

#ifndef ANDROID
void Window_Create(char *Title, int Width, int Height, int MoniterNum,
		struct Window **Window)
{
	log_info("Creating Window");
	if (*Window == NULL)
		(*Window) = malloc(sizeof(struct Window));

	check_mem((*Window));

	(*Window)->WINDOW_TITLE = Title;
	(*Window)->WINDOW_WIDTH = Width;
	(*Window)->WINDOW_HEIGHT = Height;
	Game_Width = Width;
	Game_Height = Height;

	/* Initialize GLFW for video output */
	if (!glfwInit())
	{
		log_info("Unable to initialize GLFW");
		Application_Error();
	}

	int count;
	GLFWmonitor** monitors = glfwGetMonitors(&count);

	int xpos, ypos;
	glfwGetMonitorPos(monitors[MoniterNum], &xpos, &ypos);
	const GLFWvidmode* mode = glfwGetVideoMode(monitors[0]);

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a 640x480 OpenGL screen */
	(*Window)->Window = glfwCreateWindow((*Window)->WINDOW_WIDTH,
			(*Window)->WINDOW_HEIGHT, (*Window)->WINDOW_TITLE, NULL, NULL);
	glfwSetWindowPos((*Window)->Window,
			xpos + mode->width / 2 - mode->width / 4 + mode->width / 8,
			ypos + mode->height / 2 - mode->height / 4);
	if (!(*Window)->Window)
	{
		log_err("Unable to create OpenGL window");
		Application_Error();
	}

	glfwMakeContextCurrent((*Window)->Window);

	if (gl3wInit())
	{
		log_err("failed to initialize OpenGL");
		Application_Error();
	}
	if (!gl3wIsSupported(3, 3))
	{
		log_err("OpenGL 3.3 not supported");
		Application_Error();
	}

	glGenVertexArrays(1, &(*Window)->DefaultVAO);
	glBindVertexArray((*Window)->DefaultVAO);

	(*Window)->Running = true;

	printOpenGLError();

	log_info("Window has been created");

	return;

	error: Window_Free(Window);
	Application_Error();
	return;
}

void Window_Refresh(struct Window **Window)
{
	check_mem(*Window);

	glfwSwapBuffers((*Window)->Window);

	return;
	error:Application_Error();
}

void Window_Free(struct Window **Window)
{
	check_mem(*Window);

	glfwDestroyWindow((*Window)->Window);
	free(*(Window));
	*(Window) = NULL;

	return;

	error:Application_Error();
}
#else
void Window_Create(struct Window **Window, char *Title, int Width, int Height)
{
	log_info("Creating Window");

	if (*Window == NULL)
		(*Window) = malloc(sizeof(struct Window));

	check_mem((*Window));

	(*Window)->WINDOW_TITLE = Title;
	(*Window)->WINDOW_WIDTH = Width;
	(*Window)->WINDOW_HEIGHT = Height;
	Game_Width = Width;
	Game_Height = Height;

	(*Window)->Running = true;

	printOpenGLError();

	log_info("Window has been created");

	return;

	error: Window_Free(Window);
	Application_Error();
	return;
}

void Window_Free(struct Window **Window)
{
	check_mem(*Window);

	free(*(Window));
	*(Window) = NULL;

	return;
	error:Application_Error();
}
#endif

int ImageAtlas_Width = 4096, ImageAtlas_Height = 4096;
struct PackAtlas *Image_Atlas = NULL;
GLuint Image_OpenglID = 0;

int ImageEngine_Width()
{
	return ImageAtlas_Width;
}

int ImageEngine_Height()
{
	return ImageAtlas_Height;
}

GLuint ImageEngine_GetAtlas()
{
	return Image_OpenglID;
}

static void ImageEngine_SetUp()
{
	Image_Atlas = PackAtlas_Init(ImageAtlas_Width, ImageAtlas_Height);

	glGenTextures(1, &Image_OpenglID);
	glBindTexture(GL_TEXTURE_2D, Image_OpenglID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageAtlas_Width, ImageAtlas_Height, 0, GL_RGBA,
	GL_UNSIGNED_BYTE, NULL);

	// Set texture options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture_Free(GLuint Image)
{
	glDeleteTextures(1, &Image);
}

void Image_Free(struct Image *Image)
{
	PackAtlas_Remove(Image_Atlas, Image->pX, Image->pY, Image->pW + 1, Image->pH + 1);

	free(Image);
}

void Image_FreeSimple(struct Image *Image)
{
	PackAtlas_Remove(Image_Atlas, Image->pX, Image->pY, Image->pW + 1, Image->pH + 1);
}

struct Image *Image_LoadExternal(const char *Path)
{
	struct Image *Image = malloc(sizeof(struct Image));

	struct F_FileExternal *file = FileExternal_Open(Path);

	int Length = FileExternal_Length(file);
	unsigned char *buffer = malloc(Length);
	FileExternal_Read(buffer, sizeof(char), Length, file);
	FileExternal_Close(file);

	int Channels;
	unsigned char *Buffer = SOIL_load_image_from_memory(buffer, Length, &Image->pW, &Image->pH, &Channels, SOIL_LOAD_RGBA);
	if (Buffer == NULL)
	{
		log_err("Error : %s failed to load in function Image_Load", Path);
		Application_Error();
	}

	free(buffer);

	PackAtlas_Add(Image_Atlas, Image->pW + 1, Image->pH + 1, &Image->pX, &Image->pY);

	Image->x = Image->pX / (float)ImageAtlas_Width;
	Image->y = Image->pY / (float)ImageAtlas_Height;
	Image->x2 = Image->x + Image->pW / (float)ImageAtlas_Width;
	Image->y2 = Image->y + Image->pH / (float)ImageAtlas_Height;
	Image->Image = Image_OpenglID;

	glBindTexture(GL_TEXTURE_2D, Image_OpenglID);
	glTexSubImage2D(GL_TEXTURE_2D, 0, Image->pX, Image->pY, Image->pW, Image->pH, GL_RGBA, GL_UNSIGNED_BYTE,
			Buffer);
	free(Buffer);
	return Image;
}

struct Image *Image_Load(const char *Path)
{
	struct Image *Image = malloc(sizeof(struct Image));

	struct F_FileInternal *file = FileInternal_Open(Path);

	if(!file)
    {
        log_info("Couldn't load internal image : %s", Path);
        return NULL;
    }

	int Length = FileInternal_Length(file);
	unsigned char *buffer = malloc(Length);
	FileInternal_Read(buffer, sizeof(char), Length, file);
	FileInternal_Close(file);

	int Channels;
	unsigned char *Buffer = SOIL_load_image_from_memory(buffer, Length, &Image->pW, &Image->pH, &Channels, SOIL_LOAD_RGBA);
	if (Buffer == NULL)
	{
		log_err("Error : %s failed to load in function Image_Load", Path);
		Application_Error();
	}

	free(buffer);

	PackAtlas_Add(Image_Atlas, Image->pW + 1, Image->pH + 1, &Image->pX, &Image->pY);


	Image->x = Image->pX / (float)ImageAtlas_Width;
	Image->y = Image->pY / (float)ImageAtlas_Height;
	Image->x2 = Image->x + Image->pW / (float)ImageAtlas_Width;
	Image->y2 = Image->y + Image->pH / (float)ImageAtlas_Height;
	Image->Image = Image_OpenglID;

	glBindTexture(GL_TEXTURE_2D, Image_OpenglID);
	glTexSubImage2D(GL_TEXTURE_2D, 0, Image->pX, Image->pY, Image->pW, Image->pH, GL_RGBA, GL_UNSIGNED_BYTE,
			Buffer);
	free(Buffer);
	return Image;
}

void Application__Error(const char *FunctionName)
{
	log_info("Application exit on function : %s", FunctionName);
	log_info("Press enter to continue...");
	getchar();
	exit(1);
}

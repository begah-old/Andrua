/*
 ============================================================================
 Name        : Lua.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "SharedInterface.h"
#include "Application/Application.h"

static void error_callback(int error, const char* description);
static void character_callback(GLFWwindow* window, unsigned int codepoint);
static void key_callback(GLFWwindow* window, int key, int scancode, int action,
		int mods);
static void mouse_position_callback(GLFWwindow *Window, double x, double y);
static void mouse_button_callback(GLFWwindow *Window, int button, int action,
		int mods);
static void mouse_scroll_callback(GLFWwindow *Window, double x, double y);

struct timeval TimePressed = { -1, -1 }, Super_long_press = { -1, -1 }, Double_Click_Timer = { -1, -1 };
int Current_FrameCount = 0;
double Current_TimeFrame = 0;

int main(void)
{
	struct Window *Window = NULL;
	Window_Create("Lua Editor", 640, 480, 0, &Window);

	Util_Init(Window, error_callback, key_callback, character_callback,
			mouse_position_callback, mouse_button_callback,
			mouse_scroll_callback);

	Clipboard_setWindow(Window);

	log_info("Initializing application");
	Application_Init();
	log_info("Done initializing application");

	Current_TimeFrame = glfwGetTime();

	log_info("Starting main loop");

	while (!glfwWindowShouldClose(Window->Window))
	{
		/* Calculate FPS */
		if(glfwGetTime() - Current_TimeFrame >= 1.0)
		{
			Game_FPS = Current_FrameCount;
			Current_FrameCount = 0;
			Current_TimeFrame = glfwGetTime();
		}

		int Temp_Width, Temp_Height;
		glfwGetFramebufferSize(Window->Window, &Temp_Width, &Temp_Height);

		if(Temp_Width > 0 && Temp_Height > 0) {
			Game_Width = Temp_Width;
			Game_Height = Temp_Height;
		}

		glViewport(0, 0, Game_Width, Game_Height);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		Engine_renderKeyboard();

		Application_Render();

		Util_Flush();

		Window_Refresh(&Window);
		if (!Window->Running)
			glfwSetWindowShouldClose(Window->Window, !Window->Running);
		Mouse.justPressed = Mouse.justReleased = Mouse.justLongPressedReleased = Mouse.justScrolled =
				Keyboard.justTyped = Keyboard.justPressed = Mouse.justSuper_longReleased = Mouse.just_DoubleClicked =
						Keyboard.justReleased = Mouse.justQuickPressed = Mouse.Faked_Input = false;
		if (Mouse.pressedCooldown > 0)
			Mouse.pressedCooldown--;
		Mouse.prevX = Mouse.x;
		Mouse.prevY = Mouse.y;
		glfwPollEvents();

		{
			/* Timing event's */
			double DistanceX = Mouse.x - Mouse.prevX, DistanceY = Mouse.y - Mouse.prevY;
			DistanceX = DistanceX < 0 ? DistanceX * -1 : DistanceX;
			DistanceY = DistanceY < 0 ? DistanceY * -1 : DistanceY;

			double Distance = sqrt(DistanceX + DistanceY);

			if(TimePressed.tv_sec != -1)
			{
				if(Distance > 1.0)
				{
					TimePressed.tv_sec = -1;
				}
				else
				{
					struct timeval STR;
					gettimeofday(&STR, NULL);
					long int diff = Time_elapsed(TimePressed, STR);
					if(diff >= 1 && Mouse.isPressed)
					{
						TimePressed.tv_sec = -1;
						Mouse.isLongedPressed = true;
						gettimeofday(&Super_long_press, NULL);
						log_info("Just long press");
					}
				}

			}
			/* Super long press logic */
			if(Mouse.isLongedPressed)
			{
				if(Distance > 1.0)
					Super_long_press.tv_sec = -1;
				else if(!Mouse.isSuper_longPress && Super_long_press.tv_sec != -1)
				{
					struct timeval STR;
					gettimeofday(&STR, NULL);
					long int diff = Time_elapsed(Super_long_press, STR);
					if(diff >= 1 && Mouse.isPressed)
					{
						Mouse.isSuper_longPress = true;
						Super_long_press.tv_sec = -1;
						log_info("Just super long pressed");
					}
				}
			}

			if(Double_Click_Timer.tv_sec != -1)
			{
				struct timeval STR;
				gettimeofday(&STR, NULL);
				long int diff = Time_elapsed(Double_Click_Timer, STR);
				if(diff >= 1)
					Double_Click_Timer.tv_sec = -1;
			}
		}
		Current_FrameCount++;
	}

	log_info("Terminating application");

	Application_Free();
	Util_Free(&Window);

	return EXIT_SUCCESS;
}

/* Event's, used to modify the Keyboard and Mouse structure */
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

void character_callback(GLFWwindow* window, unsigned int codepoint)
{
	Keyboard.justTyped = true;
	Keyboard.Character = codepoint;
	log_info("Key : %i", codepoint);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action,
		int mods)
{
	if (action)
		Keyboard.justPressed = true;
	else
		Keyboard.justReleased = true;

	Keyboard.Key.key = key;
	Keyboard.Key.scancode = scancode;
	Keyboard.Key.mods = mods;
}

static void mouse_position_callback(GLFWwindow *Window, double x, double y)
{
	Mouse.x = x;
	Mouse.y = (double) Game_Height - y;
}

static void mouse_button_callback(GLFWwindow *Window, int button, int action,
		int mods)
{

	if (!button)
		Mouse.isLeftPressed = action;

	if (Mouse.pressedCooldown == 0 && action)
	{
		log_info("Just pressed");
		Mouse.justPressed = true;
		Mouse.pressedCooldown = 10;
	}
	if (!action)
	{
		if(Mouse.isLongedPressed) {Mouse.justLongPressedReleased = true;log_info("Just long released");}
		Mouse.justReleased = true;
		log_info("Just released");
	}
	Mouse.isPressed = action;

	if(Mouse.justPressed)
	{
		gettimeofday(&TimePressed, NULL);
	} else
	{
		Mouse.isLongedPressed = false;
		TimePressed.tv_sec = -1;
	}

	if(!action)
	{
		if(Mouse.isSuper_longPress)
		{
			Mouse.justSuper_longReleased = true;
			log_info("Just super long released");
		}
		Mouse.isSuper_longPress = false;
		Super_long_press.tv_sec = -1;

		if(!Mouse.justLongPressedReleased)
		{
			log_info("Just quick pressed");
			Mouse.justQuickPressed = true;
			if(Double_Click_Timer.tv_sec == -1)
				gettimeofday(&Double_Click_Timer, NULL);
			else
			{
				Mouse.just_DoubleClicked = true;
				log_info("Just double clicked");
				Double_Click_Timer.tv_sec = -1;
			}
		}
	}
}

static void mouse_scroll_callback(GLFWwindow *Window, double x, double y)
{
	Mouse.justScrolled = true;
	Mouse.scrollX = x;
	Mouse.scrollY = y;
}

/*
 * Console.h
 *
 *  Created on: Aug 29, 2015
 *      Author: mathi
 */

#ifndef APPLICATION_CONSOLE_CONSOLE_H_
#define APPLICATION_CONSOLE_CONSOLE_H_

#include "../Editor.h"

struct Lua_Console
{
	float Width, Height;
	int Ligne_inWindow;
	float Ligne_Height, Default_Ligne_Height;
	struct Quad Background;

	struct vector_t *Lignes;

	struct Gui_Horizontal_ScrollBar *ScrollBar;
	struct Gui_Vertical_ScrollBar *VSrollBar;

	struct Image *Close_Text;
	struct Quad Close;

	_Bool Minimized;

	struct Quad Minimize_Render, Minimize;

	int MaxWidth_Index;
};

struct Lua_Console *Console;

void Console_Init();
int Console_Render();
void Console_addLigne(char *Data);
void Console_Close();

#endif /* APPLICATION_CONSOLE_CONSOLE_H_ */

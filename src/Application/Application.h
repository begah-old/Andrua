/*
 * Application.h
 *
 *  Created on: Jul 15, 2015
 *      Author: begah
 */

#ifndef APPLICATION_APPLICATION_H_
#define APPLICATION_APPLICATION_H_

#include "../Util/Util.h"

char *Project_Name;

struct Project {
	char *Script_toExecute;
} Project;

//Give underlying code to have access to disable the checking of switching between different screens
_Bool Cancel_SwitchScreen;

void Application_Init();
void Application_Render();
void Application_Free();

#include "Editor/Editor.h"
#include "Image/Image_Editor.h"
#include "Settings/Settings.h"

#endif /* APPLICATION_APPLICATION_H_ */

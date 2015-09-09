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

void Application_Init();
void Application_Render();
void Application_Free();

#include "Editor/Editor.h"
#include "Image/Image_Editor.h"

#endif /* APPLICATION_APPLICATION_H_ */

//
// Created by root on 8/15/15.
//

#ifndef LUAEDITOR_PLATFORM_H
#define LUAEDITOR_PLATFORM_H

#ifndef ANDROID
/* Comment the following line if compiling for Desktop */
#define ANDROID
#endif

#ifndef ANDROID

// OpenGL/GLFW Includes
#include "GL/gl3w.h"

#include <GL/glu.h>
#include <GLFW/glfw3.h>
#else
#include <GLES2/gl2.h>
#include <string.h>
#include <jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#endif

#ifndef GL_RED
#ifdef GL_ALPHA
#define GL_RED GL_ALPHA
#else
#define GL_RED GL_LUMINANCE
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define __need_timeval
#include <time.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdarg.h>
#include <ctype.h>

#ifndef _WIN32
#include <unistd.h>
#endif

#ifndef S_IFDIR
#define S_IFDIR	__S_IFDIR
#endif

#endif //LUAEDITOR_PLATFORM_H

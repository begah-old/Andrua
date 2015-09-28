/*
 * debug.c
 *
 *  Created on: Jun 9, 2015
 *      Author: begah
 */

#include "debug.h"

#include "../Util.h"

int printOglError(char *file, int line)
{
    int glErr;

    glErr = glGetError();
    if (glErr != GL_NO_ERROR)
    {
#ifndef ANDROID
        printf("glError in file %s @ line %d: %s\n",
			     file, line, gluErrorString(glErr));
#else
        printf("glError in file %s @ line %d: ",
               file, line);
        JNIEnv *env;
        int isAttached = 0, status;

        if ((status = (*Java_VM)->GetEnv(Java_VM, (void**)&env, JNI_VERSION_1_6)) < 0) {
            if ((status = (*Java_VM)->AttachCurrentThread(Java_VM, &env, NULL)) < 0) {
                return 0;
            }
            isAttached = 1;
        }

        jclass clazz = (*env)->FindClass(env, "com/andrua/Native");
        jmethodID mid = (*env)->GetStaticMethodID(env, clazz, "PrintOpenglError", "(I)V");
        (*env)->CallStaticVoidMethod(env, clazz, mid, glErr);

        if (isAttached) (*Java_VM)->DetachCurrentThread(Java_VM);
#endif
        return 1;
    }
    return 0;
}

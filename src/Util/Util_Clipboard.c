/*
 * Util_Clipboard.c
 *
 *  Created on: Aug 17, 2015
 *      Author: root
 */

#include "Util.h"

struct Window *Window;

void Clipboard_setWindow(struct Window *Windo)
{
	Window = Windo;
}

#ifndef ANDROID

char *Clipboard_get()
{
	const char *String = glfwGetClipboardString(Window->Window);

	char *S = malloc(sizeof(char) * (String_length(String) + 1));
	for(int i = 0; i < String_length(String) + 1; i++)
		S[i] = Character_isValid(String[i]) ? String[i] : ' ';

	return S;
}

void Clipboard_set(const char *String)
{
	glfwSetClipboardString(Window->Window, String);
}

#else

char *Clipboard_get()
{
	JNIEnv *env;
	int isAttached = 0, status;

	if ((status = (*Java_VM)->GetEnv(Java_VM, (void**)&env, JNI_VERSION_1_6)) < 0) {
		if ((status = (*Java_VM)->AttachCurrentThread(Java_VM, &env, NULL)) < 0) {
			return NULL;
		}
		isAttached = 1;
	}

	jclass clazz = (*env)->FindClass(env, "com/andrua/Native");
	jmethodID mid = (*env)->GetStaticMethodID(env, clazz, "getClipboard", "()Ljava/lang/String;");
	jstring s = (jstring)(*env)->CallStaticObjectMethod(env, clazz, mid);

	const char *Temp = (*env)->GetStringUTFChars(env, s, NULL);
	char *Return = malloc(sizeof(char) * (String_length(Temp) + 1));

	for(int i = 0; i < String_length(Temp) + 1; i++)
		Return[i] = Character_isValid(Temp[i]) ? Temp[i] : ' ';

	(*env)->ReleaseStringUTFChars(env, s, Temp);
	if (isAttached) (*Java_VM)->DetachCurrentThread(Java_VM);

	return Return;
}

void Clipboard_set(const char *String)
{
	JNIEnv *env;
	int isAttached = 0, status;

	if ((status = (*Java_VM)->GetEnv(Java_VM, (void**)&env, JNI_VERSION_1_6)) < 0) {
		if ((status = (*Java_VM)->AttachCurrentThread(Java_VM, &env, NULL)) < 0) {
			return;
		}
		isAttached = 1;
	}

	jclass clazz = (*env)->FindClass(env, "com/andrua/Native");
	jmethodID mid = (*env)->GetStaticMethodID(env, clazz, "setClipboard", "(Ljava/lang/String;)V");

	jstring s = (*env)->NewStringUTF(env, String);

	(*env)->CallStaticVoidMethod(env, clazz, mid, s);
}

#endif

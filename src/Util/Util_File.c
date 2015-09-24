/*
 * Util_File.c
 *
 *  Created on: Jun 25, 2015
 *      Author: begah
 */

#include "Util.h"

#ifdef _WIN32
extern int mkdir(const char *pathname, mode_t mode);
#endif

void Dir_Create(char *name)
{
	struct stat st = {0};

    int PathLength = String_length(name), ExecutableLength = String_length(
			Executable_Path);
	char *FullPath = malloc(sizeof(char) * (PathLength + ExecutableLength + 1));
	FullPath = memcpy(FullPath, Executable_Path,
			sizeof(char) * ExecutableLength);
	FullPath += ExecutableLength;
	FullPath = memcpy(FullPath, name, sizeof(char) * PathLength);
	FullPath[PathLength] = '\0';
	FullPath -= ExecutableLength;

	log_info("Creating folder : %s", FullPath);
	if (stat(FullPath, &st) == -1) {
	    mkdir(FullPath, 0700);
	}
	free(FullPath);
	return;
}

_Bool Dir_Exists(char *name)
{
    struct stat st = {0};

    int PathLength = String_length(name), ExecutableLength = String_length(
			Executable_Path);
	char *FullPath = malloc(sizeof(char) * (PathLength + ExecutableLength + 1));
	FullPath = memcpy(FullPath, Executable_Path,
			sizeof(char) * ExecutableLength);
	FullPath += ExecutableLength;
	FullPath = memcpy(FullPath, name, sizeof(char) * PathLength);
	FullPath[PathLength] = '\0';
	FullPath -= ExecutableLength;

	if (stat(FullPath, &st) == -1) {
		free(FullPath);
	    return false;
	}
	free(FullPath);

	return st.st_mode & S_IFDIR;
}

struct F_FileExternal *FileExternal_Open(const char *FilePath)
{
	struct F_FileExternal *File = malloc(sizeof(struct F_FileExternal));

	int PathLength = String_length(FilePath), ExecutableLength = String_length(Executable_Path);
	char *FullPath = malloc(sizeof(char) * (PathLength + ExecutableLength + 1));
	FullPath = memcpy(FullPath, Executable_Path,
						  sizeof(char) * ExecutableLength);
	FullPath += ExecutableLength;
	FullPath = memcpy(FullPath, FilePath, sizeof(char) * PathLength);
	FullPath[PathLength] = '\0';
	FullPath -= ExecutableLength;

	File->File = fopen(FullPath, "rb");

	free(FullPath);
	return File;
}

struct F_FileExternal *FileExternal_OpenWrite(const char *FilePath, _Bool NeedRead, _Bool DontOverride)
{
	struct F_FileExternal *File = malloc(sizeof(struct F_FileExternal));

	int PathLength = String_length(FilePath), ExecutableLength = String_length(Executable_Path);
	char *FullPath = malloc(sizeof(char) * (PathLength + ExecutableLength + 1));
	FullPath = memcpy(FullPath, Executable_Path,
						  sizeof(char) * ExecutableLength);
	FullPath += ExecutableLength;
	FullPath = memcpy(FullPath, FilePath, sizeof(char) * PathLength);
	FullPath[PathLength] = '\0';
	FullPath -= ExecutableLength;

	if(NeedRead && !DontOverride)
		File->File = fopen(FullPath, "w+");
	else if(NeedRead) File->File = fopen(FullPath, "r+");
	else
		File->File = fopen(FullPath, "wb");

	free(FullPath);
	return File;
}

_Bool FileExternal_Exists(const char *FilePath)
{
	int PathLength = String_length(FilePath), ExecutableLength = String_length(Executable_Path);
	char *FullPath = malloc(sizeof(char) * (PathLength + ExecutableLength + 1));
	FullPath = memcpy(FullPath, Executable_Path,
						  sizeof(char) * ExecutableLength);
	FullPath += ExecutableLength;
	FullPath = memcpy(FullPath, FilePath, sizeof(char) * PathLength);
	FullPath[PathLength] = '\0';
	FullPath -= ExecutableLength;

    struct stat st;
    int result = stat(FullPath, &st);
    free(FullPath);
    return result == 0;
}

long int FileExternal_Length(struct F_FileExternal *file)
{
	fseek(file->File, 0L, SEEK_END);
	long int sz = ftell(file->File);
	fseek(file->File, 0L, SEEK_SET);
	return sz;
}

FILE *FileExternal_getCFile(struct F_FileExternal *file)
{
	return file->File;
}

char *FileExternal_GetFullPath(const char *FileName)
{
	int PathLength = String_length(FileName), ExecutableLength = String_length(Executable_Path);
	char *FullPath = malloc(sizeof(char) * (PathLength + ExecutableLength + 1));
	FullPath = memcpy(FullPath, Executable_Path,
						  sizeof(char) * ExecutableLength);
	FullPath += ExecutableLength;
	FullPath = memcpy(FullPath, FileName, sizeof(char) * PathLength);
	FullPath[PathLength] = '\0';
	FullPath -= ExecutableLength;

	return FullPath;
}

struct F_FileExternal *FileExternal_fromFile(FILE *file)
{
	struct F_FileExternal *F = malloc(sizeof(struct F_FileExternal));
	F->File = file;
	return F;
}

void FileExternal_Seek(struct F_FileExternal *file, int offset, int whence)
{
	fseek(file->File, offset, whence);
}

int FileExternal_Tell(struct F_FileExternal *file)
{
	return ftell(file->File);
}

int FileExternal_Read(void *ptr, size_t Unit_size, int Unit_number, struct F_FileExternal *file)
{
	return fread(ptr, Unit_size, Unit_number, file->File);
}

void FileExternal_Write(void *ptr, size_t Unit_size, int Unit_number, struct F_FileExternal *file)
{
	fwrite(ptr, Unit_size, Unit_number, file->File);
}

void FileExternal_Close(struct F_FileExternal *file)
{
	fclose(file->File);
	free(file);
}

#ifndef ANDROID
struct F_FileInternal *FileInternal_Open(const char *FilePath)
{
	int PathLength = String_length(FilePath), ExecutableLength = String_length(
			Asset_Path);
	char *FullPath = malloc(sizeof(char) * (PathLength + ExecutableLength + 1));
	FullPath = memcpy(FullPath, Asset_Path,
					  sizeof(char) * ExecutableLength);
	FullPath += ExecutableLength;
	FullPath = memcpy(FullPath, FilePath, sizeof(char) * PathLength);
	FullPath[PathLength] = '\0';
	FullPath -= ExecutableLength;

	struct F_FileInternal *file = malloc(sizeof(struct F_FileInternal));
	file->File = fopen(FullPath, "rb");
	free(FullPath);

	if(!file->File)
    {
        log_info("File not found %s", FilePath);
        free(file);
        return NULL;
    }

	return file;
}

long int FileInternal_Length(struct F_FileInternal *file)
{
	fseek(file->File, 0L, SEEK_END);
	long int sz = ftell(file->File);
	fseek(file->File, 0L, SEEK_SET);
	return sz;
}

_Bool FileInternal_Exists(const char *FilePath)
{
	int PathLength = String_length(FilePath), ExecutableLength = String_length(Asset_Path);
	char *FullPath = malloc(sizeof(char) * (PathLength + ExecutableLength + 1));
	FullPath = memcpy(FullPath, Asset_Path,
						  sizeof(char) * ExecutableLength);
	FullPath += ExecutableLength;
	FullPath = memcpy(FullPath, FilePath, sizeof(char) * PathLength);
	FullPath[PathLength] = '\0';
	FullPath -= ExecutableLength;

    struct stat st;
    int result = stat(FullPath, &st);
    free(FullPath);
    return result == 0;
}

int FileInternal_Read(void *ptr, size_t Unit_size, int Unit_number, struct F_FileInternal *file)
{
	return fread(ptr, Unit_size, Unit_number, file->File);
}

void FileInternal_Seek(struct F_FileInternal *file, int offset, int whence)
{
	fseek(file->File, offset, whence);
}

void FileInternal_Close(struct F_FileInternal *file)
{
	fclose(file->File);
	free(file);
}
#elif defined(ANDROID)
struct F_FileInternal *FileInternal_Open(const char *FilePath)
{
	JNIEnv *env;
	int isAttached = 0, status;
	if ((status = (*Java_VM)->GetEnv(Java_VM, (void**)&env, JNI_VERSION_1_6)) < 0) {
		if ((status = (*Java_VM)->AttachCurrentThread(Java_VM, &env, NULL)) < 0) {
			struct F_FileInternal file;
			file.env = NULL;
		}
		isAttached = 1;
	}

	AAssetManager* mgr = AAssetManager_fromJava(env, Java_AssetManager);
	AAsset* asset = AAssetManager_open(mgr,FilePath,AASSET_MODE_BUFFER);

	if(!asset)
	{
		log_info("File not found %s", FilePath);
		return NULL;
	}

	struct F_FileInternal *file = malloc(sizeof(struct F_FileInternal));
	file->env = env;
	file->mgr = mgr;
	file->asset = asset;
	file->isAttached = isAttached;

	return file;
}

_Bool FileInternal_Exists(const char *FilePath)
{
	JNIEnv *env;
	int isAttached = 0, status;
	if ((status = (*Java_VM)->GetEnv(Java_VM, (void**)&env, JNI_VERSION_1_6)) < 0) {
		if ((status = (*Java_VM)->AttachCurrentThread(Java_VM, &env, NULL)) < 0) {
			struct F_FileInternal file;
			file.env = NULL;
		}
		isAttached = 1;
	}

	AAssetManager* mgr = AAssetManager_fromJava(env, Java_AssetManager);
	AAsset* asset = AAssetManager_open(mgr,FilePath,AASSET_MODE_BUFFER);

	_Bool Result = asset != NULL;
	AAsset_close(asset);
	if (isAttached) (*Java_VM)->DetachCurrentThread(Java_VM);

	return Result;
}

long int FileInternal_Length(struct F_FileInternal *file)
{
	if(!file->env) return -1;
	return AAsset_getLength(file->asset);
}

void FileInternal_Seek(struct F_FileInternal *file, int offset, int whence)
{
	if(!file->env) return;
	AAsset_seek(file->asset, offset, whence);
}

int FileInternal_Read(void *ptr, size_t Unit_size, int Unit_number, struct F_FileInternal *file)
{
	if(!file->env) return 0;
	return AAsset_read(file->asset, ptr, Unit_size * Unit_number);
}

void FileInternal_Close(struct F_FileInternal *file)
{
	if(!file->env) return;
	AAsset_close(file->asset);

	if (file->isAttached) (*Java_VM)->DetachCurrentThread(Java_VM);
}
#endif

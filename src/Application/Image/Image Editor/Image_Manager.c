/*
 * Image_Manager.c
 *
 *  Created on: Jul 18, 2015
 *      Author: begah
 */

#include "Image_Manager.h"

unsigned int Image_createTexture(struct Image_RawData *Image)
{
	unsigned int Texture;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Image->Width,
			Image->Height, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, Image->Data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return Texture;
}

struct Image_RawData *Image_loadPNG(const char *Name)
{
	struct Image_RawData *Image = malloc(sizeof(struct Image_RawData));

	char *Path = FileExternal_GetFullPath(Name);
	Image->Data = SOIL_load_image(Path, (int *)(&Image->Width),
			(int *)(&Image->Height), NULL, SOIL_LOAD_RGBA);
	free(Path);

	return Image;
}

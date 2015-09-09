//
// Created by root on 8/9/15.
//

#include "Util.h"

#ifndef ANDROID

// Render for all application

#define VERTICES_MAX 1080
#define FONT_VERTICES_MAX 6000
#define TEXTURE_MAX 15

// Default Shader (color and blending)
GLuint DS_Shader, DS_Shader_VBO;
GLint DS_Shader_uni;
int DS_Vertices_Count = 0;

struct Vector6f DS_Vertices[VERTICES_MAX];

static void DS_Flush()
{
	if(DS_Vertices_Count)
	{
		glUseProgram(DS_Shader);
		glUniform2f(DS_Shader_uni, Game_Width, Game_Height);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, DS_Shader_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0,
				sizeof(struct Vector6f) * DS_Vertices_Count, DS_Vertices);

		glBindBuffer(GL_ARRAY_BUFFER, DS_Shader_VBO);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vector6f), 0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(struct Vector6f),
				(const GLvoid*) 8);
		glDrawArrays(GL_TRIANGLES, 0, DS_Vertices_Count);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glUseProgram(0);

		for (int i = 0; i < DS_Vertices_Count; i++)
			DS_Vertices[i].x = DS_Vertices[i].y = 0;
		DS_Vertices_Count = 0;
	}
}

static void DS_Push_Vertice(float x, float y, float r, float g, float b,
		float a)
{
	if (DS_Vertices_Count == VERTICES_MAX)
		DS_Flush();
	DS_Vertices[DS_Vertices_Count].x = x;
	DS_Vertices[DS_Vertices_Count].y = y;
	DS_Vertices[DS_Vertices_Count].z = r;
	DS_Vertices[DS_Vertices_Count].w = g;
	DS_Vertices[DS_Vertices_Count].h = b;
	DS_Vertices[DS_Vertices_Count].o = a;
	DS_Vertices_Count++;
}

static void DS_Push_Quad(struct Quad Quad, struct Vector4f Color)
{
	DS_Push_Vertice(Quad.v1.x, Quad.v1.y, Color.x, Color.y, Color.z, Color.w);
	DS_Push_Vertice(Quad.v3.x, Quad.v3.y, Color.x, Color.y, Color.z, Color.w);
	DS_Push_Vertice(Quad.v2.x, Quad.v2.y, Color.x, Color.y, Color.z, Color.w);

	DS_Push_Vertice(Quad.v1.x, Quad.v1.y, Color.x, Color.y, Color.z, Color.w);
	DS_Push_Vertice(Quad.v4.x, Quad.v4.y, Color.x, Color.y, Color.z, Color.w);
	DS_Push_Vertice(Quad.v3.x, Quad.v3.y, Color.x, Color.y, Color.z, Color.w);
}

// Image Shader (texture and blending)
GLuint IS_Shader, IS_Shader_VBO, IS_Shader_IBO;
GLint IS_Shader_uni, IS_Shader_uni2;

int IS_Texture_Count = 0;
struct Vector4f IS_Vector[4];

GLuint IS_Texture[TEXTURE_MAX];
struct Vector4f IS_Vertices[TEXTURE_MAX][VERTICES_MAX / TEXTURE_MAX];
struct Vector4f IS_Blending[TEXTURE_MAX][VERTICES_MAX / TEXTURE_MAX];
int IS_Vertices_Count[TEXTURE_MAX];

static void IS_Flush()
{
	glUseProgram(IS_Shader);
	glUniform2f(IS_Shader_uni, Game_Width, Game_Height);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, IS_Shader_VBO);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vector4f), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vector4f),
			(const GLvoid*) 8);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IS_Shader_IBO);

	for (int TextureID = 0; TextureID < IS_Texture_Count; TextureID++)
	{
		glBindTexture(GL_TEXTURE_2D, IS_Texture[TextureID]);
		struct Vector4f Blend =
		{ -2, -2, -2, -2 };
		for (int i = 0; i < IS_Vertices_Count[TextureID]; i += 4)
		{
			IS_Vector[0] = IS_Vertices[TextureID][i];
			IS_Vector[1] = IS_Vertices[TextureID][i + 1];
			IS_Vector[2] = IS_Vertices[TextureID][i + 2];
			IS_Vector[3] = IS_Vertices[TextureID][i + 3];
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(IS_Vector), IS_Vector);

			if (Blend.x != IS_Blending[TextureID][i].x
					|| Blend.y != IS_Blending[TextureID][i].y
					|| Blend.z != IS_Blending[TextureID][i].z
					|| Blend.w != IS_Blending[TextureID][i].w)
			{
				glUniform4f(IS_Shader_uni2, IS_Blending[TextureID][i].x,
						IS_Blending[TextureID][i].y,
						IS_Blending[TextureID][i].z,
						IS_Blending[TextureID][i].w);
				Blend = IS_Blending[TextureID][i];
			}

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	printOpenGLError();

	IS_Texture_Count = 0;
}

static void IS_Push_Vertice(float x, float y, float textx, float texty,
		GLuint Texture, struct Vector4f Blending)
{
	int ID = -1;
	for (int i = 0; i < IS_Texture_Count; i++)
	{

		if (IS_Texture[i] == Texture && IS_Vertices_Count[i] < VERTICES_MAX / TEXTURE_MAX)
		{
			ID = i;
			break;
		}
	}
	if (ID == -1)
	{
		if (IS_Texture_Count == TEXTURE_MAX)
			IS_Flush();
		IS_Texture[IS_Texture_Count] = Texture;
		ID = IS_Texture_Count;
		IS_Vertices_Count[IS_Texture_Count] = 0;
		IS_Texture_Count++;
	}
	if (IS_Vertices_Count[ID] == VERTICES_MAX / TEXTURE_MAX)
	{
		IS_Flush();
	}

	IS_Vertices[ID][IS_Vertices_Count[ID]] = Vector4_Create(x, y, textx, texty);
	IS_Blending[ID][IS_Vertices_Count[ID]] = Blending;
	IS_Vertices_Count[ID]++;
}

void IS_Push_Quad(struct Quad Quad, struct Quad TextQuad, GLuint Text,
		struct Vector4f Blend)
{
	IS_Push_Vertice(Quad.v1.x, Quad.v1.y, TextQuad.v1.x, TextQuad.v1.y, Text,
			Blend);
	IS_Push_Vertice(Quad.v2.x, Quad.v2.y, TextQuad.v2.x, TextQuad.v2.y, Text,
			Blend);
	IS_Push_Vertice(Quad.v3.x, Quad.v3.y, TextQuad.v3.x, TextQuad.v3.y, Text,
			Blend);
	IS_Push_Vertice(Quad.v4.x, Quad.v4.y, TextQuad.v4.x, TextQuad.v4.y, Text,
			Blend);
}

// Font Shader
GLuint FS_Shader, FS_Shader_VBO;
GLint FS_Shader_uni, FS_Shader_uni2;
int FS_Texture_Count = 0;

struct Vector4f FS_Vertices[TEXTURE_MAX][FONT_VERTICES_MAX];
struct Vector4f FS_Color[TEXTURE_MAX][FONT_VERTICES_MAX];
int FS_Vertices_Count[TEXTURE_MAX];
GLuint FS_Font_Image[TEXTURE_MAX];

static void FS_Flush()
{
	printOpenGLError();

	glUseProgram(FS_Shader);
	glEnableVertexAttribArray(0);
	glUniform2f(FS_Shader_uni, Game_Width, Game_Height);

	glActiveTexture(GL_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, FS_Shader_VBO);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IS_Shader_IBO);

	for (int i = 0; i < FS_Texture_Count; i++)
	{
		glBindTexture(GL_TEXTURE_2D, FS_Font_Image[i]);
		// Update content of VBO memory
		struct Vector4f Color = Vector4_Create(-1, -1, -1, -1);

		for (int j = 0; j < FS_Vertices_Count[i]; j += 4)
		{
			if (Color.x != FS_Color[i][j].x || Color.y != FS_Color[i][j].y
					|| Color.z != FS_Color[i][j].z
					|| Color.w != FS_Color[i][j].w)
			{
				Color = FS_Color[i][j];
				glUniform4f(FS_Shader_uni2, Color.x, Color.y, Color.z, Color.w);
			}
			IS_Vector[0] = FS_Vertices[i][j];
			IS_Vector[1] = FS_Vertices[i][j + 1];
			IS_Vector[2] = FS_Vertices[i][j + 2];
			IS_Vector[3] = FS_Vertices[i][j + 3];
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(IS_Vector), IS_Vector); // Be sure to use glBufferSubData and not glBufferData

			// Render quad
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	FS_Texture_Count = 0;
	printOpenGLError();
}

static void FS_Push_Vertice(GLuint FontImage, float x, float y, float textx,
		float texty, struct Vector4f Color)
{
	int ID = -1;
	for (int i = 0; i < FS_Texture_Count; i++)
	{
		if (FS_Font_Image[i]
				== FontImage&& FS_Vertices_Count[i] < FONT_VERTICES_MAX)
		{
			ID = i;
			break;
		}
	}
	if (ID == -1)
	{
		if (FS_Texture_Count == TEXTURE_MAX)
			FS_Flush();
		FS_Font_Image[FS_Texture_Count] = FontImage;
		ID = FS_Texture_Count;
		FS_Vertices_Count[FS_Texture_Count] = 0;
		FS_Texture_Count++;
	}
	if (FS_Vertices_Count[ID] == (int) FONT_VERTICES_MAX)
	{

		FS_Flush();
	}
	FS_Vertices[ID][FS_Vertices_Count[ID]] = Vector4_Create(x, y, textx, texty);
	FS_Color[ID][FS_Vertices_Count[ID]] = Color;
	FS_Vertices_Count[ID]++;
}

static void FS_Push_Quad(GLuint FontImage, struct Quad Quad,
		struct Quad TextQuad, struct Vector4f Color)
{
	FS_Push_Vertice(FontImage, Quad.v1.x, Quad.v1.y, TextQuad.v1.x,
			TextQuad.v1.y, Color);
	FS_Push_Vertice(FontImage, Quad.v2.x, Quad.v2.y, TextQuad.v2.x,
			TextQuad.v2.y, Color);
	FS_Push_Vertice(FontImage, Quad.v3.x, Quad.v3.y, TextQuad.v3.x,
			TextQuad.v3.y, Color);
	FS_Push_Vertice(FontImage, Quad.v4.x, Quad.v4.y, TextQuad.v4.x,
			TextQuad.v4.y, Color);

}

static void LoadShaders()
{
	log_info("Initializing shaders");
	DS_Shader = Shader_LoadDefault();
	DS_Shader_uni = glGetUniformLocation(DS_Shader, "ScreenSize");

	IS_Shader = Shader_LoadImage();
	IS_Shader_uni = glGetUniformLocation(IS_Shader, "ScreenSize");
	IS_Shader_uni2 = glGetUniformLocation(IS_Shader, "ColorBlend");

	int Indices[] =
	{ 0, 1, 2, 0, 3, 2 };
	glGenBuffers(1, &IS_Shader_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IS_Shader_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), &Indices,
	GL_STATIC_DRAW);

	struct Quad Quad = Quad_Create(0, 0, 0, 480, 640, 480, 640, 0);

	IS_Vector[0] = Vector4_Create(Quad.v1.x, Quad.v1.y, 0.0f, 0.0f);
	IS_Vector[1] = Vector4_Create(Quad.v2.x, Quad.v2.y, 0.0f, 1.0f);
	IS_Vector[2] = Vector4_Create(Quad.v3.x, Quad.v3.y, 1.0f, 1.0f);
	IS_Vector[3] = Vector4_Create(Quad.v4.x, Quad.v4.y, 1.0f, 0.0f);

	glGenBuffers(1, &IS_Shader_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, IS_Shader_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(IS_Vector), IS_Vector,
	GL_DYNAMIC_DRAW);

	glGenBuffers(1, &DS_Shader_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, DS_Shader_VBO);
	glBufferData(GL_ARRAY_BUFFER, VERTICES_MAX * sizeof(struct Vector6f),
	NULL, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &FS_Shader_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, FS_Shader_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(IS_Vector), IS_Vector,
	GL_DYNAMIC_DRAW);

	FS_Shader = glCreateProgram();
	check(FS_Shader >= 0, "Couldn't create shader program");

	const char * vs =
			"#version 330 core\n"
					"layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>\n"
					"uniform vec2 ScreenSize;\n"
					"out vec2 TexCoords;\n"
					"void main()\n"
					"{\n"
					"gl_Position = vec4(vertex.x/ScreenSize.x*2-1, vertex.y/ScreenSize.y*2-1, 0.0, 1.0);\n"
					"TexCoords = vertex.zw;\n"
					"}\n "
					"\n ";
	GLuint vso = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vso, 1, (const char **) &vs, NULL);
	glCompileShader(vso);
	glAttachShader(FS_Shader, vso);

	const char * fs = "#version 330 core\n"
			"in vec2 TexCoords;\n"
			"out vec4 color;\n"
			"uniform sampler2D text;\n"
			"uniform vec4 textColor;\n"
			"void main()\n"
			"{\n"
			"vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
			"color = textColor * sampled;\n"
			"}\n "
			"\n ";
	GLuint fso = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fso, 1, (const char **) &fs, NULL);
	glCompileShader(fso);
	glAttachShader(FS_Shader, fso);

	glLinkProgram(FS_Shader);
	glDeleteShader(vso);
	glDeleteShader(fso);

	FS_Shader_uni = glGetUniformLocation(FS_Shader, "ScreenSize");
	FS_Shader_uni2 = glGetUniformLocation(FS_Shader, "textColor");

	printOpenGLError();

	log_info("Done loading shader");

	return;
	error: Application_Error(); return;
}

void Renderer_SetUp()
{
	log_info("Initialization of renderer");
	LoadShaders();

	Default_Shader.pushVertices = DS_Push_Vertice;
	Default_Shader.pushQuad = DS_Push_Quad;

	Image_Shader.pushVertices = IS_Push_Vertice;
	Image_Shader.pushQuad = IS_Push_Quad;

	Font_Shader.pushVertices = FS_Push_Vertice;
	Font_Shader.pushQuad = FS_Push_Quad;
	log_info("Done initializing renderer");
}

void Renderer_Flush()
{
	DS_Flush();
	IS_Flush();
	FS_Flush();
}

void Renderer_CleanUp()
{
	log_info("Renderer clean up start");
	Shader_Free(DS_Shader);
	Shader_Free(IS_Shader);
	Shader_Free(FS_Shader);

	glDeleteBuffers(1, &DS_Shader_VBO);
	glDeleteBuffers(1, &IS_Shader_VBO);
	glDeleteBuffers(1, &FS_Shader_VBO);
	glDeleteBuffers(1, &IS_Shader_IBO);
	log_info("Renderer clean up end");
}

#endif

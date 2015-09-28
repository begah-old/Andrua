//
// Created by root on 8/9/15.
//

#include "Util.h"

#ifdef ANDROID

#define VERTICES_MAX 1080
#define FONT_VERTICES_MAX 1080
#define TEXTURE_MAX 10

GLuint DS_Shader, DS_Attribute_Pos, DS_Attribute_Color, DS_ScreenSize;

int DS_Vertices_Count = 0;

struct Vector3f DS_Vertices[VERTICES_MAX];
struct Vector4f DS_VerticesColor[VERTICES_MAX];

static void DS_Flush()
{
    glUseProgram(DS_Shader);

    glUniform2f(DS_ScreenSize, Game_Width, Game_Height);

    glEnableVertexAttribArray(DS_Attribute_Pos);
    glEnableVertexAttribArray(DS_Attribute_Color);

    glVertexAttribPointer(DS_Attribute_Pos, 3,
                          GL_FLOAT, false,
                          0, DS_Vertices);
    glVertexAttribPointer(DS_Attribute_Color, 4,
                          GL_FLOAT, false,
                          0, DS_VerticesColor);

    glDrawArrays(GL_TRIANGLES, 0, DS_Vertices_Count);
    printOpenGLError();
    glDisableVertexAttribArray(DS_Attribute_Pos);
    glDisableVertexAttribArray(DS_Attribute_Color);

    glUseProgram(0);

    printOpenGLError();

    DS_Vertices_Count = 0;
}

static void DS_Push_Vertice(float x, float y, float r, float g, float b, float a, int z)
{
    if(DS_Vertices_Count == VERTICES_MAX)
        DS_Flush();
    DS_Vertices[DS_Vertices_Count] = Vector3_Create(x, y, z);
    DS_VerticesColor[DS_Vertices_Count] = Vector4_Create(r, g, b, a);
    DS_Vertices_Count++;
}

static void DS_Push_Quad(struct Quad Quad, struct Vector4f Color, int z)
{
    DS_Push_Vertice(Quad.v1.x, Quad.v1.y, Color.x, Color.y, Color.z, Color.w, z);
    DS_Push_Vertice(Quad.v3.x, Quad.v3.y, Color.x, Color.y, Color.z, Color.w, z);
    DS_Push_Vertice(Quad.v2.x, Quad.v2.y, Color.x, Color.y, Color.z, Color.w, z);

    DS_Push_Vertice(Quad.v1.x, Quad.v1.y, Color.x, Color.y, Color.z, Color.w, z);
    DS_Push_Vertice(Quad.v4.x, Quad.v4.y, Color.x, Color.y, Color.z, Color.w, z);
    DS_Push_Vertice(Quad.v3.x, Quad.v3.y, Color.x, Color.y, Color.z, Color.w, z);
}

GLuint IS_Shader, IS_Attribute_Pos, IS_Attribute_Text;
GLint IS_Uniform_ScreenSize, IS_Shader_uni2;

int IS_Texture_Count = 0;
struct Vector5f IS_Vector[6];

GLuint IS_Texture[TEXTURE_MAX];
struct Vector5f IS_Vertices[TEXTURE_MAX][VERTICES_MAX / TEXTURE_MAX];
struct Vector4f IS_Blending[TEXTURE_MAX][VERTICES_MAX / TEXTURE_MAX];
int IS_Vertices_Count[TEXTURE_MAX];

static void IS_Flush() {
    glUseProgram(IS_Shader);
    glUniform2f(IS_Uniform_ScreenSize, Game_Width, Game_Height);

    glEnableVertexAttribArray(IS_Attribute_Pos);
    glEnableVertexAttribArray(IS_Attribute_Text);

    for (int TextureID = 0; TextureID < IS_Texture_Count; TextureID++) {
        glBindTexture(GL_TEXTURE_2D, IS_Texture[TextureID]);
        struct Vector4f Blend = { -2, -2, -2, -2 };
        for (int i = 0; i < IS_Vertices_Count[TextureID]; i += 4) {
            IS_Vector[0] = IS_Vertices[TextureID][i];
            IS_Vector[1] = IS_Vertices[TextureID][i + 1];
            IS_Vector[2] = IS_Vertices[TextureID][i + 2];
            IS_Vector[3] = IS_Vertices[TextureID][i + 2];
            IS_Vector[4] = IS_Vertices[TextureID][i];
            IS_Vector[5] = IS_Vertices[TextureID][i + 3];
            glVertexAttribPointer(IS_Attribute_Pos, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vector5f), IS_Vector);
            glVertexAttribPointer(IS_Attribute_Text, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vector5f), &IS_Vector[0].w);

            if (Blend.x != IS_Blending[TextureID][i].x
                || Blend.y != IS_Blending[TextureID][i].y
                || Blend.z != IS_Blending[TextureID][i].z
                || Blend.w != IS_Blending[TextureID][i].w) {printOpenGLError();
                glUniform4f(IS_Shader_uni2, IS_Blending[TextureID][i].x,
                            IS_Blending[TextureID][i].y,
                            IS_Blending[TextureID][i].z,
                            IS_Blending[TextureID][i].w);printOpenGLError();
                Blend = IS_Blending[TextureID][i];
            }

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }

    glDisableVertexAttribArray(IS_Attribute_Pos);
    glDisableVertexAttribArray(IS_Attribute_Text);

    printOpenGLError();

    IS_Texture_Count = 0;
}

static void IS_Push_Vertice(float x, float y, float textx, float texty,
                            GLuint Texture, struct Vector4f Blending, int z)
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

    IS_Vertices[ID][IS_Vertices_Count[ID]] = Vector5_Create(x, y, z, textx, texty);
    IS_Blending[ID][IS_Vertices_Count[ID]] = Blending;
    IS_Vertices_Count[ID]++;
}

void IS_Push_Quad(struct Quad Quad, struct Quad TextQuad, GLuint Text,
                  struct Vector4f Blend, int z) {
    IS_Push_Vertice(Quad.v1.x, Quad.v1.y, TextQuad.v1.x, TextQuad.v1.y, Text,
                    Blend, z);
    IS_Push_Vertice(Quad.v2.x, Quad.v2.y, TextQuad.v2.x, TextQuad.v2.y, Text,
                    Blend, z);
    IS_Push_Vertice(Quad.v3.x, Quad.v3.y, TextQuad.v3.x, TextQuad.v3.y, Text,
                    Blend, z);
    IS_Push_Vertice(Quad.v4.x, Quad.v4.y, TextQuad.v4.x, TextQuad.v4.y, Text,
                    Blend, z);
}

// Font Shader
GLuint FS_Shader, FS_Shader_Attrib, FS_Shader_Attrib2;
GLint FS_Shader_uni, FS_Shader_uni2;
int FS_Texture_Count = 0;

struct Vector5f FS_Vertices[TEXTURE_MAX][FONT_VERTICES_MAX];
struct Vector4f FS_Color[TEXTURE_MAX][FONT_VERTICES_MAX];
int FS_Vertices_Count[TEXTURE_MAX];
GLuint FS_Font_Image[TEXTURE_MAX];

static void FS_Flush() {
    printOpenGLError();

    glUseProgram(FS_Shader);
    glUniform2f(FS_Shader_uni, Game_Width, Game_Height);

    glEnableVertexAttribArray(FS_Shader_Attrib);
    glEnableVertexAttribArray(FS_Shader_Attrib2);

    glActiveTexture(GL_TEXTURE0);

    for (int i = 0; i < FS_Texture_Count; i++) {
        glBindTexture(GL_TEXTURE_2D, FS_Font_Image[i]);
        // Update content of VBO memory
        struct Vector4f Color = Vector4_Create(-1, -1, -1, -1);

        for (int j = 0; j < FS_Vertices_Count[i]; j += 4) {
            if (Color.x != FS_Color[i][j].x || Color.y != FS_Color[i][j].y
                || Color.z != FS_Color[i][j].z
                || Color.w != FS_Color[i][j].w) {
                Color = FS_Color[i][j];
                glUniform4f(FS_Shader_uni2, Color.x, Color.y, Color.z, Color.w);
            }
            IS_Vector[0] = FS_Vertices[i][j];
            IS_Vector[1] = FS_Vertices[i][j + 1];
            IS_Vector[2] = FS_Vertices[i][j + 2];
            IS_Vector[3] = FS_Vertices[i][j + 2];
            IS_Vector[4] = FS_Vertices[i][j];
            IS_Vector[5] = FS_Vertices[i][j + 3];

            glVertexAttribPointer(FS_Shader_Attrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), IS_Vector);
            glVertexAttribPointer(FS_Shader_Attrib2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), &IS_Vector[0].w);

            // Render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    FS_Texture_Count = 0;

    glDisableVertexAttribArray(FS_Shader_Attrib);
    glDisableVertexAttribArray(FS_Shader_Attrib2);

    printOpenGLError();
}

static void FS_Push_Vertice(GLuint FontImage, float x, float y, float textx,
                            float texty, struct Vector4f Color, int z) {
    int ID = -1;
    for (int i = 0; i < FS_Texture_Count; i++) {
        if (FS_Font_Image[i] == FontImage && FS_Vertices_Count[i] < FONT_VERTICES_MAX) {
            ID = i;
            break;
        }
    }
    if (ID == -1) {
        if (FS_Texture_Count == TEXTURE_MAX)
            FS_Flush();
        FS_Font_Image[FS_Texture_Count] = FontImage;
        ID = FS_Texture_Count;
        FS_Vertices_Count[FS_Texture_Count] = 0;
        FS_Texture_Count++;
    }
    if (FS_Vertices_Count[ID] == (int)FONT_VERTICES_MAX)
    {
        FS_Flush();
    }
    FS_Vertices[ID][FS_Vertices_Count[ID]] = Vector5_Create(x, y, z, textx, texty);
    FS_Color[ID][FS_Vertices_Count[ID]] = Color;
    FS_Vertices_Count[ID]++;
}

static void FS_Push_Quad(GLuint FontImage, struct Quad Quad,
                         struct Quad TextQuad, struct Vector4f Color, int z) {
    FS_Push_Vertice(FontImage, Quad.v1.x, Quad.v1.y, TextQuad.v1.x,
                    TextQuad.v1.y, Color, z);
    FS_Push_Vertice(FontImage, Quad.v2.x, Quad.v2.y, TextQuad.v2.x,
                    TextQuad.v2.y, Color, z);
    FS_Push_Vertice(FontImage, Quad.v3.x, Quad.v3.y, TextQuad.v3.x,
                    TextQuad.v3.y, Color, z);
    FS_Push_Vertice(FontImage, Quad.v4.x, Quad.v4.y, TextQuad.v4.x,
                    TextQuad.v4.y, Color, z);

}

static void LoadShaders()
{
    log_info("Initializing shaders");
    DS_Shader = Shader_LoadDefault();
    printOpenGLError();

    if(!glIsProgram(DS_Shader))
    {
        log_err("Android Default Shader didn't succeed in creating");
        goto error;
    }

    DS_Attribute_Pos = glGetAttribLocation(DS_Shader, "vPosition");
    DS_Attribute_Color = glGetAttribLocation(DS_Shader, "Color");

    DS_ScreenSize = glGetUniformLocation(DS_Shader, "ScreenSize");
    printOpenGLError();

    IS_Shader = Shader_LoadImage();

    IS_Attribute_Pos = glGetAttribLocation(IS_Shader, "a_position");
    IS_Attribute_Text = glGetAttribLocation(IS_Shader, "a_texCoord");

    IS_Uniform_ScreenSize = glGetUniformLocation(IS_Shader, "ScreenSize");
    IS_Shader_uni2 = glGetUniformLocation(IS_Shader, "ColorBlend");

    FS_Shader = Shader_loadCustom("attribute vec3 vertex;\n"
                                          "attribute vec2 coords;\n"
                                          "uniform vec2 ScreenSize;\n"
                                          "varying vec2 TexCoords;\n"
                                          "void main()\n"
                                          "{\n"
                                          "gl_Position = vec4(vertex.x/ScreenSize.x*2.0-1.0, vertex.y/ScreenSize.y*2.0-1.0, vertex.z / 10.0, 1.0);\n"
                                          "TexCoords = coords;\n"
                                          "}\n "
                                          "\n ",
                                  "precision mediump float;\n"
                                          "varying vec2 TexCoords;\n"
                                          "uniform sampler2D text;\n"
                                          "uniform vec4 textColor;\n"
                                          "void main()\n"
                                          "{\n"
                                          "vec4 sampled = vec4(1.0, 1.0, 1.0, texture2D(text, TexCoords).a);\n"
                                          "gl_FragColor = textColor * sampled;\n"
                                          "}\n ");

    FS_Shader_Attrib = glGetAttribLocation(FS_Shader, "vertex");
    FS_Shader_Attrib2 = glGetAttribLocation(FS_Shader, "coords");

    FS_Shader_uni = glGetUniformLocation(FS_Shader, "ScreenSize");
    FS_Shader_uni2 = glGetUniformLocation(FS_Shader, "textColor");

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
    log_info("Renderer clean up end");
}

#endif
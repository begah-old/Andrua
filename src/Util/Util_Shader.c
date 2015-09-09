//
// Created by root on 8/9/15.
//

#include "Util.h"

void Shader_Free(GLuint Shader) {
    glDeleteProgram(Shader);
}

static GLuint LoadShader(const char *src, GLenum Type)
{
    GLuint shader;
    GLint Compiled;

    shader = glCreateShader(Type);

    if(shader == 0)
        return 0;

    glShaderSource(shader, 1, &src, NULL);

    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &Compiled);

    if(!Compiled)
    {
        GLint infoLen = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

        if(infoLen > 1)
        {
            char *infoLog = malloc(sizeof(char) * infoLen);

            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            printf("Opengl error : %s %i : %s", __FILE__, __LINE__, infoLog);
            free(infoLog);
        }

        glDeleteShader(shader);
    }
    return shader;
}

GLuint Shader_loadCustom(char *VS, char *FS)
{
    GLuint vertexShader, fragmentShader, Shader;
    GLint Linked;

    vertexShader = LoadShader(VS, GL_VERTEX_SHADER);
    fragmentShader = LoadShader(FS, GL_FRAGMENT_SHADER);

    Shader = glCreateProgram();

    if(Shader == 0)
        return 0;

    glAttachShader(Shader, vertexShader);
    glAttachShader(Shader, fragmentShader);

    glLinkProgram(Shader);

    glGetProgramiv(Shader, GL_LINK_STATUS, &Linked);

    if(!Linked)
    {
        GLint infoLen = 0;
        glGetProgramiv(Shader, GL_INFO_LOG_LENGTH, &infoLen);

        if(infoLen > 1)
        {
            char *infoLog = malloc(sizeof(char) * infoLen);

            glGetShaderInfoLog(Shader, infoLen, NULL, infoLog);
            printf("OpenGL error linking program : %s %i : %s", __FILE__, __LINE__, infoLog);
            free(infoLog);
            Application_Error();
            return -1;
        }

        glDeleteProgram(Shader);
        return Shader;
    }

    return Shader;
}

#ifndef ANDROID
GLuint Shader_LoadDefault() {

    return Shader_loadCustom("#version 330 core\n"
                                     "layout (location = 0) in vec2 Position;\n"
                                     "layout (location = 1) in vec4 Color;\n"
                                     "uniform vec2 ScreenSize;\n"
                                     "out vec4 oColor;\n"
                                     "void main()\n"
                                     "{\n"
                                     "gl_Position = vec4(Position.x / ScreenSize.x * 2 - 1, Position.y / ScreenSize.y * 2 - 1, 0.0, 1.0);\n"
                                     "oColor = Color;\n"
                                     "} \n",
							 "#version 330 core\n"
                             "in vec4 oColor;\n"
                                     "out vec4 FragColor;\n"
                                     "void main()\n"
                                     "{\n"
                                     "FragColor = oColor;\n"
                                     "} \n");
}

GLuint Shader_LoadImage() {
    return Shader_loadCustom("#version 330 core\n"
                                     "layout (location = 0) in vec2 Position;\n"
                                     "layout (location = 1) in vec2 TexCoord;\n"
                                     "uniform vec2 ScreenSize;\n"
                                     "uniform vec4 ColorBlend;\n"
                                     "out vec2 TexCoord0;\n"
                                     "out vec4 ColorBlendOut;\n"
                                     "void main()\n"
                                     "{\n"
                                     "gl_Position = vec4(Position.x / ScreenSize.x * 2 - 1, Position.y / ScreenSize.y * 2 - 1, 0.0, 1.0);\n"
                                     "TexCoord0 = TexCoord;\n"
                                     "ColorBlendOut = ColorBlend;\n"
                                     "} \n",
                             "#version 330 core\n"
                                     "in vec2 TexCoord0;\n"
                                     "in vec4 ColorBlendOut;\n"
                                     "out vec4 FragColor;\n"
                                     "uniform sampler2D gSampler;\n"
                                     "void main()\n"
                                     "{\n"
                                     "FragColor = ColorBlendOut + texture2D(gSampler, TexCoord0.xy);\n"
                                     "} \n");
}
#else
GLuint Shader_LoadDefault()
{
    return Shader_loadCustom("attribute vec2 vPosition;\n"
                             "attribute vec4 Color;\n"
                             "uniform vec2 ScreenSize;\n"
                             "varying vec4 oColor;\n"
                             "void main() {\n"
                             "  gl_Position = vec4(vPosition.x / ScreenSize.x * 2.0 - 1.0, vPosition.y / ScreenSize.y * 2.0 - 1.0, 0.0, 1.0);\n"
                             "  oColor = Color;"
                             "}\n",
                             "precision mediump float;\n"
                             "varying vec4 oColor;\n"
                             "void main() {\n"
                             "  gl_FragColor = oColor;\n"
                             "}\n");
}

GLuint Shader_LoadImage() {
    return Shader_loadCustom("attribute vec2 a_position;\n"
                             "attribute vec2 a_texCoord;\n"
                             "uniform vec2 ScreenSize;\n"
                             "varying vec2 Texture_Coord;\n"
                             "void main()\n"
                             "{\n"
                             "   gl_Position = vec4(a_position.x / ScreenSize.x * 2.0 - 1.0, a_position.y / ScreenSize.y * 2.0 - 1.0, 0.0, 1.0); \n"
                             "   Texture_Coord = a_texCoord;\n"
                             "}\n" ,
                             "precision mediump float;\n"
                             "varying vec2 Texture_Coord;\n"
                             "uniform sampler2D s_texture;\n"
                             "uniform vec4 ColorBlend;\n"
                             "void main()\n"
                             "{\n"
                             "  gl_FragColor = ColorBlend + texture2D( s_texture, Texture_Coord );\n"
                             "}\n");
}
#endif

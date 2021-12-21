#include "Shader.h"
#include <Windows.h>
void checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success && ShaderDebug)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::string df = infoLog;
            df += "\n -- --------------------------------------------------- -- \n";
            LPWSTR filename = new wchar_t[df.length()];
            MultiByteToWideChar(0, 0, df.c_str(), df.length(), filename, df.length());
            MessageBox(nullptr, filename, L"ERROR::SHADER_COMPILATION_ERROR", MB_ICONERROR);
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success && ShaderDebug)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::string df = infoLog;
            df  += "\n -- --------------------------------------------------- -- \n";
            LPWSTR filename = new wchar_t[df.length()];
            MultiByteToWideChar(0, 0, df.c_str(), df.length(), filename, df.length());
            MessageBox(nullptr, filename, L"ERROR::PROGRAM_LINKING_ERROR", MB_ICONERROR);
        }
    }
}

//shader//
void shader::init(const char* path, GLuint type) {
    this->type = type;
    std::string df = rdFile(path) + "//\0";
    const char* t = df.c_str();
    if (ShaderDebug) {
        LPWSTR filename = new wchar_t[df.length()];
        MultiByteToWideChar(0, 0, t, df.length(), filename, df.length());
        MessageBox(nullptr, filename, L"Shader code", MB_OK);
    }
    id = glCreateShader(type);
    glShaderSource(id, 1, &t, NULL);
    glCompileShader(id);
    checkCompileErrors(id, "");
}
void shader::finit() {
    glDeleteShader(id);
}
//shader//

//program//
void program::setShaderFVG(shader shad, GLuint index) { fvg[index] = shad; }
void program::setShaderFVG(const char* path, GLuint index) { fvg[index].init(path, (index == 0)?GL_VERTEX_SHADER:(index == 1)?GL_FRAGMENT_SHADER:GL_GEOMETRY_SHADER); }
void program::create() {
    id = glCreateProgram();
    glAttachShader(id, fvg[0].id);
    glAttachShader(id, fvg[1].id);
    if (fvg[2].id) glAttachShader(id, fvg[2].id);
    glLinkProgram(id);
    checkCompileErrors(id, "PROGRAM");

    fvg[0].finit();
    fvg[1].finit();
    if (fvg[2].id) fvg[2].finit();
}
void program::use() { glUseProgram(id); }
GLuint program::getUnigorm(const char* name) { return glGetUniformLocation(id, name); }
GLuint program::getAtribut(const char* name) { return glGetAttribLocation( id, name); }
//program//
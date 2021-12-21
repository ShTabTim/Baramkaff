#pragma once
#include <glad/glad.h>
#include"../FileHelper/FileHelper.h"
#define ShaderDebug false

typedef	struct shader {
	GLuint id;
	GLuint type;
	void init(const char* path, GLuint type);
	void finit();
}shader;


typedef struct program {
	GLuint id;
	shader fvg[3];
	void setShaderFVG(shader shad, GLuint index);
	void setShaderFVG(const char* path, GLuint index);
	void create();
	void use();
	GLuint getUnigorm(const char* name);
	GLuint getAtribut(const char* name);
}program;
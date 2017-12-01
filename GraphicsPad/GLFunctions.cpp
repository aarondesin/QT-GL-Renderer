#pragma once

//#include <GL/glew.h>
#include <global.h>
#include <cstring>
#include <iostream>

using namespace std;

void doGLFunction(const char* funcName, void** params)
{
	if (strcmp(funcName, "glActiveTexture") == 0) glActiveTexture((GLenum)params[0]);
	if (strcmp(funcName, "glFramebufferTexture2D") == 0) glFramebufferTexture2D((GLenum)params[0], (GLenum)params[1], (GLenum)params[2], (GLuint)params[3], (GLint)params[4]);
	if (strcmp(funcName, "glGenFramebuffers") == 0) glGenFramebuffers((GLsizei)params[0], (GLuint*)params[1]);


	cout << "Invalid GL function name \"" << funcName << "\"!" << std::endl;
}
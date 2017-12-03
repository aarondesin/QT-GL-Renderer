#pragma once

#include <global.h>
#include <iostream>
using namespace std;

class GLHelper
{
public:
	static const int MAX_TEXTURE_ID = 15;
	static int getNextTextureID()
	{
		int i = nextTextureID;
		nextTextureID++;
		return i;
	}
	static const int MAX_FBO_ID = 3;
	static int getNextFramebufferObjectID()
	{
		int i = nextFramebufferObjectID;
		nextFramebufferObjectID++;
		return i;
	}
	static void GLHelper::checkErrors(string location)
	{
		GLenum err;
		string errMsg = "";
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			switch (err)
			{
			case GL_INVALID_ENUM:
				errMsg = "GL_INVALID_ENUM";
				break;
			case GL_INVALID_VALUE:
				errMsg = "GL_INVALID_VALUE";
				break;
			case GL_INVALID_OPERATION:
				errMsg = "GL_INVALID_OPERATION";
				break;
			case GL_STACK_OVERFLOW:
				errMsg = "GL_STACK_OVERFLOW";
				break;
			case GL_STACK_UNDERFLOW:
				errMsg = "GL_STACK_UNDERFLOW";
				break;
			case GL_OUT_OF_MEMORY:
				errMsg = "GL_OUT_OF_MEMORY";
				break;
			default:
				errMsg = "UNDEFINED";
				break;
			}

			cout << "Error " << err << ": " << errMsg << " after " << location << endl;
			//throw exception();
		}
	}
private:
	static int nextTextureID;
	static int nextFramebufferObjectID;
};
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
		GLenum err = glGetError();
		string errMsg = "";
		switch (err)
		{
		case GL_NO_ERROR:
			return;
		case GL_INVALID_ENUM:
			errMsg = "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			errMsg = "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			errMsg = "GL_INVALID_OPERATION";
			break;
		default:
			errMsg = "UNDEFINED";
			break;
		}
		if (err != GL_NO_ERROR)
		{
			cout << "Error " << err << ": " << errMsg << " after " << location << endl;
			throw exception();
		}
	}
private:
	static int nextTextureID;
	static int nextFramebufferObjectID;
};
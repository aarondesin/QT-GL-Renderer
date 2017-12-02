#pragma once

#include <global.h>
#include <iostream>
using namespace std;

class GLHelper
{
public:
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
};
#include <global.h>

#pragma once

struct ShaderObject
{
public:
	GLenum type;
	ShaderObject(GLenum shaderType, const char* filename) : type(shaderType)
	{
		shaderID = glCreateShader(shaderType);
		string temp = readShaderCode(filename);
		adapter[0] = temp.c_str();
		glShaderSource(shaderID, 1, adapter, 0);
		glCompileShader(shaderID);
	}
	GLuint getShaderID() { return shaderID; }
	bool checkStatus()
	{
		return GLHelper::checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
	}
protected:
	const GLchar* adapter[1];
	GLuint shaderID;
	string readShaderCode(const char* fileName)
	{
		ifstream input(fileName);
		if (!input.good())
		{
			cout << "Failed to load file: " << fileName;
			exit(1);
		}

		return string(istreambuf_iterator<char>(input),
			istreambuf_iterator<char>());
	}
};
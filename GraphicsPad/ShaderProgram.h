#include <global.h>
#include <ShaderObject.h>

#pragma once

struct ShaderProgram
{
public:
	ShaderProgram() : programID(glCreateProgram()) {}
	ShaderProgram(ShaderObject* vertexShader, ShaderObject* fragmentShader) :
		ShaderProgram(vertexShader->getShaderID(), fragmentShader->getShaderID()) {}
	ShaderProgram(GLuint vertShaderID, GLuint fragShaderID) : ShaderProgram()
	{
		vertexShaderID = vertShaderID;
		fragmentShaderID = fragShaderID;
		glAttachShader(programID, vertexShaderID);
		glAttachShader(programID, fragmentShaderID);
		glLinkProgram(programID);
	}
	GLint getProgramID() { return programID; }
	GLint getUniformLocation(const GLchar* uniformName)
	{
		GLint result = glGetUniformLocation(programID, uniformName);
		GLHelper::checkErrors("getUniformLocation().glGetUniformLocation()");
		if (result < 0)
		{
			cout << "Failed to find uniform \"" << uniformName << "\" in program " << programID << "!" << endl;
			//throw exception();
		}
		return result;
	}
	bool checkStatus()
	{
		return GLHelper::checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS) &&
			GLHelper::checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_VALIDATE_STATUS);
	}
protected:
	GLint programID;
	GLuint vertexShaderID;
	GLuint fragmentShaderID;
};
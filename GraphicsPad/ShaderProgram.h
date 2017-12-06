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

		GLHelper::checkErrors("ShaderProgram().glLinkProgram()");

		GLHelper::checkErrors("ShaderProgram constructor");
	}
	GLint getProgramID() { return programID; }
	GLint getUniformLocation(const GLchar* uniformName)
	{
		GLboolean isProgram = glIsProgram(programID);
		if (isProgram == GL_FALSE)
		{
			cout << "Not a program! " << programID << endl;
			throw exception();
		}

		GLint result = glGetUniformLocation(programID, uniformName);
		GLHelper::checkErrors("ShaderProgram.getUniformLocation().glGetUniformLocation()");
		if (result < 0)
		{
			cout << "Failed to find uniform \"" << uniformName << "\" in program " << programID << "!" << endl;
			//throw exception();
		}
		return result;
	}
	void setUniformInt(const GLchar* uniformName, GLint value)
	{
		checkIfActiveProgram();
		glUniform1i(getUniformLocation(uniformName), value);
		GLHelper::checkErrors("ShaderProgram.setUniformInt()", uniformName);
	}
	void setUniformUInt(const GLchar* uniformName, GLuint value)
	{
		checkIfActiveProgram();
		glUniform1ui(getUniformLocation(uniformName), value);
		GLHelper::checkErrors("ShaderProgram.setUniformUInt()", uniformName);
	}
	void setUniformFloat(const GLchar* uniformName, GLfloat value)
	{
		checkIfActiveProgram();
		glUniform1f(getUniformLocation(uniformName), value);
		GLHelper::checkErrors("ShaderProgram.setUniformFloat()", uniformName);
	}
	void setUniformVec3(const GLchar* uniformName, glm::vec3 value)
	{
		checkIfActiveProgram();
		glUniform3f(getUniformLocation(uniformName), value.x, value.y, value.z);
		GLHelper::checkErrors("ShaderProgram.setUniformVec3()", uniformName);
	}
	void setUniformMat4(const GLchar* uniformName, glm::mat4 value)
	{
		checkIfActiveProgram();
		glUniformMatrix4fv(getUniformLocation(uniformName), 1, GL_FALSE, &value[0][0]);
		GLHelper::checkErrors("ShaderProgram.setUniformMat4()", uniformName);
	}
	bool checkStatus()
	{
		return GLHelper::checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS) &&
			GLHelper::checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_VALIDATE_STATUS);
	}
	static void checkIfActiveProgram()
	{
		GLint currentProgram;
		glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
		if (GL_CURRENT_PROGRAM < 0)
		{
			cout << "No current program!" << endl;
		}

		GLboolean isProgram = glIsProgram(currentProgram);
		if (isProgram == GL_FALSE)
		{
			cout << "Not a program! " << currentProgram << endl;
			throw exception();
		}
	}
protected:
	GLint programID;
	GLuint vertexShaderID;
	GLuint fragmentShaderID;
};
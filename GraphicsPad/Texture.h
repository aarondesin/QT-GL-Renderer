#pragma once

#include <iostream>
#include <global.h>
#include <GLHelper.h>

using namespace std;

struct Texture
{
private:
	GLuint textureID;
public:
	Texture() {
		throw exception();
	}
	Texture(int width, int height, GLenum internalFormat, GLenum format, const GLvoid* pixels)
	{
		// Check format
		switch (format)
		{
			case GL_DEPTH_COMPONENT: case GL_DEPTH_STENCIL:
			case GL_RED: case GL_RG: case GL_RGB: case GL_RGBA:
				break;
			default:
				throw exception();
		}

		textureID = GLHelper::getNextTextureID();

		GLenum activeTexture = (GLenum)(GL_TEXTURE0 + textureID);
		if (activeTexture < GL_TEXTURE0 || activeTexture > GL_TEXTURE31)
		{
			cout << "Invalid texture ID! " << activeTexture << endl;
			throw exception();
		}

		glActiveTexture(activeTexture);

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		GLHelper::checkErrors("texture init");

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
			width, height, 0,
			format, GL_UNSIGNED_BYTE, pixels);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		GLHelper::checkErrors("texture set");

		setFilter(GL_LINEAR);
		setWrapMode(GL_REPEAT);

		GLHelper::checkErrors("texture constructor");
	}
	Texture(QImage* image, GLenum internalFormat, GLenum format) : 
		Texture (image->width(), image->height(), internalFormat, format, image->bits())
	{
		if (image == NULL) 
		{
			cout << "NULL REFERENCE: Image is null!" << endl;
		}
	}
	void setFilter(GLenum filter)
	{
		switch (filter) 
		{
		case GL_NEAREST: case GL_LINEAR:
			break;
		default:
			throw exception();
		}

		glActiveTexture(GL_TEXTURE0+textureID);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

		GLHelper::checkErrors("texture.setFilter");
	}
	void setWrapMode(GLenum wrapMode)
	{
		// Check wrap mode
		switch (wrapMode)
		{
		case GL_MIRRORED_REPEAT: case GL_CLAMP_TO_EDGE:
		case GL_CLAMP_TO_BORDER: case GL_REPEAT:
			break;
		default:
			throw exception();
		}

		glActiveTexture(GL_TEXTURE0+textureID);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

		GLHelper::checkErrors("texture.setWrapMode");
	}
	void setTextureID(GLuint texID)
	{
		if (texID > GLHelper::MAX_TEXTURE_ID || texID < -1)
		{
			cout << "Invalid texture ID! " << texID << endl;
			throw exception();
		}

		textureID = texID;
	}
	GLuint getTextureID()
	{
		if (textureID < 0)
		{
			cout << "Invalid texture ID! " << textureID << endl;
			throw exception();
		}

		return textureID;
	}
	~Texture() 
	{
		textureID = -1;
	}
};
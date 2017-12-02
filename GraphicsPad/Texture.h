#pragma once

#include <iostream>
#include <global.h>
#include <GLHelper.h>

using namespace std;

class Texture
{
private:
	static int nextTextureID;
public:
	static const int MAX_TEXTURE_ID = 15;
	static int getNextTextureID() 
	{ 
		int i = nextTextureID; 
		nextTextureID++;
		return i;
	}
	Texture() {
		throw exception();
	}
	Texture(string texName, int width, int height, GLenum internalFormat, GLenum format, const GLvoid* pixels)
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

		
		glGenTextures(1, textureID);
		glBindTexture(GL_TEXTURE_2D, *textureID);

		if (texName.length() <= 0) throw exception();
		name = texName;

		textureID = new GLuint(getNextTextureID());

		GLenum activeTexture = (GLenum)(GL_TEXTURE0 + textureID);
		if (activeTexture < GL_TEXTURE0 || activeTexture > GL_TEXTURE31)
		{
			cout << "Invalid texture ID! " << activeTexture << endl;
			throw exception();
		}

		glActiveTexture(activeTexture);

		GLHelper::checkErrors("texture init");

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
			width, height, 0,
			format, GL_UNSIGNED_BYTE, pixels);

		GLHelper::checkErrors("texture set");

		setFilter(GL_LINEAR);
		setWrapMode(GL_CLAMP_TO_EDGE);

		GLHelper::checkErrors("texture constructor");
	}
	Texture(string texName, QImage* image, GLenum internalFormat, GLenum format) : 
		Texture (texName, image->width(), image->height(), internalFormat, format, image->bits())
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

		glActiveTexture(GL_TEXTURE0+*textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

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

		glActiveTexture(GL_TEXTURE0+*textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

		GLHelper::checkErrors("texture.setWrapMode");
	}
	void setTextureID(GLuint texID)
	{
		if (texID > MAX_TEXTURE_ID || texID < -1)
		{
			cout << "Invalid texture ID! " << texID << endl;
			throw exception();
		}

		*textureID = texID;
	}
	GLint getTextureID()
	{
		if (*textureID > MAX_TEXTURE_ID || textureID < 0)
		{
			cout << "Invalid texture ID! " << textureID << endl;
			throw exception();
		}

		return *textureID;
	}
	~Texture() 
	{
		delete textureID;
	}
protected:
	string name;
	GLuint* textureID;
};
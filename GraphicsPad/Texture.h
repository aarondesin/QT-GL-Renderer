#include <QtGui/qimage.h>

#pragma once

struct Texture
{
	Texture() : textureID(-1) {}
	Texture(GLuint id, int width, int height, const GLvoid* pixels)
	{
		textureID = id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glActiveTexture(id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
			width, height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		setFilter(GL_LINEAR);
		setWrapMode(GL_CLAMP_TO_EDGE);
	}
	Texture(GLuint id, QImage image) : 
		Texture (id, image.width(), image.height(), image.bits()){}
	void setFilter(GLenum filter)
	{
		glActiveTexture(textureID);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	}
	void setWrapMode(GLenum wrapMode)
	{
		glActiveTexture(textureID);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	}
	GLuint textureID;
};
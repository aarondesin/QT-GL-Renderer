#include <QtGui/qimage.h>

#pragma once

struct Texture
{
	Texture() : glTextureID(-1), textureID(-1) {}
	Texture(GLuint id, GLenum glID, QImage image) 
	{
		textureID = id;
		glTextureID = glID;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
			image.width(), image.height(), 0,
			GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
		setFilter(GL_LINEAR);
		setWrapMode(GL_CLAMP_TO_EDGE);
	}
	void setFilter(GLenum filter)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	}
	void setWrapMode(GLenum wrapMode)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	}
	GLenum glTextureID;
	GLuint textureID;
};
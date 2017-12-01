#pragma once

#include <global.h>

class Texture
{
public:
	static int nextTextureID;
	static int getNextTextureID() { return nextTextureID++; }
	Texture(int width, int height, GLenum format, const GLvoid* pixels)
	{
		
		textureID = nextTextureID++;
		glActiveTexture(GL_TEXTURE0 + textureID);

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format,
			width, height, 0,
			format, GL_UNSIGNED_BYTE, pixels);
		setFilter(GL_LINEAR);
		setWrapMode(GL_CLAMP_TO_EDGE);
		
	}
	Texture(QImage* image) : 
		Texture (image->width(), image->height(), GL_RGBA, image->bits()){}
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
	GLuint getTextureID() { return textureID; }
protected:
	GLuint textureID;
};
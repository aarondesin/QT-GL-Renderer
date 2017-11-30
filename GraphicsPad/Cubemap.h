#pragma once

#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3
#define BACK 4
#define FORWARD 5

#define GL_TEXTURE_WRAP_R 0x8072

#define GL_TEXTURE_CUBE_MAP 0x8513
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X 0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y 0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z 0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x851A

//extern PFNGLACTIVETEXTUREPROC glActiveTexture;

struct Cubemap
{
	Cubemap() : cubemapID(-1) {}
	Cubemap(GLuint id, QImage* images)
	{
		cubemapID = id;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		glActiveTexture(cubemapID);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, images[LEFT].width(), 
			images[LEFT].height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, images[LEFT].bits());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, images[RIGHT].width(),
			images[RIGHT].height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, images[RIGHT].bits());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, images[UP].width(),
			images[UP].height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, images[UP].bits());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, images[DOWN].width(), 
			images[DOWN].height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, images[DOWN].bits());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, images[BACK].width(),
			images[BACK].height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, images[BACK].bits());
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, images[FORWARD].width(),
			images[FORWARD].height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, images[FORWARD].bits());
		setFilter(GL_LINEAR);
		setWrapMode(GL_CLAMP_TO_EDGE);
	}
	void setFilter(GLenum filter)
	{
		glActiveTexture(cubemapID);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filter);
	}
	void setWrapMode(GLenum wrapMode)
	{
		glActiveTexture(cubemapID);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapMode);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapMode);
	}
	GLuint cubemapID;
};
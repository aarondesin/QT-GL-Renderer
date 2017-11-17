#include <Texture.h>

using namespace std;

#pragma once

struct Framebuffer
{
	Framebuffer() : framebufferObjectID(-1), renderTextureID(-1) {}
	Framebuffer(GLuint renderTexID, int width, int height)
	{
		Texture renderTexture = Texture(renderTextureID, width, height, NULL);
		renderTextureID = renderTexID;

		glGenFramebuffers(1, &framebufferObjectID);
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferObjectID);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTextureID, 0);

		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
			cout << "Framebuffer incomplete!" << endl;
		else cout << "Framebuffer complete." << endl;
	}
	GLuint framebufferObjectID;
	GLuint renderTextureID;
	GLenum status;
};
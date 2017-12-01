#pragma once

#include <global.h>
#include <iostream>
#include <Texture.h>

using namespace std;

class Framebuffer
{
public:
	static int nextFramebufferObjectID;
	Framebuffer() : framebufferObjectID(-1), renderTextureID(-1) {}
	Framebuffer(bool useColor, bool useDepth, int width, int height)
	{
		if (!useDepth && !useColor)
		{
			cout << "Framebuffer must use either color or depth attachments!" << endl;
			throw std::exception();
		}

		framebufferObjectID = nextFramebufferObjectID++;

		if (useColor)
		{
			Texture renderTexture = Texture(width, height, GL_RGB, NULL);
			renderTextureID = renderTexture.getTextureID();

			glGenFramebuffers(1, &framebufferObjectID);
			glBindFramebuffer(GL_FRAMEBUFFER, framebufferObjectID);
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTextureID, 0);
		}

		if (useDepth)
		{
			Texture depthTexture = Texture(width, height, GL_RGB, NULL);

		}

		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	}
	GLuint framebufferObjectID;
	GLuint renderTextureID;
	GLenum status;
};

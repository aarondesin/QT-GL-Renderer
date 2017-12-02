#pragma once

#include <global.h>
#include <iostream>
#include <Texture.h>

using namespace std;

class Framebuffer
{
protected:
	Texture* renderTexture;
	Texture* depthTexture;
	GLuint framebufferObjectID;
	GLenum status;
public:
	Framebuffer() { throw exception(); }
	static int nextFramebufferObjectID;
	Framebuffer(string name, bool useColor, bool useDepth, int width, int height)
	{
		if (!useDepth && !useColor)
		{
			cout << "Framebuffer must use either color or depth attachments!" << endl;
			throw std::exception();
		}

		framebufferObjectID = nextFramebufferObjectID++;

		glGenFramebuffers(1, &framebufferObjectID);
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferObjectID);

		if (useColor)
		{
			renderTexture = &Texture(name, width, height, GL_RGB, GL_RGB, NULL);
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture->getTextureID(), 0);
		}

		if (useDepth)
		{
			depthTexture = &Texture(name, width, height, GL_DEPTH_COMPONENT16, GL_R, NULL);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture->getTextureID(), 0);
		}

		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	}
	~Framebuffer() 
	{ 
		delete renderTexture;
		delete depthTexture;
		framebufferObjectID = 0;
		status = 0;
	}
	GLenum getStatus() { return status; }
	GLint getFramebufferObjectID() { return framebufferObjectID; }
};

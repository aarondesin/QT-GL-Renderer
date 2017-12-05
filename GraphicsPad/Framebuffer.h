#pragma once

#include <global.h>
#include <iostream>
#include <Texture.h>
#include <GLHelper.h>

using namespace std;

struct Framebuffer
{
protected:
	Texture* renderTexture = NULL;
	Texture* depthTexture = NULL;
	GLuint framebufferObjectID;
	GLenum status;
	GLint width, height;
public:
	Framebuffer() { throw exception(); }
	Framebuffer(string name, bool useColor, bool useDepth, int w, int h)
	{
		width = w;
		height = h;

		if (!useDepth && !useColor)
		{
			cout << "Framebuffer must use either color or depth attachments!" << endl;
			throw std::exception();
		}

		framebufferObjectID = GLHelper::getNextFramebufferObjectID();

		glGenFramebuffers(1, &framebufferObjectID);
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferObjectID);

		if (useColor)
		{
			renderTexture = new Texture(name, width, height, GL_RGB, GL_RGB, NULL);
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glActiveTexture(GL_TEXTURE0 + renderTexture->getTextureID());
			glBindTexture(GL_TEXTURE_2D, renderTexture->getTextureID());
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture->getTextureID(), 0);
			//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTexture->getTextureID(), 0);
		}

		if (useDepth)
		{
			depthTexture = new Texture(name, width, height, GL_DEPTH_COMPONENT16, GL_R, NULL);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture->getTextureID(), 0);
		}

		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		GLHelper::checkErrors("Framebuffer constructor");
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
	GLint getWidth() { return width; }
	GLint getHeight() { return height; }
	GLint getRenderTextureID() { return renderTexture->getTextureID(); }
	GLint getDepthTextureID() { return depthTexture->getTextureID(); }
};

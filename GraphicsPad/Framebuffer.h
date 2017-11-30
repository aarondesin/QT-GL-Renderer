#pragma once



#include <iostream>

#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_FRAMEBUFFER 0x8D40

//extern PFNGLBINDBUFFERPROC glBindFramebuffer;
//extern PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
extern PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers(GLsizei, GLuint*);
//#define glGenFramebuffer GLEW_GET_FUN(__glewGenFramebuffers)

//#define glBindFramebuffer GLEW_GET_FUN(__glewBindFramebuffer)

struct Framebuffer
{
public:
	Framebuffer() : framebufferObjectID(-1), renderTextureID(-1) {}
	Framebuffer(GLuint renderTexID, int width, int height)
	{
		Texture renderTexture = Texture(renderTextureID, width, height, GL_RGB, NULL);
		renderTextureID = renderTexID;

		glGenFramebuffers(1, &framebufferObjectID);
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferObjectID);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTextureID, NULL);

		//status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		/*if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			string err;
			switch (status)
			{
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				err = "Incomplete attachment.";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				err = "Incomplete draw buffer.";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				err = "Missing attachment.";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
				err = "Layer targets.";
				break;
			default:
				err = "Unknown";
				break;
			}
			std::cout << "Framebuffer incomplete! " << err << endl;
		}
		else std::cout << "Framebuffer complete." << endl;*/
	}
	GLuint framebufferObjectID;
	GLuint renderTextureID;
	GLenum status;
};

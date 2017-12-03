#pragma once

#ifndef MY_GL_WINDOW
#define MY_GL_WINDOW

#include <Camera.h>
#include <string>
#include "Texture.h"
#include "Cubemap.h"
#include "Framebuffer.h"
#include "ShapeData.h"
#include "Renderable.h"
using namespace std;

class MyGLWindow : public QGLWidget
{
protected:
	void initializeGL();
	void paintGL();
	void MyGLWindow::mouseMoveEvent(QMouseEvent*);
	void MyGLWindow::keyPressEvent(QKeyEvent* e);
	void MyGLWindow::initScene();
	void MyGLWindow::updateUniforms();
	void MyGLWindow::spawnRenderable();
	void MyGLWindow::drawSkybox(Camera* cam, bool flipped);
	void MyGLWindow::draw(Camera* cam, bool flipped);
	void MyGLWindow::installShaders();
	QImage* MyGLWindow::makeImage(string filename);
	Texture* MyGLWindow::makeTexture(string filename);
	Cubemap* MyGLWindow::makeCubemap(string filename);
	Framebuffer* MyGLWindow::makeFramebuffer(string filename, bool useColor, bool useDepth, int width, int height);
	void MyGLWindow::addTexture(string name, Texture* texture);
	void MyGLWindow::addGeometry(string name, ShapeData* geometry);
	void MyGLWindow::addMaterial(string name, Material* material);
	void MyGLWindow::initTextures();
	void MyGLWindow::initMaterials();
	void MyGLWindow::initGeometries();
};

#endif
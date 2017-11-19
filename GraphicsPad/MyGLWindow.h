#ifndef MY_GL_WINDOW
#define MY_GL_WINDOW
#include <QtOpenGL\qglwidget>
#include <QtGui\qkeyevent>
#include <Camera.h>

class MyGLWindow : public QGLWidget
{
protected:
	void initializeGL();
	void paintGL();
	void MyGLWindow::mouseMoveEvent(QMouseEvent*);
	void MyGLWindow::keyPressEvent(QKeyEvent* e);
	void MyGLWindow::initScene();
	//void MyGLWindow::sendDataToOpenGL();
	void MyGLWindow::updateUniforms();
	//void MyGLWindow::setupVertexArrays();
	void MyGLWindow::spawnRenderable();
	void MyGLWindow::draw(Camera cam, bool flipped);
public:
};

#endif
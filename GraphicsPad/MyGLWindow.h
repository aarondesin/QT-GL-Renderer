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
	void MyGLWindow::sendDataToOpenGL();
	void MyGLWindow::updateScene();
	void MyGLWindow::setupVertexArrays();
	void MyGLWindow::addSphere();
	void MyGLWindow::draw(Camera cam, bool flipped);
public:
};

#endif
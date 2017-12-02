#pragma once
#include<global.h>
#include <MyGLWindow.h>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <iostream>
#include <fstream>
#include <Vertex.h>
#include <ShapeData.h>
#include <ShapeGenerator.h>
#include "Camera.h"
#include "Light.h"
#include "Scene.h"
#include "Renderable.h"
#include "Texture.h"
#include "Cubemap.h"
#include "Framebuffer.h"
#include "Material.h"
#include <map>
#include <list>
#include <OBJLoader.h>
using namespace std;

#define VERTEX_BYTE_SIZE sizeof(float) * 15

const float RANDOM_PLACEMENT_OFFSET = 40.0f;

GLint programID = -1;

Scene scene("Main");
Scene* activeScene;

Framebuffer *activeFramebuffer;

// All images
map<string, QImage> images;

// All textures
const int MAX_TEXTURES = 10;
map<string, Texture> textures;

// All cubemaps
const int MAX_CUBEMAPS = 3;
map<string, Cubemap> cubemaps;

// All framebuffers
const int MAX_FRAMEBUFFERS = 2;
map<string, Framebuffer> framebuffers;

// All geometries
map<string, ShapeData> geometries;

// All materials
map<string, Material> materials;

// Uniform locations
GLint cameraPosUniformLoc;
GLint mvpUniformLocation;
GLint modelMatUniformLocation;

GLint ambientLightUniformLoc;
GLint lightPosUniformLoc;
GLint diffuseLightUniformLoc;
GLint specularColorUniformLoc;
GLint specularPowerUniformLoc;

GLint diffuseTextureUniformLoc;
GLint diffuseStrengthUniformLoc;
GLint normalStrengthUniformLoc;
GLint normalMapUniformLoc;
GLint cubemapUniformLoc;
GLint reflectivityUniformLoc;
GLint emissionStrengthUniformLoc;
GLint indexOfRefractionUniformLoc;
GLint fresnelValueUniformLoc;

GLint getUniformLocation(const GLchar* uniformName)
{
	GLint result = glGetUniformLocation(programID, uniformName);
	if (result < 0)
	{
		cout << "Failed to find uniform \"" << uniformName << "\"!" << endl;
	}
	return result;
}

QImage MyGLWindow::makeImage(string filename)
{
	GLHelper::checkErrors("makeImage -- before function entry");

	QString filenameAsQString = QString::fromStdString(filename + ".png");
	GLHelper::checkErrors("makeImage -- filename processing");

	QImage loadedImage = QImage(filenameAsQString, "png");
	GLHelper::checkErrors("makeImage -- construction");

	if (loadedImage.isNull())
	{
		cout << "Failed to load image \"" << filenameAsQString.constData() << "\"!" << endl;
		throw std::exception();
	}

	QImage image = QGLWidget::convertToGLFormat(loadedImage);
	GLHelper::checkErrors("makeImage -- conversion");

	std::pair<string, QImage> pair = 
		std::pair<string, QImage>(filename, image);

	images.emplace(pair);
	cout << "--IMAGE: " << filename << "--" << endl;
	GLHelper::checkErrors("makeImage");
	return image;
}

Texture MyGLWindow::makeTexture(string filename)
{
	GLHelper::checkErrors("makeTexture -- before function entry");

	QImage image = makeImage(filename);
	GLHelper::checkErrors("makeTextue -- image creation");

	if (image.isNull())
	{
		cout << "NULL REFERENCE: Image is null!" << endl;
		throw exception();
	}

	Texture texture = Texture(filename, image.width(), image.height(), GL_RGBA, GL_RGBA, image.bits());
	GLHelper::checkErrors("makeTexture - texture construction");


	std::pair<string, Texture> pair = (std::pair<string, Texture>(filename, texture));
	textures.emplace(pair);
	cout << "--TEXTURE (" << texture.getTextureID() << "): " << filename << "--" << endl;
	GLHelper::checkErrors("makeTexture - function complete");

	return texture;
}

Cubemap MyGLWindow::makeCubemap(string filename)
{
	QImage left = makeImage(filename + "_Left");
	QImage right = makeImage(filename + "_Right");
	QImage up = makeImage(filename + "_Up");
	QImage down = makeImage(filename + "_Down");
	QImage back = makeImage(filename + "_Back");
	QImage forward = makeImage(filename + "_Forward");
	QImage images[] = { left, right, up, down, back, forward };
	Cubemap cubemap = Cubemap((QImage*)images);
	cubemaps.emplace(std::pair<string, Cubemap>(filename, cubemap));
	cout << "--CUBEMAP (" << cubemap.cubemapID << "): " << filename << "--" << endl;
	GLHelper::checkErrors("makeCubemap");
	return cubemap;
}

Framebuffer MyGLWindow::makeFramebuffer(string name, bool useColor, bool useDepth, int width, int height)
{
	cout << "Making Framebuffer \"" << name << "\"...";

	Framebuffer framebuffer = Framebuffer(name, useColor, useDepth, width, height);

	if (framebuffer.getStatus() != GL_FRAMEBUFFER_COMPLETE)
	{
		string err;
		switch (framebuffer.getStatus())
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			err = "Incomplete attachment.";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			err = "Incomplete draw buffer.";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			err = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			err = "Layer targets.";
			break;
		default:
			err = "Unknown";
			break;
		}
		std::cout << endl  << "Framebuffer incomplete! " << err << endl;
		throw std::exception();
	}

	framebuffers.emplace(std::pair<string, Framebuffer>(name, framebuffer));

	//cout << "--FRAMEBUFFER (" << framebuffer.framebufferObjectID << "): " << name << "--" << endl;
	cout << "done." << endl;
	GLHelper::checkErrors("makeFramebuffer");
	return framebuffer;
}

void MyGLWindow::addGeometry(string name, ShapeData geometry)
{
	glGenBuffers(1, &geometry.vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, geometry.vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, geometry.vertexBufferSize(),
		geometry.vertices, GL_STATIC_DRAW);

	GLHelper::checkErrors("addGeometry - vertex buffer creation");

	glGenBuffers(1, &geometry.indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry.indexBufferID);
	glBufferData(GL_ARRAY_BUFFER, geometry.indexBufferSize(),
		geometry.indices, GL_STATIC_DRAW);

	GLHelper::checkErrors("addGeometry - index buffer creation");

	glGenVertexArrays(1, &geometry.vertexArrayObjectID);
	glBindVertexArray(geometry.vertexArrayObjectID);
	glEnableVertexAttribArray(0); // Position
	glEnableVertexAttribArray(1); // Color
	glEnableVertexAttribArray(2); // Normal
	glEnableVertexAttribArray(3); // UV
	glEnableVertexAttribArray(4); // Tangent
	glBindBuffer(GL_ARRAY_BUFFER, geometry.vertexBufferID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (char*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (char*)(sizeof(float) * 7));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (char*)(sizeof(float) * 10));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (char*)(sizeof(float) * 12));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry.indexBufferID);

	GLHelper::checkErrors("addGeometry - VAO creation");

	geometries.emplace(std::pair<string, ShapeData>(name, geometry));
	cout << "Added geometry: " << name << endl;
	GLHelper::checkErrors("addGeometry");
}

void setActiveDiffuseTexture(Texture diffuse)
{
	GLint texID = diffuse.getTextureID();

	diffuseTextureUniformLoc = getUniformLocation("diffuseTexture");
	glUniform1i(diffuseTextureUniformLoc, texID);

	GLHelper::checkErrors("setActiveDiffuseTexture");
}

void setActiveDiffuseTexture(string filename)
{
	Texture diffuse = textures.at(filename);
	setActiveDiffuseTexture(diffuse);
}

void setActiveNormalMap(Texture normalMap)
{
	GLint normalMapID = normalMap.getTextureID();

	normalMapUniformLoc = getUniformLocation("normalMap");
	glUniform1i(normalMapUniformLoc, normalMapID);

	GLHelper::checkErrors("setActiveNormalMap");
}

void setActiveNormalMap(string filename)
{
	Texture normalMap = textures.at(filename);
	setActiveNormalMap(normalMap);
}

void setActiveCubemap(Cubemap cubemap)
{
	GLint cubemapID = cubemap.cubemapID;

	cubemapUniformLoc = getUniformLocation("cubemap");
	glUniform1i(cubemapUniformLoc, cubemap.cubemapID);

	GLHelper::checkErrors("setActiveCubemap");
}

void setActiveCubemap(string filename)
{
	Cubemap cubemap = cubemaps.at(filename);
	setActiveCubemap(cubemap);
}

void setActiveFramebuffer(Framebuffer framebuffer)
{
	activeFramebuffer = &framebuffer;
}

void setActiveFramebuffer(string name)
{
	Framebuffer framebuffer = framebuffers.at(name);
	setActiveFramebuffer(framebuffer);
}

void setActiveMaterial(Material material)
{
	glUniform1i(diffuseTextureUniformLoc, material.diffuse.getTextureID());
	glUniform1f(diffuseStrengthUniformLoc, material.diffuseStrength);
	glUniform1i(normalMapUniformLoc, material.normal.getTextureID());
	glUniform1f(normalStrengthUniformLoc, material.normalStrength);
	glUniform1f(specularPowerUniformLoc, material.specularPower);
	glUniform1f(emissionStrengthUniformLoc, material.emissionStrength);
	glUniform1f(reflectivityUniformLoc, material.reflectivity);
	glUniform1f(indexOfRefractionUniformLoc, material.indexOfRefraction);
	glUniform1f(fresnelValueUniformLoc, material.fresnelValue);
}

void MyGLWindow::updateUniforms()
{
	// Update ambient lighting strength
	glUniform3f(ambientLightUniformLoc,
		activeScene->ambientLight.r,
		activeScene->ambientLight.g,
		activeScene->ambientLight.b);

	// Update light position
	glUniform3f(lightPosUniformLoc,
		activeScene->activeLight->position.x,
		activeScene->activeLight->position.y,
		activeScene->activeLight->position.z);

	// Update diffuse lighting
	glUniform3f(diffuseLightUniformLoc,
		activeScene->activeLight->color.r,
		activeScene->activeLight->color.g,
		activeScene->activeLight->color.b);

	// Update specular lighting
	glUniform3f(specularColorUniformLoc,
		activeScene->activeLight->color.r,
		activeScene->activeLight->color.g,
		activeScene->activeLight->color.b);

	// Update camera position
	glUniform3f(cameraPosUniformLoc,
		activeScene->getActiveCamera()->getPosition().x,
		activeScene->getActiveCamera()->getPosition().y,
		activeScene->getActiveCamera()->getPosition().z);
}

void MyGLWindow::mouseMoveEvent(QMouseEvent* e)
{
	activeScene->getActiveCamera()->mouseUpdate(glm::vec2(e->x(), e->y()));
	repaint();
}

void MyGLWindow::spawnRenderable()
{
	int randomType = rand() % 2;
	Renderable renderable;
	switch (randomType)
	{
	case 0:
		renderable.geometry = geometries.at("sphere");
		break;
	case 1:
		renderable.geometry = geometries.at("cube");
		break;
	}

	renderable.material = materials.at("standard");

	glm::vec3 randPosition = glm::vec3(
		rand() / (float)RAND_MAX - 0.5f, 
		rand() / (float)RAND_MAX - 0.5f, 
		rand() / (float)RAND_MAX - 0.5f) 
		* RANDOM_PLACEMENT_OFFSET;

	renderable.position = randPosition;

	float scaleValue = rand() / RAND_MAX * 5 + 2.5f;
	renderable.scale = glm::vec3(scaleValue, scaleValue, scaleValue);

	renderable.rotation = glm::vec3(
		rand() / RAND_MAX * 360.0f, 
		rand() / RAND_MAX * 360.0f, 
		rand() / RAND_MAX * 360.0f);

	activeScene->addRenderable(renderable);
}

void MyGLWindow::keyPressEvent(QKeyEvent* e)
{
	switch (e->key())
	{
		case Qt::Key::Key_W:
			activeScene->getActiveCamera()->moveForward();
			break;
		case Qt::Key::Key_S:
			activeScene->getActiveCamera()->moveBackward();
			break;
		case Qt::Key::Key_A:
			activeScene->getActiveCamera()->strafeLeft();
			break;
		case Qt::Key::Key_D:
			activeScene->getActiveCamera()->strafeRight();
			break;
		case Qt::Key::Key_Q:
			activeScene->getActiveCamera()->moveDown();
			break;
		case Qt::Key::Key_E:
			activeScene->getActiveCamera()->moveUp();
			break;
		case Qt::Key::Key_Space:
			spawnRenderable();
			break;
		case Qt::Key::Key_1:
			activeScene->activeLight->moveBackward();
			break;
		case Qt::Key::Key_2:
			activeScene->activeLight->moveForward();
			break;
		case Qt::Key::Key_Left:
			activeScene->activeLight->moveLeft();
			break;
		case Qt::Key::Key_Right:
			activeScene->activeLight->moveRight();
			break;
		case Qt::Key::Key_Up:
			activeScene->activeLight->moveUp();
			break;
		case Qt::Key::Key_Down:
			activeScene->activeLight->moveDown();
			break;
		case Qt::Key::Key_R:
			activeScene->switchCamera();
			break;
	}
	MyGLWindow::updateGL();
}

void MyGLWindow::initMaterials()
{
	Material lightMaterial;
	lightMaterial.diffuseStrength = 0.0f;
	lightMaterial.emissionStrength = 1.0f;
	lightMaterial.normalStrength = 0.0f;

	Material standardMaterial;
	standardMaterial.diffuse = textures.at("tri");
	standardMaterial.diffuseStrength = 1.0f;
	standardMaterial.normal = textures.at("ShapesNormal");
	standardMaterial.normalStrength = 1.0f;
	standardMaterial.specularPower = 100.0f;
	materials.emplace(std::pair<string, Material>("standard", standardMaterial));

	Material flatMaterial;
	flatMaterial.diffuseStrength = 0.0f;
	flatMaterial.normalStrength = 0.0f;
	materials.emplace(std::pair<string, Material>("flat", flatMaterial));

	Material reflectiveMaterial;
	reflectiveMaterial.diffuseStrength = 0.0f;
	reflectiveMaterial.normal = textures.at("ShapesNormal");
	reflectiveMaterial.normalStrength = 1.0f;
	reflectiveMaterial.reflectivity = 1.0f;
	materials.emplace(std::pair<string, Material>("reflective", reflectiveMaterial));

	Material f2Material;
	f2Material.diffuse = textures.at("Textures/BristolF2_Albedo");
	f2Material.diffuseStrength = 1.0f;
	f2Material.normal = textures.at("Textures/BristolF2_Normal");
	f2Material.normalStrength = 1.0f;
	f2Material.reflectivity = 1.0f;
	materials.emplace(std::pair<string, Material>("f2", f2Material));
}

void MyGLWindow::initGeometries()
{
	ShapeData cube = ShapeGenerator::makeCube();
	MyGLWindow::addGeometry("cube", cube);

	ShapeData sphere = ShapeGenerator::makeSphere();
	MyGLWindow::addGeometry("sphere", sphere);

	ShapeData plane = ShapeGenerator::makePlane(20U);
	MyGLWindow::addGeometry("plane", plane);

	ShapeData model = OBJLoader::loadOBJFile("Models/BristolF2.obj");
	MyGLWindow::addGeometry("f2", model);
}

void MyGLWindow::initScene()
{
	activeScene = &scene;
	activeScene->ambientLight = glm::vec3(1.0f, 1.0f, 1.0f);
	activeScene->activeLight->position = glm::vec3(0.0f, 5.0f, 0.0f);

	Renderable skybox;
	skybox.geometry = geometries.at("cube");
	skybox.scale = glm::vec3(100.0f, 100.0f, 100.0f);
	activeScene->skybox = skybox;

	Renderable cube;
	cube.geometry = geometries.at("cube");
	cube.material = materials.at("standard");
	cube.position = glm::vec3(0.0f, 2.5f, 0.0f);
	cube.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	cube.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	activeScene->addRenderable(cube);

	Renderable sphere;
	sphere.geometry = geometries.at("sphere");
	sphere.material = materials.at("flat");
	sphere.position = glm::vec3(3.0f, 2.0f, 1.0f);
	sphere.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	sphere.scale = glm::vec3(2.0f, 2.0f, 2.0f);
	activeScene->addRenderable(sphere);

	Renderable plane;
	plane.geometry = geometries.at("plane");
	plane.material = materials.at("reflective");
	plane.position = glm::vec3(0.0f, 0.0f, 0.0f);
	plane.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	plane.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	activeScene->addRenderable(plane);

	Renderable f2;
	f2.geometry = geometries.at("f2");
	f2.material = materials.at("f2");
	f2.position = glm::vec3(0.0f, 1.0f, 0.0f);
	f2.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	f2.scale = glm::vec3(0.1f, 0.1f, 0.1f);
	activeScene->addRenderable(f2);
}

string readShaderCode(const char* fileName)
{
	ifstream input(fileName);
	if (!input.good())
	{
		cout << "Failed to load file: " << fileName;
		exit(1);
	}

	return string(istreambuf_iterator<char>(input),
		istreambuf_iterator<char>());
}

bool checkStatus(GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
	GLenum statusType)
{
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		cout << buffer << endl;
		delete[] buffer;
		return false;
	}
	return true;
}

bool checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programID)
{
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

void MyGLWindow::installShaders()
{
	// Create shader objects
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Adapter is used to package the shader code strings in the format
	// that OpenGL wants
	const GLchar* adapter[1];

	// Assign shaders
	string temp = readShaderCode("VertexShader.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);

	temp = readShaderCode("FragmentShader.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	// If either of the shaders is broken, quit out
	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return;

	// Init GL program and link shaders to it
	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	if (!checkProgramStatus(programID))
		return;

	glUseProgram(programID);

	GLHelper::checkErrors("installShaders");
}

void MyGLWindow::initializeGL()
{
	// Inits OpenGL stuff
	glewInit();
	GLHelper::checkErrors("initializeGL -- glewInit");

	installShaders();

	glEnable(GL_DEPTH_TEST);
	initGeometries();

	// Diffuse texture
	Texture tri = makeTexture("tri");
	setActiveDiffuseTexture("tri");

	GLHelper::checkErrors("initializeGL -- load default diffuse texture");

	// Normal map
	Texture normal = makeTexture("ShapesNormal");
	setActiveNormalMap("ShapesNormal");

	GLHelper::checkErrors("initializeGL -- load default normal texture");

	// Cubemap
	Cubemap cubemap = makeCubemap("Skybox_Dawn");
	setActiveCubemap("Skybox_Dawn");

	GLHelper::checkErrors("initializeGL -- load default skybox texture");

	// F2
	makeTexture("Textures/BristolF2_Albedo");
	makeTexture("Textures/BristolF2_AO");
	makeTexture("Textures/BristolF2_MetallicSmoothness");
	makeTexture("Textures/BristolF2_Normal");

	GLHelper::checkErrors("initializeGL -- load resources");

	initMaterials();
	initScene();
	installShaders();

	// Get uniforms
	modelMatUniformLocation     = getUniformLocation("modelMatrix");
	mvpUniformLocation          = getUniformLocation("mvp");
	cameraPosUniformLoc         = getUniformLocation("camPos");

	ambientLightUniformLoc      = getUniformLocation("ambientLight");
	lightPosUniformLoc          = getUniformLocation("lightPos");
	diffuseLightUniformLoc      = getUniformLocation("diffuseColor");
	specularColorUniformLoc     = getUniformLocation("specularColor");
	specularPowerUniformLoc     = getUniformLocation("specularPower");

	diffuseTextureUniformLoc    = getUniformLocation("diffuseTexture");
	diffuseStrengthUniformLoc   = getUniformLocation("diffuseStrength");
	normalMapUniformLoc         = getUniformLocation("normalMap");
	cubemapUniformLoc           = getUniformLocation("cubemap");
	normalStrengthUniformLoc    = getUniformLocation("normalStrength");
	reflectivityUniformLoc      = getUniformLocation("reflectivity");
	indexOfRefractionUniformLoc = getUniformLocation("indexOfRefraction");
	fresnelValueUniformLoc      = getUniformLocation("fresnelValue");

	GLHelper::checkErrors("initializeGL -- find uniforms");

	// Render texture
	Framebuffer framebuffer = makeFramebuffer("RenderTexture", true, false, 1024, 1024);
	setActiveFramebuffer(framebuffer);

	// Shadow map
	Framebuffer shadowMap = makeFramebuffer("ShadowMap", false, true, 1024, 1024);

	// Clear to black
	glClearColor(0, 0, 0, 1);

	GLHelper::checkErrors("initializeGL");
}

void MyGLWindow::drawSkybox(Camera cam, bool flipped)
{
	glm::vec3 camPosBackup = activeScene->getActiveCamera()->getPosition();
	GLuint vaoID = activeScene->skybox.geometry.vertexArrayObjectID;
	glBindVertexArray(vaoID);

	activeScene->getActiveCamera()->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	float scaleVal = flipped ? 1.0f : -1.0f;
	glm::mat4 camMat = cam.getWorldToViewMatrix() * glm::scale(glm::vec3(1.0f, scaleVal, 1.0f));
	glm::mat4 modelMat = glm::mat4();



	glDrawElements(GL_TRIANGLES, activeScene->skybox.geometry.numIndices, GL_UNSIGNED_SHORT, 0);

	glClear(GL_DEPTH_BUFFER_BIT);

	activeScene->getActiveCamera()->setPosition(camPosBackup);

	GLHelper::checkErrors("drawSkybox");
}

void MyGLWindow::draw(Camera cam, bool flipped)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	updateUniforms();

	// View to projection
	glm::mat4 projMat = glm::perspective(
		activeScene->getActiveCamera()->fov,
		((float)width() / (float)height()),
		activeScene->getActiveCamera()->clipNear,
		activeScene->getActiveCamera()->clipFar);

	drawSkybox(cam, false);

	float scaleVal = flipped ? -1.0f : 1.0f;
	glm::mat4 camMat = cam.getWorldToViewMatrix() * glm::scale(glm::vec3(1.0f, scaleVal, 1.0f));

	vector<Renderable>* renderables = &activeScene->getRenderables();
	vector<Renderable>::iterator renderablesIterator = 
		renderables->begin();
	for (; renderablesIterator != renderables->end(); ++renderablesIterator)
	{
		// Send geometry data and matrices
		Renderable renderable = *renderablesIterator;
		glBindVertexArray(renderable.geometry.vertexArrayObjectID);
		glm::mat4 modelToWorldMatrix = renderable.getModelToWorldMatrix();
		glm::mat4 modelViewProjectionMatrix = projMat * camMat * modelToWorldMatrix;
		glUniformMatrix4fv(modelMatUniformLocation, 1, GL_FALSE, &modelToWorldMatrix[0][0]);
		glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &modelViewProjectionMatrix[0][0]);

		// Send material data
		setActiveMaterial(renderable.material);

		// Draw renderable
		glDrawElements(GL_TRIANGLES, renderable.geometry.numIndices, GL_UNSIGNED_SHORT, 0);
	}

	GLHelper::checkErrors("draw");
}

void MyGLWindow::paintGL()
{
	if (activeFramebuffer != NULL &&
		activeFramebuffer->getFramebufferObjectID() != NULL)
	{
		// Render to framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, activeFramebuffer->getFramebufferObjectID());
		glViewport(0, 0, 1024, 1024);
		draw(activeScene->getRenderTargetCamera(), true);
	}

	// Render to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width(), height());
	//glUniform1i(renderTextureUniformLoc, activeFramebuffer->renderTextureID);
	draw(*(activeScene->getActiveCamera()), false);

	GLHelper::checkErrors("paintGL");
}
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
#include <Skybox.h>
#include <map>
#include <list>
#include <OBJLoader.h>
#include <ShaderProgram.h>
#include <ShaderObject.h>
using namespace std;

#define VERTEX_BYTE_SIZE sizeof(float) * 15

const float RANDOM_PLACEMENT_OFFSET = 40.0f;

ShaderProgram* activeProgram = NULL;
ShaderProgram* standardProgram = NULL;
ShaderProgram* shadowProgram = NULL;
//ShaderProgram* drawToScreenProgram = NULL;
bool loadPlaneModel = true;

Scene* scene = new Scene;
Scene* activeScene = scene;

Framebuffer *activeFramebuffer = NULL;
Framebuffer* shadowMap = NULL;
glm::mat4 depthMVP;

// All images
map<string, QImage*>* images = NULL;

// All textures
const int MAX_TEXTURES = 10;
map<string, Texture*>* textures = NULL;

// All cubemaps
const int MAX_CUBEMAPS = 3;
map<string, Cubemap*>* cubemaps = NULL;

// All framebuffers
const int MAX_FRAMEBUFFERS = 2;
map<string, Framebuffer*>* framebuffers = NULL;

// All geometries
map<string, ShapeData*>* geometries = NULL;

// All materials
map<string, Material*>* materials = NULL;

ShapeData* getGeometry(string name)
{
	if (geometries == NULL)
	{
		cout << "Geometries map has not been initialized!" << endl;
		return NULL;
	}

	ShapeData* result = geometries->at(name);
	return result;
}

Material* getMaterial(string name)
{
	if (materials == NULL)
	{
		cout << "Materials map has not been initialized!" << endl;
		return NULL;
	}

	return materials->at(name);
}

Texture* getTexture(string name)
{
	if (textures == NULL)
	{
		cout << "Textures map has not been initialized!" << endl;
		return NULL;
	}

	return textures->at(name);
}

Cubemap* getCubemap(string name)
{
	if (cubemaps == NULL)
	{
		cout << "Cubemaps map has not been initialized!" << endl;
		return NULL;
	}

	return cubemaps->at(name);
}

void setActiveProgram(ShaderProgram* program)
{
	GLHelper::checkErrors("setActiveProgram()");

	if (program == activeProgram) return;

	GLboolean result = glIsProgram(program->getProgramID());
	GLHelper::checkErrors("setActiveProgram().glIsProgram()");
	if (!result)
	{
		cout << "ID \"" << program << "\" is not a program!" << endl;
		return;
	}

	glValidateProgram(program->getProgramID());
	GLHelper::checkErrors("setActiveProgram().glValidateProgram()");

	activeProgram = program;
	glUseProgram(program->getProgramID());
	GLHelper::checkErrors("setActiveProgram().glUseProgram()");

	//cout << "Set program to " << program->getProgramID() << endl;
}

void setActiveDiffuseTexture(Texture* diffuse)
{
	GLint texID = diffuse->getTextureID();
	glActiveTexture(GL_TEXTURE0 + texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	standardProgram->setUniformInt("diffuseTexture", texID);

	GLHelper::checkErrors("setActiveDiffuseTexture");
}

void setActiveNormalMap(Texture* normalMap)
{
	GLint normalMapID = normalMap->getTextureID();
	glActiveTexture(GL_TEXTURE0 + normalMapID);
	glBindTexture(GL_TEXTURE_2D, normalMapID);
	standardProgram->setUniformInt("normalMap", normalMapID);

	GLHelper::checkErrors("setActiveNormalMap");
}

void setActiveAmbientOcclusionMap(Texture* ao)
{
	GLint aoMapID = ao->getTextureID();
	glActiveTexture(GL_TEXTURE0 + aoMapID);
	glBindTexture(GL_TEXTURE_2D, aoMapID);
	standardProgram->setUniformInt("ambientOcclusion", aoMapID);
}

void setActiveMetallicSmoothnessMap(Texture* ms)
{
	GLint msMapID = ms->getTextureID();
	glActiveTexture(GL_TEXTURE0 + msMapID);
	glBindTexture(GL_TEXTURE_2D, msMapID);
	standardProgram->setUniformInt("metallicSmoothness", msMapID);
}

void setActiveCubemap(Cubemap* cubemap)
{
	GLint cubemapID = cubemap->cubemapID;
	glActiveTexture(GL_TEXTURE0 + cubemapID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
	standardProgram->setUniformInt("cubemap", cubemapID);

	GLHelper::checkErrors("setActiveCubemap");
}

void setActiveShadowMap(Texture* shadowMap)
{
	GLint shadowMapID = shadowMap->getTextureID();
	glActiveTexture(GL_TEXTURE0 + shadowMapID);
	glBindTexture(GL_TEXTURE_2D, shadowMapID);
	standardProgram->setUniformInt("shadowMap", shadowMapID);

	GLHelper::checkErrors("setActiveShadowMap");
}

void setActiveFramebuffer(Framebuffer* framebuffer)
{
	activeFramebuffer = framebuffer;
}

void setActiveMaterial(Material* material)
{
	if (material->diffuse != NULL)
	{
		setActiveDiffuseTexture(material->diffuse);
		standardProgram->setUniformFloat("diffuseStrength", material->diffuseStrength);
	}

	else
	{
		setActiveDiffuseTexture(getTexture("Textures/White"));
		standardProgram->setUniformFloat("diffuseStrength", 0.0f);
	}

	if (material->normal != NULL)
	{
		setActiveNormalMap(material->normal);
		standardProgram->setUniformFloat("normalStrength", material->normalStrength);
	}

	else
	{
		setActiveNormalMap(getTexture("Textures/Gray"));
		standardProgram->setUniformFloat("normalStrength", 0.0f);
	}

	standardProgram->setUniformFloat("specularPower", material->specularPower);
	standardProgram->setUniformFloat("emissionStrength", material->emissionStrength);
	standardProgram->setUniformFloat("reflectivity", material->reflectivity);
	standardProgram->setUniformFloat("indexOfRefraction", material->indexOfRefraction);
	standardProgram->setUniformFloat("fresnelValue", material->fresnelValue);
	standardProgram->setUniformFloat("useCubemap", material->useCubemap);

	if (material->ambientOcclusion != NULL)
	{ 
		setActiveAmbientOcclusionMap(material->ambientOcclusion);
		standardProgram->setUniformFloat("ambientOcclusionScale", material->occlusionScale);
	}

	else
	{
		setActiveAmbientOcclusionMap(getTexture("Textures/White"));
		standardProgram->setUniformFloat("ambientOcclusionScale", 0.0f);
	}

	if (material->metallicSmoothness != NULL)
	{
		setActiveMetallicSmoothnessMap(material->metallicSmoothness);
	}

	else
	{
		setActiveMetallicSmoothnessMap(getTexture("Textures/Gray"));
	}

	GLHelper::checkErrors("setActiveMaterial");
}

QImage* MyGLWindow::makeImage(string filename)
{
	GLHelper::checkErrors("makeImage -- before function entry");

	QString filenameAsQString = QString::fromStdString(filename + ".png");
	GLHelper::checkErrors("makeImage -- filename processing");

	QImage* loadedImage = new QImage(filenameAsQString, "png");
	GLHelper::checkErrors("makeImage -- construction");

	if (loadedImage->isNull())
	{
		cout << "Failed to load image \"" << filenameAsQString.constData() << "\"!" << endl;
		throw std::exception();
	}


	QImage* image = new QImage(QGLWidget::convertToGLFormat(*loadedImage));

	GLHelper::checkErrors("makeImage -- conversion");

	std::pair<string, QImage*> pair = 
		std::pair<string, QImage*>(filename, image);

	if (images == NULL) images = new map<string, QImage*>;
	images->emplace(pair);
	cout << "--IMAGE: " << filename << "--" << endl;
	GLHelper::checkErrors("makeImage");
	return image;
}

Texture* MyGLWindow::makeTexture(string filename)
{
	GLHelper::checkErrors("makeTexture -- before function entry");

	QImage* image = makeImage(filename);
	GLHelper::checkErrors("makeTexture -- image creation");

	if (image->isNull())
	{
		cout << "NULL REFERENCE: Image is null!" << endl;
		throw exception();
	}

	Texture* texture = new Texture(image->width(), image->height(), GL_RGBA, GL_RGBA, image->bits());

	GLHelper::checkErrors("makeTexture - texture construction");


	std::pair<string, Texture*> pair = (std::pair<string, Texture*>(filename, texture));
	if (textures == NULL) textures = new map<string, Texture*>;
	textures->emplace(pair);
	cout << "--TEXTURE (" << texture->getTextureID() << "): " << filename << "--" << endl;
	GLHelper::checkErrors("makeTexture - function complete");

	return texture;
}

Cubemap* MyGLWindow::makeCubemap(string filename)
{
	QImage* left = makeImage(filename + "_Left");
	QImage* right = makeImage(filename + "_Right");
	QImage* up = makeImage(filename + "_Up");
	QImage* down = makeImage(filename + "_Down");
	QImage* back = makeImage(filename + "_Back");
	QImage* forward = makeImage(filename + "_Forward");
	QImage* images[] = { left, right, up, down, back, forward };
	Cubemap* cubemap = new Cubemap((QImage**)images);


	if (cubemaps == NULL) cubemaps = new map<string, Cubemap*>;
	cubemaps->emplace(std::pair<string, Cubemap*>(filename, cubemap));
	cout << "--CUBEMAP (" << cubemap->cubemapID << "): " << filename << "--" << endl;
	GLHelper::checkErrors("makeCubemap");
	return cubemap;
}

Framebuffer* MyGLWindow::makeFramebuffer(string name, bool useColor, bool useDepth, int width, int height)
{
	cout << "Making Framebuffer \"" << name << "\"...";

	Framebuffer* framebuffer = new Framebuffer(name, useColor, useDepth, width, height);

	if (framebuffer->getStatus() != GL_FRAMEBUFFER_COMPLETE)
	{
		string err;
		switch (framebuffer->getStatus())
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

	if (framebuffers == NULL) framebuffers = new map<string, Framebuffer*>;
	framebuffers->emplace(std::pair<string, Framebuffer*>(name, framebuffer));
	cout << "done." << endl;
	GLHelper::checkErrors("makeFramebuffer");
	return framebuffer;
}

void MyGLWindow::addGeometry(string name, ShapeData* geometry)
{
	if (geometry == NULL)
	{
		cout << "addGeometry: given null geometry!" << endl;
		return;
	}

	glGenBuffers(1, &geometry->vertexBufferID);
	GLHelper::checkErrors("MyGLWindow::addGeometry().glGenBuffers()");

	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, geometry->vertexBufferSize(),
		geometry->vertices, GL_STATIC_DRAW);

	GLHelper::checkErrors("addGeometry - vertex buffer creation");

	glGenBuffers(1, &geometry->indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, geometry->indexBufferSize(),
		geometry->indices, GL_STATIC_DRAW);

	GLHelper::checkErrors("addGeometry - index buffer creation");

	glGenVertexArrays(1, &geometry->vertexArrayObjectID);
	glBindVertexArray(geometry->vertexArrayObjectID);
	glEnableVertexAttribArray(0); // Position
	glEnableVertexAttribArray(1); // Color
	glEnableVertexAttribArray(2); // Normal
	glEnableVertexAttribArray(3); // UV
	glEnableVertexAttribArray(4); // Tangent
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBufferID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (char*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (char*)(sizeof(float) * 7));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (char*)(sizeof(float) * 10));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (char*)(sizeof(float) * 12));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->indexBufferID);

	GLHelper::checkErrors("addGeometry - VAO creation");

	if (geometries == NULL) geometries = new map<string, ShapeData*>;
	geometries->emplace(std::pair<string, ShapeData*>(name, geometry));
	cout << "Added geometry: " << name << endl;
	GLHelper::checkErrors("addGeometry");
}

void MyGLWindow::addTexture(string name, Texture* texture)
{
	if (texture == NULL)
	{
		cout << "addTexture: given null texture!" << endl;
		return;
	}

	if (textures == NULL) textures = new map<string, Texture*>;
	textures->emplace(std::pair<string, Texture*>(name, texture));
}

void MyGLWindow::addMaterial(string name, Material* material)
{
	if (material == NULL)
	{
		cout << "addMaterial: given null material!" << endl;
		return;
	}

	if (materials == NULL) materials = new map<string, Material*>;
	materials->emplace(std::pair<string, Material*>(name, material));
}

void MyGLWindow::updateUniforms()
{
	standardProgram->setUniformVec3("ambientLight", activeScene->getAmbientLight());
	standardProgram->setUniformVec3("lightPos", activeScene->getActiveLight()->renderable->transform.getPosition());
	standardProgram->setUniformVec3("diffuseColor", activeScene->getActiveLight()->color);
	standardProgram->setUniformVec3("specularColor", activeScene->getActiveLight()->color);
	standardProgram->setUniformVec3("camPos", activeScene->getActiveCamera()->transform.getPosition());

	GLHelper::checkErrors("update scene uniforms");
}

void MyGLWindow::mouseMoveEvent(QMouseEvent* e)
{
	activeScene->getActiveCamera()->mouseUpdate(glm::vec2(e->x(), e->y()));
	repaint();
}

void MyGLWindow::spawnRenderable()
{
	int randomType = rand() % 2;
	Renderable* renderable = new Renderable;
	switch (randomType)
	{
	case 0:
		renderable->geometry = getGeometry("sphere");
		break;
	case 1:
		renderable->geometry = getGeometry("cube");
		break;
	}

	renderable->material = materials->at("standard");

	glm::vec3 randPosition = glm::vec3(
		rand() / (float)RAND_MAX - 0.5f, 
		rand() / (float)RAND_MAX - 0.5f, 
		rand() / (float)RAND_MAX - 0.5f) 
		* RANDOM_PLACEMENT_OFFSET;

	renderable->transform.setPosition(randPosition);

	float scaleValue = rand() / RAND_MAX * 5 + 2.5f;
	renderable->transform.setScale(glm::vec3(scaleValue, scaleValue, scaleValue));

	renderable->transform.setRotation(glm::vec3(
		rand() / RAND_MAX * 360.0f, 
		rand() / RAND_MAX * 360.0f, 
		rand() / RAND_MAX * 360.0f));

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
			activeScene->getActiveCamera()->moveLeft();
			break;
		case Qt::Key::Key_D:
			activeScene->getActiveCamera()->moveRight();
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
			activeScene->getActiveLight()->renderable->transform.moveBackward();
			break;
		case Qt::Key::Key_2:
			activeScene->getActiveLight()->renderable->transform.moveForward();
			break;
		case Qt::Key::Key_Left:
			activeScene->getActiveLight()->renderable->transform.moveLeft();
			break;
		case Qt::Key::Key_Right:
			activeScene->getActiveLight()->renderable->transform.moveRight();
			break;
		case Qt::Key::Key_Up:
			activeScene->getActiveLight()->renderable->transform.moveUp();
			break;
		case Qt::Key::Key_Down:
			activeScene->getActiveLight()->renderable->transform.moveDown();
			break;
		case Qt::Key::Key_R:
			activeScene->switchCamera();
			break;
	}
	MyGLWindow::updateGL();
}

void MyGLWindow::initGeometries()
{
	ShapeData* cube = OBJLoader::loadOBJFile("Models/Cube.obj");
	MyGLWindow::addGeometry("cube", cube);

	//ShapeData* invCube = OBJLoader::loadOBJFile("Models/InvertedCube.obj");
	ShapeData* invCube = OBJLoader::loadOBJFile("Models/Cube.obj");
	invCube->invertNormals();
	MyGLWindow::addGeometry("invCube", invCube);

	ShapeData* plane = OBJLoader::loadOBJFile("Models/Plane.obj");
	MyGLWindow::addGeometry("plane", plane);

	ShapeData* sphere = OBJLoader::loadOBJFile("Models/Sphere16.obj");
	MyGLWindow::addGeometry("sphere", sphere);

	/*ShapeData* screenQuad = new ShapeData();
	screenQuad->numIndices = 4;
	screenQuad->numVertices = 4;
	screenQuad->indices = new unsigned short[4]{ 0, 1, 2, 3 };
	Vertex v1 = Vertex();
	v1.position = glm::vec3(0.0f, 0.0f, 0.0f);
	Vertex v2 = Vertex();
	v2.position = glm::vec3(1.0f, 0.0f, 0.0f);
	Vertex v3 = Vertex();
	v3.position = glm::vec3(1.0f, 1.0f, 0.0f);
	Vertex v4 = Vertex();
	v4.position = glm::vec3(0.0f, 1.0f, 0.0f);
	screenQuad->vertices = new Vertex[4]
	{
		v1, v2, v3, v4
	};
	MyGLWindow::addGeometry("screenQuad", screenQuad);*/

	if (loadPlaneModel)
	{
		ShapeData* model = OBJLoader::loadOBJFile("Models/BristolF2.obj");
		MyGLWindow::addGeometry("f2", model);
	}
}

void MyGLWindow::initTextures()
{
	makeTexture("Textures/White");
	makeTexture("Textures/Gray");
	makeTexture("Textures/Black");
	makeTexture("tri");
	makeTexture("ShapesNormal");

	// Cubemap
	makeCubemap("Textures/Skybox_Dawn512");

	// F2
	if (loadPlaneModel)
	{
		makeTexture("Textures/BristolF2_Albedo");
		makeTexture("Textures/BristolF2_AO");
		makeTexture("Textures/BristolF2_MetallicSmoothness");
		makeTexture("Textures/BristolF2_Normal");
	}
}

void MyGLWindow::initMaterials()
{
	// Light material
	Material* lightMaterial = new Material;
	lightMaterial->diffuse = NULL;
	lightMaterial->diffuseStrength = 0.0f;
	lightMaterial->normal = NULL;
	lightMaterial->normalStrength = 0.0f;
	lightMaterial->emissionStrength = 1.0f;
	lightMaterial->normalStrength = 0.0f;
	lightMaterial->reflectivity = 0.0f;
	lightMaterial->useCubemap = 0.0f;
	lightMaterial->metallicSmoothness = NULL;
	lightMaterial->ambientOcclusion = NULL;
	addMaterial("light", lightMaterial);

	// Skybox material
	Material* skyboxMaterial = new Material;
	skyboxMaterial->diffuse = NULL;
	skyboxMaterial->diffuseStrength = 0.0f;
	skyboxMaterial->normal = NULL;
	skyboxMaterial->normalStrength = 0.0f;
	skyboxMaterial->specularPower = 0.01f;
	skyboxMaterial->emissionStrength = 1.0f;
	skyboxMaterial->reflectivity = 0.0f;
	skyboxMaterial->indexOfRefraction = 1.0f;
	skyboxMaterial->fresnelValue = 0.0f;
	skyboxMaterial->useCubemap = 1.0f;
	addMaterial("skybox", skyboxMaterial);

	// Reflective material
	Material* reflectiveMaterial = new Material;
	reflectiveMaterial->diffuseStrength = 0.0f;
	reflectiveMaterial->normal = getTexture("ShapesNormal");
	reflectiveMaterial->normalStrength = 1.0f;
	reflectiveMaterial->reflectivity = 1.0f;
	addMaterial("reflective", reflectiveMaterial);

	// Standard material
	Material* standardMaterial = new Material;
	standardMaterial->diffuse = getTexture("tri");
	standardMaterial->diffuseStrength = 1.0f;
	standardMaterial->normal = getTexture("ShapesNormal");
	standardMaterial->normalStrength = 1.0f;
	standardMaterial->specularPower = 100.0f;
	addMaterial("standard", standardMaterial);

	// Flat material
	Material* flatMaterial = new Material;
	flatMaterial->diffuseStrength = 0.0f;
	flatMaterial->normalStrength = 0.0f;
	addMaterial("flat", flatMaterial);

	// Shadow map material
	Material* shadowMapMaterial = new Material();
	shadowMapMaterial->diffuse = shadowMap->getDepthTexture();
	shadowMapMaterial->diffuseStrength = 1.0f;
	shadowMapMaterial->normal = NULL;
	shadowMapMaterial->normalStrength = 0.0f;
	shadowMapMaterial->emissionStrength = 1.0f;
	addMaterial("shadowMap", shadowMapMaterial);

	if (loadPlaneModel)
	{
		// F2 material
		Material* f2Material = new Material;
		f2Material->diffuse = getTexture("Textures/BristolF2_Albedo");
		f2Material->diffuseStrength = 1.0f;
		f2Material->normal = getTexture("Textures/BristolF2_Normal");
		//f2Material->normalStrength = 1.0f;
		f2Material->normalStrength = 0.0f;
		f2Material->reflectivity = 0.2f;
		f2Material->ambientOcclusion = getTexture("Textures/BristolF2_AO");
		f2Material->occlusionScale = 1.0f;
		f2Material->metallicSmoothness = getTexture("Textures/BristolF2_MetallicSmoothness");
		addMaterial("f2", f2Material);
	}
}

void MyGLWindow::initScene()
{
	Renderable* lightRenderable = new Renderable;
	lightRenderable->material = getMaterial("light");
	lightRenderable->geometry = getGeometry("sphere");

	activeScene = scene;

	// Light renderable
	activeScene->getActiveLight()->renderable = lightRenderable;
	activeScene->getActiveLight()->renderable->transform.setPosition(glm::vec3(0.0f, 5.0f, 0.0f));
	activeScene->setAmbientLight(glm::vec3(0.23f, 0.22f, 0.23f));
	activeScene->getActiveLight()->renderable->transform.setViewDirection(glm::normalize(glm::vec3(1.0f, -0.25f, 1.0f)));

	// Skybox
	activeScene->skybox = new Skybox;
	activeScene->skybox->geometry = getGeometry("cube");
	activeScene->skybox->cubemap = getCubemap("Textures/Skybox_Dawn512");

	// Plane
	Renderable* plane = new Renderable;
	plane->geometry = getGeometry("plane");
	plane->material = getMaterial("flat");
	plane->transform.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	plane->transform.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	plane->transform.setScale(glm::vec3(50.0f, 50.0f, 50.0f));
	plane->castsShadows = true;
	activeScene->addRenderable(plane);

	Renderable* shadowMapPlane = new Renderable;
	shadowMapPlane->geometry = getGeometry("plane");
	shadowMapPlane->material = getMaterial("shadowMap");
	shadowMapPlane->transform.setPosition(glm::vec3(-5.0f, 2.5f, -5.0f));
	shadowMapPlane->transform.setRotation(glm::vec3(90.0f, 45.0f, 0.0f));
	shadowMapPlane->transform.setScale(glm::vec3(3.0f, 3.0f, 3.0f));
	shadowMapPlane->castsShadows = false;
	shadowMapPlane->cullingOn = false;
	activeScene->addRenderable(shadowMapPlane);

	if (loadPlaneModel)
	{
		// F2 renderable
		Renderable* f2 = new Renderable;
		f2->geometry = getGeometry("f2");
		f2->material = getMaterial("f2");
		f2->transform.setPosition(glm::vec3(0.0f, 2.5f, 0.0f));
		f2->transform.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
		f2->transform.setScale(glm::vec3(0.01f, 0.01f, 0.01f));
		f2->drawMode = GL_TRIANGLES;
		activeScene->addRenderable(f2);
	}

	else
	{
		// Cube
		Renderable* cube = new Renderable;
		cube->geometry = getGeometry("cube");
		cube->material = getMaterial("standard");
		cube->transform.setPosition(glm::vec3(0.0f, 2.5f, 0.0f));
		cube->transform.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
		cube->transform.setScale(glm::vec3(2.0f, 2.0f, 2.0f));
		activeScene->addRenderable(cube);

		// Sphere
		Renderable* sphere = new Renderable;
		sphere->geometry = getGeometry("sphere");
		sphere->material = getMaterial("flat");
		sphere->transform.setPosition(glm::vec3(3.0f, 2.0f, 1.0f));
		sphere->transform.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
		sphere->transform.setScale(glm::vec3(2.0f, 2.0f, 2.0f));
		activeScene->addRenderable(sphere);
	}
}

void MyGLWindow::installShaders()
{
	// Create shader objects
	ShaderObject* standardVertexShader = new ShaderObject(GL_VERTEX_SHADER, "VertexShaders/Standard.glsl");
	ShaderObject* standardFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER, "FragmentShaders/Standard.glsl");
	ShaderObject* shadowVertexShader = new ShaderObject(GL_VERTEX_SHADER, "VertexShaders/Shadow.glsl");
	ShaderObject* shadowFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER, "FragmentShaders/Shadow.glsl");
	//ShaderObject* screenVertexShader = new ShaderObject(GL_VERTEX_SHADER, "VertexShaders/Screen.glsl");
	//ShaderObject* screenFragmentShader = new ShaderObject(GL_FRAGMENT_SHADER, "FragmentShaders/Screen.glsl");

	// If either of the shaders is broken, quit out
	if (!standardVertexShader->checkStatus() || !standardFragmentShader->checkStatus() ||
		!shadowVertexShader->checkStatus() || !shadowFragmentShader->checkStatus())// ||
		//!screenVertexShader->checkStatus() || !screenFragmentShader->checkStatus())
		throw exception();

	cout << "Shaders compiled successfully." << endl;

	// Init GL program and link shaders to it
	standardProgram = new ShaderProgram(standardVertexShader, standardFragmentShader);
	shadowProgram = new ShaderProgram(shadowVertexShader, shadowFragmentShader);
	//drawToScreenProgram = new ShaderProgram(screenVertexShader, screenFragmentShader);

	GLHelper::checkErrors("installShaders() -- create programs");

	if (!standardProgram->checkStatus() || !shadowProgram->checkStatus())// || !drawToScreenProgram->checkStatus())
		throw exception();

	GLHelper::checkErrors("installShaders() -- check program status");

	setActiveProgram(standardProgram);

	GLHelper::checkErrors("installShaders");
}

void MyGLWindow::initializeGL()
{
	// Inits OpenGL stuff
	glewInit();
	GLHelper::checkErrors("initializeGL -- glewInit");

	installShaders();

	// Render texture
	activeFramebuffer = makeFramebuffer("RenderTexture", true, false, 1024, 1024);

	// Shadow map
	shadowMap = makeFramebuffer("ShadowMap", false, true, 1024, 1024);

	initGeometries();
	initTextures();
	initMaterials();
	initScene();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Clear to black
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

	GLHelper::checkErrors("initializeGL");
}

void MyGLWindow::drawSkybox(Camera* cam, glm::mat4 projMat, bool flipped)
{
	glm::vec3 camPosBackup = glm::vec3(cam->transform.getPosition());
	glBindVertexArray(activeScene->skybox->geometry->vertexArrayObjectID);

	GLHelper::checkErrors("drawSkybox -- bind vertex array");
	
	setActiveMaterial(getMaterial("skybox"));
	setActiveCubemap(activeScene->skybox->cubemap);

	cam->transform.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	float scaleVal = flipped ? 1.0f : -1.0f;
	glm::mat4 camMat = cam->getWorldToViewMatrix() * glm::scale(glm::vec3(1.0f, scaleVal, 1.0f));
	glm::mat4 modelMat = glm::mat4();
	glm::mat4 mvp = projMat * cam->getWorldToViewMatrix() * modelMat;

	standardProgram->setUniformVec3("camPos", cam->transform.getPosition());
	standardProgram->setUniformMat4("modelMatrix", modelMat);
	standardProgram->setUniformMat4("mvp", mvp);

	GLHelper::checkErrors("drawSkybox -- calculate matrices");

	glDrawElements(GL_TRIANGLES, activeScene->skybox->geometry->numIndices, GL_UNSIGNED_SHORT, 0);

	GLHelper::checkErrors("drawSkybox -- draw elements");

	glClear(GL_DEPTH_BUFFER_BIT);

	cam->transform.setPosition(camPosBackup);

	GLHelper::checkErrors("drawSkybox");
}

void drawRenderable(Renderable* renderable, glm::mat4 camMat, glm::mat4 projMat, bool shadow)
{
	glBindVertexArray(renderable->geometry->vertexArrayObjectID);

	GLHelper::checkErrors("draw -- bind renderable VAO");

	if (shadow)
	{
		glm::mat4 depthModelMat = renderable->transform.getModelToWorldMatrix();
		depthMVP = projMat * camMat * depthModelMat;
		shadowProgram->setUniformMat4("depthMVP", depthMVP);
	}

	else
	{
		glm::mat4 modelToWorldMatrix = renderable->transform.getModelToWorldMatrix();
		glm::mat4 modelViewProjectionMatrix = projMat * camMat * modelToWorldMatrix;
		
		standardProgram->setUniformMat4("modelMatrix", modelToWorldMatrix);
		standardProgram->setUniformMat4("mvp", modelViewProjectionMatrix);

		glm::mat4 shadowBiasMatrix = glm::mat4(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.5f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f
		);
		glm::mat4 shadowBiasMVP = shadowBiasMatrix * depthMVP;
		standardProgram->setUniformMat4("depthBiasMVP", shadowBiasMVP);
	}

	GLHelper::checkErrors("draw -- send renderable matrices");

	if (renderable->cullingOn) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	GLHelper::checkErrors("draw -- set render material");

	// Draw renderable
	glDrawElements(renderable->drawMode, renderable->geometry->numIndices, GL_UNSIGNED_SHORT, 0);

	GLHelper::checkErrors("draw -- draw renderable elements");
}

void MyGLWindow::drawShadows(Camera* cam, bool flipped)
{
	glClear(GL_DEPTH_BUFFER_BIT);

	glm::vec3 pos = activeScene->getActiveLight()->renderable->transform.getPosition();
	glm::vec3 dir = -activeScene->getActiveLight()->renderable->transform.getViewDirection();

	glm::mat4 depthProjMat = glm::ortho<float>(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 20.0f);
	//glm::mat4 depthViewMat = glm::lookAt(dir, -pos, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 depthViewMat = glm::lookAt(dir, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 depthModelMat = glm::mat4(1.0);

	GLHelper::checkErrors("drawShadows -- send shadow matrix");

	vector<Renderable*>* renderables = activeScene->getRenderables();
	vector<Renderable*>::iterator renderablesIterator =
		renderables->begin();
	//drawRenderable(activeScene->getActiveLight()->renderable, depthViewMat, depthProjMat, true);
	for (; renderablesIterator != renderables->end(); ++renderablesIterator)
	{
		// Send geometry data and matrices
		Renderable* renderable = *renderablesIterator;
		if (!renderable->castsShadows) continue;
		drawRenderable(renderable, depthViewMat, depthProjMat, true);
	}
}

void MyGLWindow::draw(Camera* cam, bool flipped)
{
	GLHelper::checkErrors("draw -- before function entry");

	// View to projection
	glm::mat4 projMat = glm::perspective(
		activeScene->getActiveCamera()->fov,
		((float)width() / (float)height()),
		activeScene->getActiveCamera()->clipNear,
		activeScene->getActiveCamera()->clipFar);

	GLHelper::checkErrors("draw -- update projection matrix");

	glDisable(GL_CULL_FACE);
	drawSkybox(cam, projMat, true);
	glEnable(GL_CULL_FACE);

	float scaleVal = flipped ? -1.0f : 1.0f;
	glm::mat4 camMat = cam->getWorldToViewMatrix() * glm::scale(glm::vec3(1.0f, scaleVal, 1.0f));

	standardProgram->setUniformVec3("camPos", activeScene->getActiveCamera()->transform.getPosition());

	GLHelper::checkErrors("draw -- update camera pos");

	vector<Renderable*>* renderables = activeScene->getRenderables();
	vector<Renderable*>::iterator renderablesIterator = 
		renderables->begin();
	drawRenderable(activeScene->getActiveLight()->renderable, camMat, projMat, false);
	for (; renderablesIterator != renderables->end(); ++renderablesIterator)
	{
		// Send geometry data and matrices
		Renderable* renderable = *renderablesIterator;

		// Send material data
		setActiveMaterial(renderable->material);
		drawRenderable(renderable, camMat, projMat, false);
	}

	GLHelper::checkErrors("draw");
}

void MyGLWindow::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLHelper::checkErrors("paintGL -- clear buffers");

	setActiveProgram(standardProgram);

	updateUniforms();

	GLHelper::checkErrors("paintGL -- update matrix uniforms");

	// Draw to shadow map
	if (shadowMap != NULL)
	{
		setActiveProgram(shadowProgram);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMap->getFramebufferObjectID());
		glViewport(0, 0, shadowMap->getWidth(), shadowMap->getHeight());
		//glCullFace(GL_FRONT);
		drawShadows(activeScene->getShadowCamera(), false);
	}

	setActiveProgram(standardProgram);
	setActiveShadowMap(shadowMap->getDepthTexture());
	//glCullFace(GL_BACK);
	if (activeFramebuffer != NULL &&
		activeFramebuffer->getFramebufferObjectID() != NULL)
	{
		// Render to framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, activeFramebuffer->getFramebufferObjectID());
		GLHelper::checkErrors("paintGL -- bind active framebuffer");

		glViewport(0, 0, activeFramebuffer->getWidth(), activeFramebuffer->getHeight());
		GLHelper::checkErrors("paintGL -- active framebuffer viewport");

		draw(activeScene->getRenderTargetCamera(), true);
		GLHelper::checkErrors("paintGL -- framebuffer draw call");
	}

	// Render to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GLHelper::checkErrors("paintGL -- bind screen framebuffer");

	glViewport(0, 0, width(), height());
	GLHelper::checkErrors("paintGL -- screen viewport");

	draw(activeScene->getActiveCamera(), false);
	GLHelper::checkErrors("paintGL -- main draw call");

	/*setActiveProgram(drawToScreenProgram);
	GLint id = shadowMap->getDepthTextureID();
	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(GL_TEXTURE_2D, id);
	drawToScreenProgram->setUniformInt("depthTexture", id);
	glBindVertexArray(getGeometry("screenQuad")->vertexArrayObjectID);
	glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, 0);
	GLHelper::checkErrors("paintGL -- draw screen quad");*/
}
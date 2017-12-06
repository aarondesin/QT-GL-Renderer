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
bool loadPlaneModel = false;

Scene* scene = new Scene;
Scene* activeScene = scene;

Framebuffer *activeFramebuffer = NULL;
Framebuffer* shadowMap = NULL;

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
GLint useCubemapUniformLoc;
GLint cubemapUniformLoc;
GLint reflectivityUniformLoc;
GLint emissionStrengthUniformLoc;
GLint indexOfRefractionUniformLoc;
GLint fresnelValueUniformLoc;
GLint ambientOcclusionUniformLoc;
GLint ambientOcclusionScaleUniformLoc;
GLint metallicSmoothnessUniformLoc;
GLint renderTextureUniformLoc;

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

	cout << "Set program to " << program->getProgramID() << endl;
}

void setActiveDiffuseTexture(Texture* diffuse)
{
	GLint texID = diffuse->getTextureID();

	if (texID < 0)
	{
		cout << "Invalid texture ID!" << endl;
		return;
	}

	glActiveTexture(GL_TEXTURE0 + texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	diffuseTextureUniformLoc = standardProgram->getUniformLocation("diffuseTexture");
	glUniform1i(diffuseTextureUniformLoc, texID);

	GLHelper::checkErrors("setActiveDiffuseTexture");
}

void setActiveNormalMap(Texture* normalMap)
{
	GLint normalMapID = normalMap->getTextureID();

	if (normalMapID < 0)
	{
		cout << "Invalid normal map ID!" << endl;
		return;
	}

	glActiveTexture(GL_TEXTURE0 + normalMapID);
	glBindTexture(GL_TEXTURE_2D, normalMapID);
	normalMapUniformLoc = standardProgram->getUniformLocation("normalMap");
	glUniform1i(normalMapUniformLoc, normalMapID);

	GLHelper::checkErrors("setActiveNormalMap");
}

void setActiveAmbientOcclusionMap(Texture* ao)
{
	GLint aoMapID = ao->getTextureID();
	if (aoMapID < 0)
	{
		cout << "Invalid AO map ID!" << endl;
	}

	glActiveTexture(GL_TEXTURE0 + aoMapID);
	glBindTexture(GL_TEXTURE_2D, aoMapID);
	ambientOcclusionUniformLoc = activeProgram->getUniformLocation("ambientOcclusion");
	glUniform1i(ambientOcclusionUniformLoc, aoMapID);
}

void setActiveMetallicSmoothnessMap(Texture* ms)
{
	GLint msMapID = ms->getTextureID();
	if (msMapID < 0)
	{
		cout << "Invalid MetallicSmoothness map ID!" << endl;
	}

	glActiveTexture(GL_TEXTURE0 + msMapID);
	glBindTexture(GL_TEXTURE_2D, msMapID);
	metallicSmoothnessUniformLoc = activeProgram->getUniformLocation("metallicSmoothness");
	glUniform1i(metallicSmoothnessUniformLoc, msMapID);
}

void setActiveRenderTexture(Texture* renderTexture)
{
	GLint renderTextureID = renderTexture->getTextureID();
	if (renderTextureID < 0)
	{
		cout << "Invalid render texture ID!" << endl;
	}

	glActiveTexture(GL_TEXTURE0 + renderTextureID);
	glBindTexture(GL_TEXTURE_2D, renderTextureID);
	renderTextureUniformLoc = activeProgram->getUniformLocation("renderTexture");
	glUniform1i(renderTextureUniformLoc, renderTextureID);
}

void setActiveCubemap(Cubemap* cubemap)
{
	GLint cubemapID = cubemap->cubemapID;

	if (cubemapID < 0)
	{
		cout << "Invalid cubemap ID!" << endl;
		return;
	}

	glActiveTexture(GL_TEXTURE0 + cubemapID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);
	cubemapUniformLoc = activeProgram->getUniformLocation("cubemap");
	glUniform1i(cubemapUniformLoc, cubemap->cubemapID);

	GLHelper::checkErrors("setActiveCubemap");
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
		diffuseStrengthUniformLoc = activeProgram->getUniformLocation("diffuseStrength");
		glUniform1f(diffuseStrengthUniformLoc, material->diffuseStrength);
	}

	else
	{
		setActiveDiffuseTexture(getTexture("Textures/White"));
		diffuseStrengthUniformLoc = activeProgram->getUniformLocation("diffuseStrength");
		glUniform1f(diffuseStrengthUniformLoc, 0.0f);
	}

	if (material->normal != NULL)
	{
		setActiveNormalMap(material->normal);
		normalStrengthUniformLoc = activeProgram->getUniformLocation("normalStrength");
		glUniform1f(normalStrengthUniformLoc, material->normalStrength);
	}

	else
	{
		setActiveNormalMap(getTexture("Textures/Gray"));
		normalStrengthUniformLoc = activeProgram->getUniformLocation("normalStrength");
		glUniform1f(normalStrengthUniformLoc, 0.0f);
	}

	specularPowerUniformLoc = activeProgram->getUniformLocation("specularPower");
	glUniform1f(specularPowerUniformLoc, material->specularPower);

	emissionStrengthUniformLoc = activeProgram->getUniformLocation("emissionStrength");
	glUniform1f(emissionStrengthUniformLoc, material->emissionStrength);

	reflectivityUniformLoc = activeProgram->getUniformLocation("reflectivity");
	glUniform1f(reflectivityUniformLoc, material->reflectivity);

	indexOfRefractionUniformLoc = activeProgram->getUniformLocation("indexOfRefraction");
	glUniform1f(indexOfRefractionUniformLoc, material->indexOfRefraction);

	fresnelValueUniformLoc = activeProgram->getUniformLocation("fresnelValue");
	glUniform1f(fresnelValueUniformLoc, material->fresnelValue);

	useCubemapUniformLoc = activeProgram->getUniformLocation("useCubemap");
	glUniform1f(useCubemapUniformLoc, material->useCubemap);

	if (material->ambientOcclusion != NULL)
	{ 
		setActiveAmbientOcclusionMap(material->ambientOcclusion);
		ambientOcclusionScaleUniformLoc = activeProgram->getUniformLocation("ambientOcclusionScale");
		glUniform1f(ambientOcclusionScaleUniformLoc, material->occlusionScale);
	}

	else
	{
		setActiveAmbientOcclusionMap(getTexture("Textures/White"));
		ambientOcclusionScaleUniformLoc = activeProgram->getUniformLocation("ambientOcclusionScale");
		glUniform1f(ambientOcclusionScaleUniformLoc, 0.0f);
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

	//cout << "--FRAMEBUFFER (" << framebuffer.framebufferObjectID << "): " << name << "--" << endl;
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
	//geometries[name] = geometry;
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
	// Update ambient lighting strength
	ambientLightUniformLoc = activeProgram->getUniformLocation("ambientLight");
	glUniform3f(ambientLightUniformLoc,
		activeScene->getAmbientLight().r,
		activeScene->getAmbientLight().g,
		activeScene->getAmbientLight().b);

	// Update light position
	lightPosUniformLoc = activeProgram->getUniformLocation("lightPos");
	glUniform3f(lightPosUniformLoc,
		activeScene->getActiveLight()->position.x,
		activeScene->getActiveLight()->position.y,
		activeScene->getActiveLight()->position.z);

	// Update diffuse lighting
	diffuseLightUniformLoc = activeProgram->getUniformLocation("diffuseColor");
	glUniform3f(diffuseLightUniformLoc,
		activeScene->getActiveLight()->color.r,
		activeScene->getActiveLight()->color.g,
		activeScene->getActiveLight()->color.b);

	// Update specular lighting
	specularColorUniformLoc = activeProgram->getUniformLocation("specularColor");
	glUniform3f(specularColorUniformLoc,
		activeScene->getActiveLight()->color.r,
		activeScene->getActiveLight()->color.g,
		activeScene->getActiveLight()->color.b);

	// Update camera position
	cameraPosUniformLoc = activeProgram->getUniformLocation("camPos");
	glUniform3f(cameraPosUniformLoc,
		activeScene->getActiveCamera()->getPosition().x,
		activeScene->getActiveCamera()->getPosition().y,
		activeScene->getActiveCamera()->getPosition().z);

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

	renderable->position = randPosition;

	float scaleValue = rand() / RAND_MAX * 5 + 2.5f;
	renderable->scale = glm::vec3(scaleValue, scaleValue, scaleValue);

	renderable->rotation = glm::vec3(
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
			activeScene->getActiveLight()->moveBackward();
			break;
		case Qt::Key::Key_2:
			activeScene->getActiveLight()->moveForward();
			break;
		case Qt::Key::Key_Left:
			activeScene->getActiveLight()->moveLeft();
			break;
		case Qt::Key::Key_Right:
			activeScene->getActiveLight()->moveRight();
			break;
		case Qt::Key::Key_Up:
			activeScene->getActiveLight()->moveUp();
			break;
		case Qt::Key::Key_Down:
			activeScene->getActiveLight()->moveDown();
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
	activeScene->getActiveLight()->position = glm::vec3(0.0f, 5.0f, 0.0f);
	activeScene->setAmbientLight(glm::vec3(0.23f, 0.22f, 0.23f));

	// Skybox
	activeScene->skybox = new Skybox;
	activeScene->skybox->geometry = getGeometry("cube");
	activeScene->skybox->cubemap = getCubemap("Textures/Skybox_Dawn512");

	// Plane
	Renderable* plane = new Renderable;
	plane->geometry = getGeometry("plane");
	plane->material = getMaterial("reflective");
	plane->position = glm::vec3(0.0f, 0.0f, 0.0f);
	plane->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	plane->scale = glm::vec3(50.0f, 50.0f, 50.0f);
	activeScene->addRenderable(plane);

	if (loadPlaneModel)
	{
		// F2 renderable
		Renderable* f2 = new Renderable;
		f2->geometry = getGeometry("f2");
		f2->material = getMaterial("f2");
		f2->position = glm::vec3(0.0f, 2.5f, 0.0f);
		f2->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		f2->scale = glm::vec3(0.01f, 0.01f, 0.01f);
		f2->drawMode = GL_TRIANGLES;
		activeScene->addRenderable(f2);
	}

	else
	{
		// Cube
		Renderable* cube = new Renderable;
		cube->geometry = getGeometry("cube");
		cube->material = getMaterial("standard");
		cube->position = glm::vec3(0.0f, 2.5f, 0.0f);
		cube->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		cube->scale = glm::vec3(2.0f, 2.0f, 2.0f);
		activeScene->addRenderable(cube);

		// Sphere
		Renderable* sphere = new Renderable;
		sphere->geometry = getGeometry("sphere");
		sphere->material = getMaterial("flat");
		sphere->position = glm::vec3(3.0f, 2.0f, 1.0f);
		sphere->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		sphere->scale = glm::vec3(2.0f, 2.0f, 2.0f);
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

	// If either of the shaders is broken, quit out
	if (!standardVertexShader->checkStatus() || !standardFragmentShader->checkStatus() ||
		!shadowVertexShader->checkStatus() || !shadowFragmentShader->checkStatus())
		throw exception();

	cout << "Shaders compiled successfully." << endl;

	// Init GL program and link shaders to it
	standardProgram = new ShaderProgram(standardVertexShader, standardFragmentShader);
	shadowProgram = new ShaderProgram(shadowVertexShader, shadowFragmentShader);

	GLHelper::checkErrors("installShaders() -- create programs");

	if (!standardProgram->checkStatus() || !shadowProgram->checkStatus())
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

	initGeometries();
	initTextures();
	initMaterials();
	initScene();

	// Render texture
	activeFramebuffer = makeFramebuffer("RenderTexture", true, false, 1024, 1024);

	// Shadow map
	shadowMap = makeFramebuffer("ShadowMap", false, true, 1024, 1024);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Clear to black
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

	GLHelper::checkErrors("initializeGL");
}

void MyGLWindow::drawSkybox(Camera* cam, glm::mat4 projMat, bool flipped)
{
	glm::vec3 camPosBackup = glm::vec3(cam->getPosition());
	glBindVertexArray(activeScene->skybox->geometry->vertexArrayObjectID);

	GLHelper::checkErrors("drawSkybox -- bind vertex array");
	
	setActiveMaterial(getMaterial("skybox"));
	setActiveCubemap(activeScene->skybox->cubemap);

	cam->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	float scaleVal = flipped ? 1.0f : -1.0f;
	glm::mat4 camMat = cam->getWorldToViewMatrix() * glm::scale(glm::vec3(1.0f, scaleVal, 1.0f));
	glm::mat4 modelMat = glm::mat4();
	glm::mat4 mvp = projMat * cam->getWorldToViewMatrix() * modelMat;

	glUniform3f(cameraPosUniformLoc, 
		cam->getPosition().x, 
		cam->getPosition().y, 
		cam->getPosition().z);
	glUniformMatrix4fv(modelMatUniformLocation, 1, GL_FALSE, &modelMat[0][0]);
	glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &mvp[0][0]);

	GLHelper::checkErrors("drawSkybox -- calculate matrices");

	glDrawElements(GL_TRIANGLES, activeScene->skybox->geometry->numIndices, GL_UNSIGNED_SHORT, 0);

	GLHelper::checkErrors("drawSkybox -- draw elements");

	glClear(GL_DEPTH_BUFFER_BIT);

	cam->setPosition(camPosBackup);

	GLHelper::checkErrors("drawSkybox");
}

void drawRenderable(Renderable* renderable, glm::mat4 camMat, glm::mat4 projMat)
{
	glBindVertexArray(renderable->geometry->vertexArrayObjectID);

	GLHelper::checkErrors("draw -- bind renderable VAO");

	glm::mat4 modelToWorldMatrix = renderable->getModelToWorldMatrix();
	glm::mat4 modelViewProjectionMatrix = projMat * camMat * modelToWorldMatrix;
	glUniformMatrix4fv(modelMatUniformLocation, 1, GL_FALSE, &modelToWorldMatrix[0][0]);
	glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &modelViewProjectionMatrix[0][0]);

	GLHelper::checkErrors("draw -- send renderable matrices");

	// Send material data
	setActiveMaterial(renderable->material);

	GLHelper::checkErrors("draw -- set render material");

	// Draw renderable
	glDrawElements(renderable->drawMode, renderable->geometry->numIndices, GL_UNSIGNED_SHORT, 0);

	GLHelper::checkErrors("draw -- draw renderable elements");
}

void MyGLWindow::drawShadows(Camera* cam, bool flipped)
{
	glm::vec3 inverseLight = -activeScene->getActiveLight()->getViewDirection();
	glm::mat4 depthProjMat = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
	glm::mat4 depthViewMat = glm::lookAt(inverseLight, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 depthModelMat = glm::mat4(1.0);
	glm::mat4 depthMVP = depthProjMat * depthViewMat * depthModelMat;

	GLint depthMatUL = activeProgram->getUniformLocation("depthMVP");
	//GLint depthMatUL = glGetUniformLocation(shadowProgramID, ")
	glUniformMatrix4fv(depthMatUL, 1, GL_FALSE, &depthMVP[0][0]);

	GLHelper::checkErrors("drawShadows -- send shadow matrix");

	vector<Renderable*>* renderables = activeScene->getRenderables();
	vector<Renderable*>::iterator renderablesIterator =
		renderables->begin();
	drawRenderable(activeScene->getActiveLight()->renderable, depthViewMat, depthProjMat);
	for (; renderablesIterator != renderables->end(); ++renderablesIterator)
	{
		// Send geometry data and matrices
		Renderable* renderable = *renderablesIterator;
		drawRenderable(renderable, depthViewMat, depthProjMat);
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

	glUniform3f(cameraPosUniformLoc,
		activeScene->getActiveCamera()->getPosition().x,
		activeScene->getActiveCamera()->getPosition().y,
		activeScene->getActiveCamera()->getPosition().z);

	GLHelper::checkErrors("draw -- update camera pos");

	vector<Renderable*>* renderables = activeScene->getRenderables();
	vector<Renderable*>::iterator renderablesIterator = 
		renderables->begin();
	drawRenderable(activeScene->getActiveLight()->renderable, camMat, projMat);
	for (; renderablesIterator != renderables->end(); ++renderablesIterator)
	{
		// Send geometry data and matrices
		Renderable* renderable = *renderablesIterator;
		drawRenderable(renderable, camMat, projMat);
	}

	GLHelper::checkErrors("draw");
}

void MyGLWindow::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLHelper::checkErrors("paintGL -- clear buffers");

	setActiveProgram(standardProgram);

	updateUniforms();

	modelMatUniformLocation = activeProgram->getUniformLocation("modelMatrix");
	mvpUniformLocation = activeProgram->getUniformLocation("mvp");

	GLHelper::checkErrors("paintGL -- update matrix uniforms");

	// Draw to shadow map
	if (shadowMap != NULL)
	{
		setActiveProgram(shadowProgram);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMap->getFramebufferObjectID());
		glViewport(0, 0, shadowMap->getWidth(), shadowMap->getHeight());
		drawShadows(activeScene->getShadowCamera(), false);
	}

	setActiveProgram(standardProgram);
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
}
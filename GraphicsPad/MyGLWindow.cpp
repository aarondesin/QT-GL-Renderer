#include <gl\glew.h>
#include <iostream>
#include <fstream>
#include <MyGLWindow.h>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <Vertex.h>
#include <ShapeData.h>
#include <ShapeGenerator.h>
#include <Camera.h>
#include <Light.h>
#include <Scene.h>
#include <Renderable.h>
#include <Texture.h>
#include <Cubemap.h>
#include <Framebuffer.h>
#include <map>
#include <list>
using namespace std;

#define VERTEX_BYTE_SIZE sizeof(float) * 15

const float RANDOM_PLACEMENT_OFFSET = 40.0f;

GLuint programID;

Scene scene;
Scene* activeScene;

GLuint activeTextureIndex = 0;
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

// All renderable objects
const int MAX_RENDERABLES = 100;
list<Renderable> renderables;

// All geometries
map<string, ShapeData> geometries;

// All materials
map<string, Material> materials;

// Uniform locations
GLuint cameraPosUniformLoc;
GLuint mvpUniformLocation;
GLuint modelMatUniformLocation;

GLuint ambientLightUniformLoc;
GLuint lightPosUniformLoc;
GLuint diffuseLightUniformLoc;
GLuint specularColorUniformLoc;
GLuint specularPowerUniformLoc;

GLuint diffuseTextureUniformLoc;
GLuint diffuseStrengthUniformLoc;
GLuint normalStrengthUniformLoc;
GLuint normalMapUniformLoc;
GLuint cubemapUniformLoc;
GLuint renderTextureUniformLoc;
GLuint reflectivityUniformLoc;
GLuint emissionStrengthUniformLoc;
GLuint indexOfRefractionUniformLoc;
GLuint fresnelValueUniformLoc;

GLuint getUniformLocation(const GLchar* uniformName)
{
	GLuint result = glGetUniformLocation(programID, uniformName);
	if (result < 0)
	{
		cout << "Failed to find uniform \"" << uniformName << "\"!" << endl;
	}
	return result;
}

QImage makeImage(string filename)
{
	QString filenameAsQString = QString::fromStdString(filename + ".png");
	QImage image = QGLWidget::convertToGLFormat(QImage(filenameAsQString, "png"));
	images.insert(std::pair<string, QImage>(filename, image));
	cout << "Successfully made image: " << filename << endl;
	return image;
}

Texture makeTexture(string filename)
{
	QImage image = makeImage(filename);
	glActiveTexture(GL_TEXTURE0 + activeTextureIndex);
	Texture texture = Texture(activeTextureIndex, image);
	textures.insert(std::pair<string, Texture>(filename, texture));
	activeTextureIndex++;
	cout << "Successfully made texture: " << filename << endl;
	return texture;
}

Cubemap makeCubemap(string filename)
{
	QImage left = makeImage(filename + "_Left");
	QImage right = makeImage(filename + "_Right");
	QImage up = makeImage(filename + "_Up");
	QImage down = makeImage(filename + "_Down");
	QImage back = makeImage(filename + "_Back");
	QImage forward = makeImage(filename + "_Forward");
	QImage images[] = { left, right, up, down, back, forward };
	glActiveTexture(GL_TEXTURE0 + activeTextureIndex);
	Cubemap cubemap = Cubemap(activeTextureIndex, images);
	cubemaps.insert(std::pair<string, Cubemap>(filename, cubemap));
	activeTextureIndex++;
	cout << "Successfully made cubemap: " << filename << endl;
	return cubemap;
}

Framebuffer makeFramebuffer(string name, int width, int height)
{
	Framebuffer framebuffer = Framebuffer(activeTextureIndex, width, height);
	framebuffers.insert(std::pair<string, Framebuffer>(name, framebuffer));
	activeTextureIndex++;

	if (framebuffer.status != GL_FRAMEBUFFER_COMPLETE)
		cout << "Framebuffer incomplete!" << endl;
	else cout << "Framebuffer complete." << endl;

	return framebuffer;
}

void addGeometry(ShapeData geometry)
{
	glGenBuffers(1, &geometry.vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, geometry.vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, geometry.vertexBufferSize(),
		geometry.vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &geometry.indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry.indexBufferID);
	glBufferData(GL_ARRAY_BUFFER, geometry.indexBufferSize(),
		geometry.indices, GL_STATIC_DRAW);

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
}

void setActiveDiffuseTexture(Texture diffuse)
{
	glUniform1i(diffuseTextureUniformLoc, diffuse.textureID);
}

void setActiveDiffuseTexture(string filename)
{
	Texture diffuse = textures[filename];
	setActiveDiffuseTexture(diffuse);
}

void setActiveNormalMap(Texture normalMap)
{
	glUniform1i(normalMapUniformLoc, normalMap.textureID);
}

void setActiveNormalMap(string filename)
{
	Texture normalMap = textures[filename];
	setActiveNormalMap(normalMap);
}

void setActiveCubemap(Cubemap cubemap)
{
	glUniform1i(cubemapUniformLoc, cubemap.cubemapID);
}

void setActiveCubemap(string filename)
{
	Cubemap cubemap = cubemaps[filename];
	setActiveCubemap(cubemap);
}

void setActiveFramebuffer(Framebuffer framebuffer)
{
	activeFramebuffer = &framebuffer;
}

void setActiveFramebuffer(string name)
{
	Framebuffer framebuffer = framebuffers[name];
	setActiveFramebuffer(framebuffer);
}

void setActiveMaterial(Material material)
{
	glUniform1i(diffuseTextureUniformLoc, material.diffuse.textureID);
	glUniform1f(diffuseStrengthUniformLoc, material.diffuseStrength);
	glUniform1i(normalMapUniformLoc, material.normal.textureID);
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
		activeScene->activeCamera->getPosition().x,
		activeScene->activeCamera->getPosition().y,
		activeScene->activeCamera->getPosition().z);
}

void MyGLWindow::mouseMoveEvent(QMouseEvent* e)
{
	activeScene->activeCamera->mouseUpdate(glm::vec2(e->x(), e->y()));
	repaint();
}

void addRenderable(Renderable renderable)
{
	renderables.push_back(renderable);
}

void MyGLWindow::spawnRenderable()
{
	int randomType = rand() % 2;
	Renderable renderable;
	switch (randomType)
	{
	case 0:
		renderable.geometry = geometries["sphere"];
		break;
	case 1:
		renderable.geometry = geometries["cube"];
		break;
	}

	renderable.material = materials["standard"];

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

	addRenderable(renderable);

	//sendDataToOpenGL();
	//setupVertexArrays();
}

void MyGLWindow::keyPressEvent(QKeyEvent* e)
{
	switch (e->key())
	{
		case Qt::Key::Key_W:
			activeScene->activeCamera->moveForward();
			break;
		case Qt::Key::Key_S:
			activeScene->activeCamera->moveBackward();
			break;
		case Qt::Key::Key_A:
			activeScene->activeCamera->strafeLeft();
			break;
		case Qt::Key::Key_D:
			activeScene->activeCamera->strafeRight();
			break;
		case Qt::Key::Key_Q:
			activeScene->activeCamera->moveDown();
			break;
		case Qt::Key::Key_E:
			activeScene->activeCamera->moveUp();
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

void initMaterials()
{
	Material lightMaterial;
	lightMaterial.diffuseStrength = 0.0f;
	lightMaterial.emissionStrength = 1.0f;
	lightMaterial.normalStrength = 0.0f;

	Material standardMaterial;
	standardMaterial.diffuse = textures["tri"];
	standardMaterial.diffuseStrength = 1.0f;
	standardMaterial.normal = textures["ShapeNormals"];
	standardMaterial.normalStrength = 1.0f;
	standardMaterial.specularPower = 100.0f;
	materials.insert(std::pair<string, Material>("standard", standardMaterial));

	Material flatMaterial;
	flatMaterial.diffuseStrength = 0.0f;
	flatMaterial.normalStrength = 0.0f;
	materials.insert(std::pair<string, Material>("flat", flatMaterial));

	Material reflectiveMaterial;
	reflectiveMaterial.diffuseStrength = 0.0f;
	reflectiveMaterial.normal = textures["ShapeNormals"];
	reflectiveMaterial.normalStrength = 1.0f;
	reflectiveMaterial.reflectivity = 1.0f;
	materials.insert(std::pair<string, Material>("reflective", reflectiveMaterial));
}

void initGeometries()
{
	ShapeData cube = ShapeGenerator::makeCube();
	geometries.insert(std::pair<string, ShapeData>("cube", cube));

	ShapeData sphere = ShapeGenerator::makeSphere();
	geometries.insert(std::pair<string, ShapeData>("sphere", sphere));

	ShapeData plane = ShapeGenerator::makePlane(20U);
	geometries.insert(std::pair<string, ShapeData>("plane", plane));
}

void MyGLWindow::initScene()
{
	activeScene = &scene;
	activeScene->ambientLight = glm::vec3(1.0f, 1.0f, 1.0f);
	activeScene->activeLight->position = glm::vec3(0.0f, 5.0f, 0.0f);

	Renderable skybox;
	skybox.geometry = geometries["cube"];
	skybox.scale = glm::vec3(100.0f, 100.0f, 100.0f);

	Renderable cube;
	cube.geometry = geometries["cube"];
	cube.material = materials["standard"];
	cube.position = glm::vec3(0.0f, 2.5f, 0.0f);
	cube.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	cube.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	addRenderable(cube);

	Renderable sphere;
	sphere.geometry = geometries["sphere"];
	sphere.material = materials["flat"];
	sphere.position = glm::vec3(3.0f, 2.0f, 1.0f);
	sphere.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	sphere.scale = glm::vec3(2.0f, 2.0f, 2.0f);
	addRenderable(sphere);

	Renderable plane;
	plane.geometry = geometries["plane"];
	plane.material = materials["reflective"];
	plane.position = glm::vec3(0.0f, 0.0f, 0.0f);
	plane.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	plane.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	addRenderable(plane);
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

void installShaders()
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
}

void MyGLWindow::initializeGL()
{
	// Inits OpenGL stuff
	glewInit();
	glEnable(GL_DEPTH_TEST);
	initGeometries();
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
	renderTextureUniformLoc     = getUniformLocation("renderTexture");
	reflectivityUniformLoc      = getUniformLocation("reflectivity");
	indexOfRefractionUniformLoc = getUniformLocation("indexOfRefraction");
	fresnelValueUniformLoc      = getUniformLocation("fresnel");

	// Diffuse texture
	Texture tri = makeTexture("tri");
	setActiveDiffuseTexture("tri");

	// Normal map
	Texture normal = makeTexture("ShapesNormal");
	setActiveNormalMap("ShapesNormal");

	// Cubemap
	Cubemap cubemap = makeCubemap("Skybox_Dawn");
	setActiveCubemap("Skybox_Dawn");

	// Render texture
	Framebuffer framebuffer = makeFramebuffer("RenderTexture", 1024, 1024);
	setActiveFramebuffer(framebuffer);

	// Clear to black
	glClearColor(0, 0, 0, 1);
}

void drawSkybox()
{
	glBindVertexArray(activeScene->skybox->vertexArrayObjectID);


}

void MyGLWindow::draw(Camera cam, bool flipped)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	updateUniforms();

	// View to projection
	glm::mat4 projMat = glm::perspective(
		activeScene->activeCamera->fov,
		((float)width() / (float)height()),
		activeScene->activeCamera->clipNear,
		activeScene->activeCamera->clipFar);

	drawSkybox();

	float scaleVal = flipped ? -1.0f : 1.0f;
	glm::mat4 camMat = cam.getWorldToViewMatrix() * glm::scale(glm::vec3(1.0f, scaleVal, 1.0f));

	list<Renderable>::iterator renderablesIterator = 
		renderables.begin();
	for (; renderablesIterator != renderables.end(); ++renderablesIterator)
	{
		// Send geometry data and matrices
		Renderable renderable = *renderablesIterator;
		glBindVertexArray(renderable.geometry.vertexArrayObjectID);
		glm::mat4 modelToWorldMatrix = renderable.getModelToWorldMatrix;
		glm::mat4 modelViewProjectionMatrix = projMat * camMat * modelToWorldMatrix;
		glUniformMatrix4fv(modelMatUniformLocation, 1, GL_FALSE, &modelToWorldMatrix[0][0]);
		glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &modelViewProjectionMatrix[0][0]);

		// Send material data
		setActiveMaterial(renderable.material);

		// Draw renderable
		glDrawElements(GL_TRIANGLES, renderable.geometry.numIndices, GL_UNSIGNED_SHORT, 0);
	}

		if (i == 0)
		{
			camPos = cam.getPosition();
			cam.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
			glUniform3f(cameraPosUniformLoc, camPos.x, camPos.y, camPos.z);
			float scaleVal = flipped ? 1.0f : -1.0f;
			camMat = cam.getWorldToViewMatrix() * glm::scale(glm::vec3(1.0f, scaleVal, 1.0f));
			modelMat = identity;
		}

		else
		{
			cam.setPosition(camPos);
			glUniform3f(cameraPosUniformLoc, camPos.x, camPos.y, camPos.z);
			modelMat = objects[i].getModelToWorldMatrix();
			float scaleVal = flipped ? -1.0f : 1.0f;
			camMat = cam.getWorldToViewMatrix() * glm::scale(glm::vec3(1.0f, scaleVal, 1.0f));
		}
		

		glUniform1f(selfIlumUniformLoc, objects[i].selfIlum);
		glUniform1f(useTextureUniformLoc, objects[i].useTexture);
		glUniform1f(useSkyboxUniformLoc, objects[i].useSkybox);
		glUniform1f(reflectivityUniformLoc, objects[i].reflectivity);
		glUniform1f(useNormalUniformLoc, objects[i].useNormal);
		glUniform1f(fresnelValueUniformLoc, objects[i].fresnelValue);
		glUniform1i(renderTextureUniformLoc, GL_FRAMEBUFFER);
		glDrawElements(GL_TRIANGLES, objects[i].numIndices, GL_UNSIGNED_SHORT, 0);

		if (i == 0)
		{
			glClear(GL_DEPTH_BUFFER_BIT);
		}
	}
}

void MyGLWindow::paintGL()
{
	// Render to framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, activeFramebuffer->framebufferObjectID);
	glViewport(0, 0, 1024, 1024);
	draw(activeScene->renderTargetCamera, true);

	// Render to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width(), height());
	glUniform1i(renderTextureUniformLoc, activeFramebuffer->renderTextureID);
	draw(activeScene->sceneCamera, false);
}
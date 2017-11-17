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
#include <Renderable.h>
#include <Texture.h>
#include <Cubemap.h>
#include <Framebuffer.h>
#include <map>
#include <list>
using namespace std;

#define VERTEX_BYTE_SIZE sizeof(float) * 15





const int NUM_OBJECTS = 100;
int objectCount = 5;
ShapeData* objects;
GLuint* vertexArrayObjectIDs;

GLuint cubeVertexArrayObjectID;
GLuint sphereVertexArrayObjectID;
GLuint planeVertexArrayObjectID;
GLuint lightVertexArrayObjectID;

GLuint cubeVertexBufferID;
GLuint sphereVertexBufferID;
GLuint planeVertexBufferID;
GLuint lightVertexBufferID;

GLuint cubeIndexBufferID;
GLuint sphereIndexBufferID;
GLuint planeIndexBufferID;
GLuint lightIndexBufferID;

GLuint* vertexBufferIDs;
GLuint* indexBufferIDs;
GLuint* arrayObjectIDs;
GLuint* objectByteOffsets;

GLuint renderTextureID;
GLuint frameBufferObjectID;
GLuint renderBufferObjectID;
GLuint depthBufferObjectID;


bool movingRenderTargetCamera = false;
float cameraFOV = 60.0f;
float clipNear = 0.1f;
float clipFar = 1000.0f;

GLuint mvpUniformLocation;
GLuint modelMatUniformLocation;
GLuint lightPosUniformLoc;
GLuint cameraPosUniformLoc;
GLuint specularColorUniformLoc;
GLuint diffuseLightUniformLoc;


GLuint useTextureUniformLoc;

GLuint useSkyboxUniformLoc;
GLuint reflectivityUniformLoc;
GLuint useNormalUniformLoc;
GLuint fresnelValueUniformLoc;
glm::mat4 camMat;
glm::mat4 projMat;

const float RANDOM_PLACEMENT_OFFSET = 40.0f;


glm::vec3 ambientLight = glm::vec3(0.1f, 0.1f, 0.1f);
GLuint ambientLightUniformLoc;
glm::vec3 lightPos = glm::vec3(0.0f, 5.0f, 0.0f);

glm::vec3 diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);

glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);

GLuint selfIlumUniformLoc;


GLuint programID;

Camera sceneCamera;
Camera renderTargetCamera;
Camera* activeCameraPtr;

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

// Uniform locations
GLuint diffuseTextureUniformLoc;
GLuint normalMapUniformLoc;
GLuint cubemapUniformLoc;
GLuint renderTextureUniformLoc;

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

/*void setActiveMaterial(string filename)
{
	Material material = materials[filename];
	setActiveDiffuseTexture(material.diffuse);
	setActiveNormalTexture(material.normal);
	setNormalStrength(material.normalStrength);
}*/

void MyGLWindow::updateScene()
{
	// Update light position
	glUniform3f(lightPosUniformLoc, lightPos.x, lightPos.y, lightPos.z);
	objects[4].position = lightPos;

	// Update ambient lighting strength
	glUniform3f(ambientLightUniformLoc, ambientLight.r, ambientLight.g, ambientLight.b);

	// Update diffuse lighting
	glUniform3f(diffuseLightUniformLoc, diffuseColor.r, diffuseColor.g, diffuseColor.b);

	// Update specular lighting
	glUniform3f(specularColorUniformLoc, specularColor.r, specularColor.g, specularColor.b);

	// Update camera position
	//glm::vec3 camPos = camera.getPosition();
	//glUniform3f(cameraPosUniformLoc, camPos.x, camPos.y, camPos.z);
}

void MyGLWindow::mouseMoveEvent(QMouseEvent* e)
{
	activeCameraPtr->mouseUpdate(glm::vec2(e->x(), e->y()));
	repaint();
}

void MyGLWindow::sendDataToOpenGL()
{
	for (int i = 0; i < NUM_OBJECTS; i++)
	{
		glGenBuffers(1, &vertexBufferIDs[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferIDs[i]);
		glBufferData(GL_ARRAY_BUFFER, objects[i].vertexBufferSize(), 
			objects[i].vertices, GL_STATIC_DRAW);

		glGenBuffers(1, &indexBufferIDs[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferIDs[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, objects[i].indexBufferSize(), 
			objects[i].indices, GL_STATIC_DRAW);
	}
}

void MyGLWindow::setupVertexArrays()
{
	for (int i = 0; i < NUM_OBJECTS; i++)
	{
		glGenVertexArrays(1, &vertexArrayObjectIDs[i]);
		glBindVertexArray(vertexArrayObjectIDs[i]);
		glEnableVertexAttribArray(0); // Position
		glEnableVertexAttribArray(1); // Color
		glEnableVertexAttribArray(2); // Normal
		glEnableVertexAttribArray(3); // UV
		glEnableVertexAttribArray(4); // Tangent
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferIDs[i]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, 0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (char*)(sizeof(float) * 3));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (char*)(sizeof(float) * 7));
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (char*)(sizeof(float) * 10));
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (char*)(sizeof(float) * 12));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferIDs[i]);
	}
}

void MyGLWindow::addSphere()
{
	if (objectCount >= NUM_OBJECTS) return;

	int randomType = rand() % 2;
	switch (randomType)
	{
	case 0:
		objects[objectCount] = ShapeGenerator::makeSphere(16);
		objects[objectCount].useNormal = 1.0f;
		objects[objectCount].useTexture = 1.0f;
		break;
	case 1:
		objects[objectCount] = ShapeGenerator::makeCube();
		objects[objectCount].useNormal = 1.0f;
		objects[objectCount].useTexture = 1.0f;
		break;
	}
	objects[objectCount].reflectivity = 0.0f;//rand() / RAND_MAX;
	objects[objectCount].indexOfRefraction = 1.33f;
	objects[objectCount].fresnelValue = 0.0f;//rand() / RAND_MAX;

	glm::vec3 randPosition = glm::vec3(
		rand() / (float)RAND_MAX - 0.5f, 
		rand() / (float)RAND_MAX - 0.5f, 
		rand() / (float)RAND_MAX - 0.5f) 
		* RANDOM_PLACEMENT_OFFSET;
	objects[objectCount].position = randPosition;
	float scaleValue = rand() / RAND_MAX * 5 + 2.5f;
	objects[objectCount].scale = glm::vec3(scaleValue, scaleValue, scaleValue);
	objects[objectCount].rotation = glm::vec3(
		rand() / RAND_MAX * 360.0f, 
		rand() / RAND_MAX * 360.0f, 
		rand() / RAND_MAX * 360.0f);
	objects[objectCount].selfIlum = 1.0f;
	objects[objectCount].setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	
	

	sendDataToOpenGL();
	setupVertexArrays();

	objectCount++;
}

void MyGLWindow::keyPressEvent(QKeyEvent* e)
{
	switch (e->key())
	{
		case Qt::Key::Key_W:
			activeCameraPtr->moveForward();
			break;
		case Qt::Key::Key_S:
			activeCameraPtr->moveBackward();
			break;
		case Qt::Key::Key_A:
			activeCameraPtr->strafeLeft();
			break;
		case Qt::Key::Key_D:
			activeCameraPtr->strafeRight();
			break;
		case Qt::Key::Key_Q:
			activeCameraPtr->moveDown();
			break;
		case Qt::Key::Key_E:
			activeCameraPtr->moveUp();
			break;
		case Qt::Key::Key_Space:
			addSphere();
			break;
		case Qt::Key::Key_1:
			lightPos.z -= 0.1f;
			break;
		case Qt::Key::Key_2:
			lightPos.z += 0.1f;
			break;
		case Qt::Key::Key_Left:
			lightPos.x -= 0.1f;
			break;
		case Qt::Key::Key_Right:
			lightPos.x += 0.1f;
			break;
		case Qt::Key::Key_Up:
			lightPos.y += 0.1f;
			break;
		case Qt::Key::Key_Down:
			lightPos.y -= 0.1f;
			break;
		case Qt::Key::Key_R:
			activeCameraPtr = activeCameraPtr == &sceneCamera ? 
				&renderTargetCamera : &sceneCamera;
			break;
	}
	MyGLWindow::updateGL();
}

void MyGLWindow::initScene()
{
	activeCameraPtr = &sceneCamera;

	objects = new ShapeData[NUM_OBJECTS];

	// Skybox
	objects[0] = ShapeGenerator::makeCube();
	objects[0].position = glm::vec3(0.0f, 0.0f, 0.0f);
	objects[0].rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	objects[0].scale = glm::vec3(100.0f, 100.0f, 100.0f);
	objects[0].selfIlum = 1.0f;
	objects[0].useTexture = 0.0f;
	objects[0].useNormal = 0.0f;
	objects[0].useSkybox = 1.0f;

	// Cube
	objects[1] = ShapeGenerator::makeCube();
	objects[1].position = glm::vec3(0.0f, 2.5f, 0.0f);
	objects[1].scale = glm::vec3(1.0f, 1.0f, 1.0f);
	objects[1].rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	objects[1].selfIlum = 0.0f;
	objects[1].setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	objects[1].useTexture = 1.0f;

	// Sphere
	objects[2] = ShapeGenerator::makeSphere();
	objects[2].position = glm::vec3(3.0f, 2.0f, 1.0f);
	objects[2].scale = glm::vec3(2.0f, 2.0f, 2.0f);
	objects[2].rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	objects[2].selfIlum = 0.0f;
	objects[2].setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	objects[2].useNormal = 0.0f;
	objects[2].useTexture = 1.0f;

	// Plane
	objects[3] = ShapeGenerator::makePlane(20U);
	objects[3].position = glm::vec3(0.0f, 0.0f, 0.0f);
	objects[3].scale = glm::vec3(1.0f, 1.0f, 1.0f);
	objects[3].rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	objects[3].selfIlum = 0.0f;
	objects[3].setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	objects[3].useTexture = 1.0f;
	objects[3].reflectivity = 0.9f;
	objects[3].indexOfRefraction = 1.0f;
	objects[3].fresnelValue = 0.0f;

	// Light (sphere)
	objects[4] = ShapeGenerator::makeSphere(16);
	objects[4].position = lightPos;
	objects[4].scale = glm::vec3(0.5f, 0.5f, 0.5f);
	objects[4].rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	objects[4].selfIlum = 1.0f;
	objects[4].setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	objects[4].useTexture = 0.0f;
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
	vertexArrayObjectIDs = new GLuint[NUM_OBJECTS];
	vertexBufferIDs = new GLuint[NUM_OBJECTS];
	indexBufferIDs = new GLuint[NUM_OBJECTS];

	// Inits OpenGL stuff
	glewInit();
	glEnable(GL_DEPTH_TEST);
	initScene();
	sendDataToOpenGL();
	setupVertexArrays();
	installShaders();

	// Get uniforms
	diffuseTextureUniformLoc = glGetUniformLocation(programID, "diffuseTexture");
	normalMapUniformLoc      = glGetUniformLocation(programID, "normalMap");
	cubemapUniformLoc        = glGetUniformLocation(programID, "cubemap");
	renderTextureUniformLoc  = glGetUniformLocation(programID, "renderTexture");

	modelMatUniformLocation = glGetUniformLocation(programID, "modelMatrix");
	mvpUniformLocation = glGetUniformLocation(programID, "mvp");
	lightPosUniformLoc = glGetUniformLocation(programID, "lightPos");
	ambientLightUniformLoc = glGetUniformLocation(programID, "ambientLight");
	diffuseLightUniformLoc = glGetUniformLocation(programID, "diffuseColor");
	selfIlumUniformLoc = glGetUniformLocation(programID, "selfIlum");
	cameraPosUniformLoc = glGetUniformLocation(programID, "camPos");
	specularColorUniformLoc = glGetUniformLocation(programID, "specularColor");
	
	useTextureUniformLoc = glGetUniformLocation(programID, "useTexture");
	
	
	useSkyboxUniformLoc = glGetUniformLocation(programID, "useSkybox");
	reflectivityUniformLoc = glGetUniformLocation(programID, "reflectivity");
	useNormalUniformLoc = glGetUniformLocation(programID, "useNormal");
	fresnelValueUniformLoc = glGetUniformLocation(programID, "fresnel");

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

void MyGLWindow::draw(Camera cam, bool flipped)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	updateScene();

	// View to projection
	projMat = glm::perspective(cameraFOV, ((float)width() / (float)height()), clipNear, clipFar);

	glm::mat4 mvp, modelMat;
	glm::mat4 identity = glm::mat4();
	glm::vec3 camPos;

	for (int i = 0; i < NUM_OBJECTS; i++)
	{
		glBindVertexArray(vertexArrayObjectIDs[i]);

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
		
		mvp = projMat * camMat * modelMat;

		glUniformMatrix4fv(modelMatUniformLocation, 1, GL_FALSE, &modelMat[0][0]);
		glUniformMatrix4fv(mvpUniformLocation, 1, GL_FALSE, &mvp[0][0]);
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
	draw(renderTargetCamera, true);

	// Render to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width(), height());
	glUniform1i(renderTextureUniformLoc, activeFramebuffer->renderTextureID);
	draw(sceneCamera, false);
}
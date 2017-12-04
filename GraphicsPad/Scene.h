#pragma once

#include <glm\glm.hpp>

#include "Camera.h"
#include "Cubemap.h"
#include "Renderable.h"
#include <Skybox.h>
#include <vector>

struct Scene
{
public:
	string name;
	glm::vec3 ambientLight = glm::vec3(+0.1f, +0.1f, +0.1f);
	Light* diffuseLight = NULL;
	Light* activeLight = NULL;
	Skybox* skybox = NULL;

	Scene() : sceneCamera(new Camera), renderTargetCamera (new Camera), activeCamera(sceneCamera),
		renderables (new vector<Renderable*>), diffuseLight (new Light), activeLight(diffuseLight),
		ambientLight (glm::vec3(+0.1f, +0.1f, +0.1f)) {}
	Scene(string sceneName) : Scene() { name = sceneName; }

	void addRenderable(Renderable* renderable) { renderables->push_back(renderable); }
	vector<Renderable*>* getRenderables() { return renderables; }
	
	void switchCamera()
	{
		activeCamera = (activeCamera == sceneCamera ? renderTargetCamera : sceneCamera);
	}
	Camera* getActiveCamera() { return activeCamera; }
	Camera* getRenderTargetCamera() { return renderTargetCamera; }
	~Scene()
	{
		name = "";
		delete diffuseLight;
		delete skybox;
		delete sceneCamera;
		delete renderTargetCamera;
		delete renderables;
		ambientLight = glm::vec3(0.0f, 0.0f, 0.0f);
		activeCamera = NULL;
	}
protected:
	vector<Renderable*>* renderables = NULL;
	Camera* sceneCamera = NULL;
	Camera* renderTargetCamera = NULL;
	Camera* activeCamera = sceneCamera;
};
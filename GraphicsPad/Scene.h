#pragma once

#include <glm\glm.hpp>

#include "Camera.h"
#include "Renderable.h"
#include <vector>

struct Scene
{
public:
	Scene() : activeCamera (&sceneCamera) {}
	Scene(string sceneName) : Scene() { name = sceneName; }
	string name;
	Light diffuseLight;
	Light* activeLight = &diffuseLight;
	Renderable* skybox;
	void addRenderable(Renderable* renderable) { renderables.push_back(renderable); }
	vector<Renderable*> getRenderables() { return renderables; }
	glm::vec3 ambientLight = glm::vec3(+0.1f, +0.1f, +0.1f);
	void switchCamera()
	{
		activeCamera = (activeCamera == &sceneCamera ? &renderTargetCamera : &sceneCamera);
	}
	Camera* getActiveCamera() { return activeCamera; }
	Camera getRenderTargetCamera() { return renderTargetCamera; }
protected:
	vector<Renderable*> renderables;
	Camera sceneCamera;
	Camera renderTargetCamera;
	Camera* activeCamera;
};
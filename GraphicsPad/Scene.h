#pragma once

#include <glm\glm.hpp>

#include "Camera.h"
#include "Renderable.h"
#include <vector>

struct Scene
{
public:
	Scene(string sceneName) { name = sceneName; }
	string name;
	Camera sceneCamera;
	Camera renderTargetCamera;
	Camera* activeCamera = &sceneCamera;
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
protected:
	vector<Renderable*> renderables;
};
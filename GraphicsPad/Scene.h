#include <glm\glm.hpp>

#include <Camera.h>

#pragma once
struct Scene
{
public:
	Camera sceneCamera;
	Camera renderTargetCamera;
	Camera* activeCamera = &sceneCamera;
	Light diffuseLight;
	Light* activeLight = &diffuseLight;
	glm::vec3 ambientLight = glm::vec3(+0.1f, +0.1f, +0.1f);
	void switchCamera()
	{
		activeCamera = (activeCamera == &sceneCamera ? &renderTargetCamera : &sceneCamera);
	}
};
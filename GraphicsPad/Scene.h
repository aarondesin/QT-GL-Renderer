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
	Skybox* skybox = NULL;
	void setActiveLightPosition(glm::vec3 pos) 
	{ 
		activeLight->renderable->transform.setPosition(pos);
		shadowCamera->transform.setPosition(pos);
	}
	void setActiveLightRotation(glm::vec3 rotation)
	{
		activeLight->renderable->transform.setRotation(rotation);
		shadowCamera->transform.setRotation(rotation);
	}
	Scene() : sceneCamera(new Camera), renderTargetCamera (new Camera), activeCamera(sceneCamera),
		renderables (new vector<Renderable*>), diffuseLight (new Light), activeLight(diffuseLight),
		ambientLight (glm::vec3(+0.1f, +0.1f, +0.1f)), shadowCamera(new Camera)
	{

	}

	void addRenderable(Renderable* renderable) { renderables->push_back(renderable); }
	vector<Renderable*>* getRenderables() { return renderables; }
	
	void switchCamera()
	{
		activeCamera = (activeCamera == sceneCamera ? renderTargetCamera : sceneCamera);
	}
	Camera* getActiveCamera() { return activeCamera; }
	Camera* getRenderTargetCamera() { return renderTargetCamera; }
	Camera* getShadowCamera() { return shadowCamera; }
	Light* getActiveLight() { return activeLight; }
	~Scene()
	{
		//name = "";
		delete diffuseLight;
		delete skybox;
		delete sceneCamera;
		delete renderTargetCamera;
		delete renderables;
		ambientLight = glm::vec3(0.0f, 0.0f, 0.0f);
		activeCamera = NULL;
	}
	glm::vec3 getAmbientLight() { return ambientLight; }
	void setAmbientLight(glm::vec3 light) { ambientLight = light; }
protected:
	glm::vec3 ambientLight;
	vector<Renderable*>* renderables = NULL;
	Camera* sceneCamera = NULL;
	Camera* renderTargetCamera = NULL;
	Camera* activeCamera = sceneCamera;
	Camera* shadowCamera = NULL;
	Light* diffuseLight = NULL;
	Light* activeLight = NULL;
};
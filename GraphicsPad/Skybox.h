#pragma once

#include <ShapeData.h>
#include <ShapeGenerator.h>
#include <Cubemap.h>

struct Skybox
{
public:
	Cubemap* cubemap = NULL;
	glm::mat4 getModelToWorldMatrix()
	{
		return glm::scale(glm::mat4(), scale);
	}
	ShapeData* geometry = NULL;
	~Skybox()
	{
		delete cubemap;
		delete geometry;
	}
protected:
	glm::vec3 scale = glm::vec3(1000.0f, 1000.0f, 1000.0f);
};
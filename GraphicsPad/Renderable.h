#pragma once

#include "ShapeData.h"
#include "Material.h"
#include <Transform.h>

struct Renderable
{
public:
	ShapeData* geometry = NULL;
	Material* material = NULL;
	Transform transform;
	GLenum drawMode = GL_TRIANGLES;

	Renderable() {}

	~Renderable()
	{
		delete geometry;
		delete material;
	}
private:
	glm::vec3 position;
};


#pragma once

#include <glm\glm.hpp>
#include <ShapeData.h>

struct Light
{
public:
	glm::vec3 color = glm::vec3(+1.0f, +1.0f, +1.0f);
	Renderable* renderable = NULL;
};
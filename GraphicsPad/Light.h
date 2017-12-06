#pragma once

#include <glm\glm.hpp>
#include <ShapeData.h>

struct Light
{
public:
	glm::vec3 position = glm::vec3(+0.0f, +0.0f, +0.0f);
	glm::vec3 color = glm::vec3(+1.0f, +1.0f, +1.0f);
	void setViewDirection(glm::vec3 v) { viewDir = v; }
	glm::vec3 getViewDirection() { return viewDir; }
	Renderable* renderable = NULL;
	void moveForward()  { position.z += 0.1f;  renderable->position.z += 0.f;
	}
	void moveBackward() { position.z -= 0.1f;  renderable->position.z -= 0.f;
	}
	void moveLeft()     { position.x -= 0.1f;  renderable->position.x -= 0.f;
	}
	void moveRight()    { position.x += 0.1f;  renderable->position.x += 0.f;
	}
	void moveUp()       { position.y += 0.1f; renderable->position.y += 0.f;
	}
	void moveDown() { position.y -= 0.1f; renderable->position.y -= 0.f; }
private:
	glm::vec3 viewDir = glm::vec3(0.0, 0.0, 1.0);
};
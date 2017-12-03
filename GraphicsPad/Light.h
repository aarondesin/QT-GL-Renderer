#pragma once

#include <glm\glm.hpp>
#include <ShapeData.h>

struct Light
{
public:
	glm::vec3 position = glm::vec3(+0.0f, +0.0f, +0.0f);
	ShapeData* geometry = NULL;
	glm::vec3 color = glm::vec3(+1.0f, +1.0f, +1.0f);
	void moveForward()  { position.z += 0.1f; }
	void moveBackward() { position.z -= 0.1f; }
	void moveLeft()     { position.x -= 0.1f; }
	void moveRight()    { position.x += 0.1f; }
	void moveUp()       { position.y += 0.1f; }
	void moveDown()     { position.y -= 0.1f; }
};
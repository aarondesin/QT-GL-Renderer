#pragma once
#include <glm\glm.hpp>
#include <iostream>
#include <Transform.h>
using namespace std;

class Camera
{
	const glm::vec3 UP;
	glm::vec2 oldMousePosition;
	static const float MOVEMENT_SPEED;
	glm::vec3 strafeDirection;
	glm::vec3 position;
	glm::vec3 viewDirection;
public:
	Camera();
	Transform transform;
	void mouseUpdate(const glm::vec2& newMousePosition);
	glm::mat4 getWorldToViewMatrix();
	float fov = 60.0f;
	float clipNear = 0.1f;
	float clipFar = 100.0f;
	void moveForward() 
	{ 
		position += MOVEMENT_SPEED * viewDirection; 
		transform.setPosition(position);
	}
	void moveBackward() 
	{ 
		position -= MOVEMENT_SPEED * viewDirection; 
		transform.setPosition(position);
	}
	void moveRight() 
	{ 
		position += MOVEMENT_SPEED * strafeDirection; 
		transform.setPosition(position);
	}
	void moveLeft() 
	{ 
		position -= MOVEMENT_SPEED * strafeDirection; 
		transform.setPosition(position);
	}
	void moveUp() 
	{ 
		position += MOVEMENT_SPEED * UP; 
		transform.setPosition(position);
	}
	void moveDown() 
	{ 
		position += MOVEMENT_SPEED * UP; 
		transform.setPosition(position);
	}
};

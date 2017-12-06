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
	
public:
	Camera();
	Transform transform;
	void mouseUpdate(const glm::vec2& newMousePosition);
	glm::mat4 getWorldToViewMatrix();

	void moveForward();
	void moveBackward();
	void strafeLeft();
	void strafeRight();
	void moveUp();
	void moveDown();
	float fov = 60.0f;
	float clipNear = 0.1f;
	float clipFar = 100.0f;
};

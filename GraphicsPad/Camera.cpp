#include "Camera.h"
#include <glm\gtx\transform.hpp>
#include <iostream>
using namespace std;

const float Camera::MOVEMENT_SPEED = 0.1f;

Camera::Camera() :
	viewDirection(0.663740635f, -0.492421985f, 0.562995136f),
	position(-3.11094999f, 2.23070025f, -2.86016989f)
{
	transform.setPosition(position);
	//transform.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
}

void Camera::mouseUpdate(const glm::vec2& newMousePosition)
{
	glm::vec2 mouseDelta = newMousePosition - oldMousePosition;
	if (glm::length(mouseDelta) > 50.0f)
	{
		oldMousePosition = newMousePosition;
		return;
	}
	const float ROTATIONAL_SPEED = 0.5f;
	strafeDirection = glm::cross(viewDirection, UP);
	glm::mat4 rotator = glm::rotate(-mouseDelta.x * ROTATIONAL_SPEED, UP) *
		glm::rotate(-mouseDelta.y * ROTATIONAL_SPEED, strafeDirection);

	viewDirection = glm::mat3(rotator) * viewDirection;
	transform.setViewDirection(viewDirection);
	//transform.setViewDirection(glm::mat3(rotator) * transform.forward());

	oldMousePosition = newMousePosition;

	//cout << viewDirection.x << "," << viewDirection.y << "," << viewDirection.z << endl;
}

glm::mat4 Camera::getWorldToViewMatrix()
{
	//return glm::lookAt(transform.getPosition(), transform.getPosition() + transform.forward(), UP);
	return glm::lookAt(transform.getPosition(), transform.getPosition() + viewDirection, UP);
}
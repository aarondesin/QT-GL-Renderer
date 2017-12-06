#pragma once

#include <global.h>
#include <glm\gtx\transform.hpp>

#define SQR(x) x*x

struct Transform
{
public:
	Transform() : position(glm::vec3(0.0f, 0.0f, 0.0f)), 
		rotation(glm::vec3(0.0f, 0.0f, 0.0f)), scale(glm::vec3(1.0f, 1.0f, 1.0f)) {}
	glm::mat4 getModelToWorldMatrix()
	{
		glm::mat4 translateMatrix = glm::translate(glm::mat4(), position);
		glm::mat4 rotateMatrixX = glm::rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 rotateMatrixY = glm::rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rotateMatrixZ = glm::rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 rotateMatrix = rotateMatrixZ * rotateMatrixY * rotateMatrixX;
		glm::mat4 scaleMatrix = glm::scale(scale);
		return translateMatrix * rotateMatrix * scaleMatrix;
	}
	glm::vec3 forward() { return getViewDirection(); }
	glm::vec3 right() { return glm::cross(forward(), glm::vec3(0.0f, 1.0f, 0.0f)); }
	glm::vec3 up() { return glm::cross(forward(), glm::vec3(1.0f, 0.0f, 0.0f)); }
	glm::vec3 getViewDirection()
	{
		return glm::vec3(
			glm::cos(rotation.y) * glm::cos(rotation.x),
			glm::sin(rotation.y) * glm::cos(rotation.x),
			glm::sin(rotation.x)
		);
	}
	void setViewDirection(glm::vec3 viewDir)
	{
		rotation = glm::vec3
		(
			glm::sqrt (SQR(viewDir.x) + SQR(viewDir.y)  + SQR(viewDir.z) ),
			glm::atan(viewDir.y / viewDir.x),
			glm::acos(viewDir.z)
		);
	}
	void setScale(glm::vec3 s) { scale = s; }
	void moveForward() { position += forward() * 0.1f; }
	void moveBackward() { position -= forward() * 0.1f; }
	void moveUp() { position += up() * 0.1f; }
	void moveDown() { position -= up() * 0.1f; }
	void moveRight() { position += right() * 0.1f; }
	void moveLeft() { position -= right() * 0.1f; }

	void setPosition(glm::vec3 pos) { position = pos; }
	glm::vec3 getPosition() { return position; }
	void setRotation(glm::vec3 rot) { rotation = rot; }
private:
	glm::vec3 scale;
	glm::vec3 rotation;
	glm::vec3 position;
};
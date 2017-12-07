#pragma once

#include <global.h>
#include <glm\gtx\transform.hpp>
#include<glm\gtx\euler_angles.hpp>

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
	/*glm::vec3 forward() 
	{
		return glm::vec3(
			glm::cos(rotation.y) * glm::cos(rotation.x),
			glm::sin(rotation.y) * glm::cos(rotation.x),
			glm::sin(rotation.x)
		);
	}
	glm::vec3 right() 
	{
		float offset = glm::radians(90.0f);
		return glm::vec3(
			glm::cos(rotation.y + offset) * glm::cos(rotation.x),
			glm::sin(rotation.y + offset) * glm::cos(rotation.x),
			glm::sin(rotation.x)
		);
	}
	glm::vec3 up() 
	{ 
		float offset = glm::radians(90.0f);
		return glm::vec3(
			glm::cos(rotation.y) * glm::cos(rotation.x + offset),
			glm::sin(rotation.y) * glm::cos(rotation.x + offset),
			glm::sin(rotation.x + offset)
		);
	}*/
	void setViewDirection(glm::vec3 dir)
	{
		viewDir = dir;
	}
	glm::vec3 getViewDirection() { return viewDir; }
	void setScale(glm::vec3 s) { scale = s; }
	void moveForward() { position.z += 0.1f; }
	void moveBackward() { position.z -= 0.1f; }
	void moveUp() { position.y += 0.1f; }
	void moveDown() { position.y -= 0.1f; }
	void moveRight() { position.x += 0.1f; }
	void moveLeft() { position.x -= 0.1f; }

	void setPosition(glm::vec3 pos) { position = pos; }
	glm::vec3 getPosition() { return position; }
	void setRotation(glm::vec3 rot) { rotation = rot; }
	glm::vec3 getRotation() { return rotation; }
private:
	glm::vec3 scale;
	glm::vec3 rotation;
	glm::vec3 position;
	glm::vec3 viewDir;
};
#pragma once

#include "ShapeData.h"
#include "Material.h"

struct Renderable
{
public:
	ShapeData* geometry = NULL;
	Material* material = NULL;
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;

	Renderable() :
		position(glm::vec3(0.0f, 0.0f, 0.0f)),
		rotation(glm::vec3(0.0f, 0.0f, 0.0f)),
		scale(glm::vec3(1.0f, 1.0f, 1.0f)) {}

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

	~Renderable()
	{
		delete geometry;
		delete material;
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		scale = glm::vec3(0.0f, 0.0f, 0.0f);
	}
};


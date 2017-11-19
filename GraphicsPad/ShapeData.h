#pragma once
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <Vertex.h>

struct ShapeData
{
	ShapeData() :
		vertices(0), numVertices(0),
		indices(0), numIndices(0) {}
	Vertex* vertices;
	GLuint numVertices;
	GLushort* indices;
	GLuint numIndices;
	GLuint vertexArrayObjectID;
	GLuint vertexBufferID;
	GLuint indexBufferID;
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;
	float useTexture = 1.0f;
	float renderTexture = 0.0f;
	float selfIlum = 0.0f;
	float useSkybox = 0.0f;
	float reflectivity = 0.0f;
	float useNormal = 1.0f;
	float indexOfRefraction = 1.0f;
	float fresnelValue = 0.0f;
	GLsizeiptr vertexBufferSize() const
	{
		return numVertices * sizeof(Vertex);
	}
	GLsizeiptr indexBufferSize() const
	{
		return numIndices * sizeof(GLushort);
	}
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

	void setColor(glm::vec4 color)
	{
		for (GLuint i = 0; i < numVertices; i++)
		{
			vertices[i].color = color;
		}
	}
	void cleanup()
	{
		delete [] vertices;
		delete [] indices;
		numVertices = numIndices = 0;
	}
};
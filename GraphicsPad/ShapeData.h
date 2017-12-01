#pragma once
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <Vertex.h>

typedef unsigned int GLuint;
typedef unsigned short GLushort;
typedef ptrdiff_t GLsizeiptr;

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
	GLsizeiptr vertexBufferSize() const
	{
		return numVertices * sizeof(Vertex);
	}
	GLsizeiptr indexBufferSize() const
	{
		return numIndices * sizeof(GLushort);
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
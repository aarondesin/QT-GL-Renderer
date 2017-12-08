#pragma once
#include "OBJLoader.h"
#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include <list>
#include <vector>
using namespace std;

#define ARRAY_SIZE(ARRAY) sizeof(ARRAY)/sizeof(ARRAY[0])

typedef string strArray[];
typedef char charArray[];
typedef Vertex vertexArray[];
typedef unsigned short indexArray[];

vector<string> splitStringByCharacter (string str, charArray chars)
{
	vector<string> tokens;

	int start = 0;
	for (int i = 0; i < str.length(); i++)
	{
		char c = str[i];
		for (int charIndex = 0; charIndex < ARRAY_SIZE(chars); charIndex++)
		{
			if (c == chars[charIndex])
			{
				tokens.push_back(str.substr(start, i - start));
				start = i + 1;
			}
		}
	}

	if (str.length() - start > 0)
	{
		tokens.push_back(str.substr(start, str.length() - start));
	}

	return tokens;
}

ShapeData* OBJLoader::loadOBJFile(string filename)
{
	ShapeData* result = new ShapeData();

	vector<glm::vec3> positions = vector<glm::vec3>();
	vector<glm::vec3> normals = vector<glm::vec3>();
	vector<glm::vec2> uvs = vector<glm::vec2>();

	vector<Vertex> vertices = vector<Vertex>();
	vector<unsigned short> indices = vector<unsigned short>();

	bool facesMode = false;

	ifstream in;
	in.open(filename);
	if (!in)
	{
		std::cout << "Failed to open " << filename << endl;
		return result;
	}

	string line;
	for (; getline(in, line);)
	{
		if (line == "") continue;

		vector<string> split = splitStringByCharacter(line, charArray{ ' ', '\n' });
		if (split.size() == 0)
		{
			//cout << "Invalid line." << endl;
		}

		else
		{
			const char* token = split[0].c_str();

			// Comments
			if (strcmp(token, "#") == 0)
			{
				//cout << "Comment: skipping." << endl;
			}

			// Line feed
			else if (strcmp(token, "\n") == 0)
			{
				//cout << "Line feed: skipping." << endl;
			}

			// Material reference
			else if (strcmp(token, "mtllib") == 0)
			{
				//cout << "Material reference: skipping." << endl;
			}

			// Group name
			else if (strcmp(token, "g") == 0)
			{
				//cout << "Group reference: skipping." << endl;
			}

			// Vertex
			else if (strcmp(token, "v") == 0)
			{
				glm::vec3 pos = OBJLoader::parseVec3(split, 1);
				positions.push_back(pos);
			}

			// Normal
			else if (strcmp(token, "vn") == 0)
			{
				glm::vec3 normal = OBJLoader::parseVec3(split, 1);
				normals.push_back(normal);
			}

			// UV
			else if (strcmp(token, "vt") == 0)
			{
				glm::vec2 uv = OBJLoader::parseVec2(split, 1);
				uvs.push_back(uv);
			}

			// Face
			else if (strcmp(token, "f") == 0)
			{
				int size = split.size();

				int index = vertices.size();

				for (int faceIndex = 1; faceIndex < size; faceIndex++)
				{
					string faceToken = split[faceIndex];

					vector<string> faceTokens = splitStringByCharacter(faceToken, charArray{ '/' });
					Vertex vertex;

					int positionIndex = stoi(faceTokens[0]);
					vertex.position = positions.data()[positionIndex-1];

					int uvIndex = stoi(faceTokens[1]);
					vertex.uv = uvs.data()[uvIndex-1];

					int normalIndex = stoi(faceTokens[2]);
					vertex.normal = normals.data()[normalIndex-1];

					vertex.tangent = glm::vec3(1.0f, 1.0f, 1.0f);
					vertex.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

					//indices.push_back(vertices.size());
					vertices.push_back(vertex);
				}

				indices.push_back(index);
				indices.push_back(index + 1);
				indices.push_back(index + 2);

				/*Vertex a = vertices.data()[index];
				Vertex b = vertices.data()[index + 1];
				Vertex c = vertices.data()[index + 1];

				glm::vec3 baPos = b.position - a.position;
				glm::vec3 caPos = c.position - a.position;

				glm::vec2 baUV = b.uv - a.uv;
				glm::vec2 caUV = c.uv - a.uv;

				baPos = baUV.x * 

				a.tangent = baPos;
				b.tangent = baPos;
				c.tangent = baPos;*/

				if (size > 4)
				{
					indices.push_back(index + size - 2);
					
					indices.push_back(index);
					indices.push_back(index + size - 3);
					
					

					/*Vertex a = vertices.data()[index];
					Vertex b = vertices.data()[index + size - 2];
					Vertex c = vertices.data()[index + size - 3];

					glm::vec3 baPos = b.position - a.position;
					glm::vec3 caPos = c.position - a.position;

					glm::vec2 baUV = b.uv - a.uv;
					glm::vec2 caUV = c.uv - a.uv;

					a.tangent = baPos;
					b.tangent = baPos;
					c.tangent = baPos;*/
				}
			}
		}
	}

	result->numVertices = vertices.size();
	result->numIndices = indices.size();

	vertices.shrink_to_fit();
	result->vertices = new Vertex[result->numVertices];
	std::memcpy(result->vertices, vertices.data(), sizeof(Vertex) * result->numVertices);
	
	indices.shrink_to_fit();
	result->indices = new unsigned short[result->numIndices];
	std::memcpy(result->indices, indices.data(), sizeof(unsigned short) * result->numIndices);

	std::cout << endl << "OBJLoader: Loaded model \"" << filename << "\"" << endl;
	std::cout << result->numVertices << " vertices, " << result->numIndices << " indices" << endl;
	std::cout << normals.size() << " normals, " << uvs.size() << " UVs" << endl << endl;

	return result;
}

glm::vec2 OBJLoader::parseVec2(vector<string> input, int startIndex)
{
	float x = std::stof(input[startIndex]);
	float y = std::stof(input[startIndex + 1]);
	return glm::vec2(x, y);
}

glm::vec3 OBJLoader::parseVec3(vector<string> input, int startIndex)
{
	//if (input->size() - startIndex < 3)
	//{
	//	cout << "Not a vec3!" << endl;
	//}

	float x = std::stof(input[startIndex]);
	float y = std::stof(input[startIndex + 1]);
	float z = std::stof(input[startIndex + 2]);
	return glm::vec3(x, y, z);
}

glm::vec4 OBJLoader::parseVec4(vector<string> input, int startIndex)
{
	float x = std::stof(input[startIndex]);
	float y = std::stof(input[startIndex + 1]);
	float z = std::stof(input[startIndex + 2]);
	float w = input.size() - startIndex >= 4 ? std::stof(input[startIndex + 3]) : 1.0f;
	return glm::vec4(x, y, z, w);
}
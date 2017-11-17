#include "OBJLoader.h"
#include <string>
#include <list>
#include <iostream>
using namespace std;

ShapeData loadOBJFile(string filename)
{
	ShapeData result;
	std::list<glm::vec3> positions;
	std::list<glm::vec2> uvs;
	std::list<glm::vec3> normals;
	
	return result;
}

string* splitStringByWhitespace(string str)
{
	return NULL;
}

void processLine(string line, ShapeData shape)
{
	string* split = splitStringByWhitespace(line);
	if (split->length() == 0)
	{
		cout << "Invalid line." << endl;
	}

	else
	{
		// Comments
		if (split[0] == "#" || split[0][0] == '#')
		{
			cout << "Comment: skipping." << endl;
			return;
		}

		// Line feed
		if (split[0] == "\n")
		{
			cout << "Line feed: skipping." << endl;
			return;
		}

		// Material reference
		else if (split[0] == "mtllib")
		{
			cout << "Material reference: skipping." << endl;
			return;
		}

		// Group name
		else if (split[0] == "g")
		{
			cout << "Group reference: skipping." << endl;
			return;
		}

		// Vertex
		else if (split[0] == "v")
		{
			//positions
		}

		// UV
		else if (split[0] == "vt")
		{
			//addUV(split, shape);
		}
	}
}

glm::vec2 parseVec2(string* input, int startIndex)
{
	float x = std::stof(input[startIndex]);
	float y = std::stof(input[startIndex+1]);
	return glm::vec2(x, y);
}

glm::vec3 parseVec3(string* input, int startIndex)
{
	float x = std::stof(input[startIndex]);
	float y = std::stof(input[startIndex+1]);
	float z = std::stof(input[startIndex+2]);
	return glm::vec3(x, y, z);
}

glm::vec4 parseVec4(string* input, int startIndex)
{
	float x = std::stof(input[startIndex]);
	float y = std::stof(input[startIndex+1]);
	float z = std::stof(input[startIndex+2]);
	float w = input->length() - startIndex >= 4 ? std::stof(input[startIndex+3]) : 1.0f;
	return glm::vec4(x, y, z, w);
}
#include <iostream>
#include <ShapeData.h>
#include <vector>
using namespace std;
class OBJLoader
{
public:
	static ShapeData OBJLoader::loadOBJFile(string filename);
protected:
	static glm::vec2 OBJLoader::parseVec2(vector<string> input, int startIndex);
	static glm::vec3 OBJLoader::parseVec3(vector<string> input, int startIndex);
	static glm::vec4 OBJLoader::parseVec4(vector<string> input, int startIndex);
};


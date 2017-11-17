#include <iostream>
#include <ShapeData.h>
using namespace std;
class OBJLoader
{
public:
	ShapeData loadOBJFile(string filename);
	void processLine(string line);
protected:
	glm::vec2 parseVec2(string* input, int startIndex);
	glm::vec4 parseVec4(string* input, int startIndex);
};


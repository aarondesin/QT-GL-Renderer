#pragma once

#include <Texture.h>

using namespace std;

struct Material 
{
public:
	Texture diffuse;
	float diffuseStrength = 1.0f;
	Texture normal;
	float normalStrength = 1.0f;
	float specularPower = 100.0f;
	float emissionStrength = 0.0f;
	float reflectivity = 0.0f;
	float indexOfRefraction = 1.0f;
	float fresnelValue = 0.0f;
};
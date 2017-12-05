#pragma once

#include "Texture.h"

struct Material 
{
public:
	Texture* diffuse = NULL;
	float diffuseStrength = 1.0f;
	Texture* normal = NULL;
	float normalStrength = 1.0f;
	float specularPower = 100.0f;
	float emissionStrength = 0.0f;
	float reflectivity = 0.0f;
	float indexOfRefraction = 1.0f;
	float fresnelValue = 0.0f;
	float useCubemap = 0.0f;

	Material() : diffuseStrength(1.0f),
		normalStrength(1.0f),
		specularPower(100.0f), emissionStrength(0.0f) {}

	~Material() 
	{
		delete diffuse;
		delete normal;
	}
};
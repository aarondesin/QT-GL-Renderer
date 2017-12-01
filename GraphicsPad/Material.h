#pragma once

#include "Texture.h"

struct Material 
{
public:
	Material() : diffuse(NULL), diffuseStrength(1.0f),
		normal(NULL), normalStrength(1.0f),
		specularPower(100.0f), emissionStrength(1.0f) {}
	Texture* diffuse;
	float diffuseStrength = 1.0f;
	Texture* normal;
	float normalStrength = 1.0f;
	float specularPower = 100.0f;
	float emissionStrength = 0.0f;
	float reflectivity = 0.0f;
	float indexOfRefraction = 1.0f;
	float fresnelValue = 0.0f;
};
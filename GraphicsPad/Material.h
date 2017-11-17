#include <Texture.h>

#pragma once

struct Material 
{
	Texture diffuse;
	Texture normal;
	float normalStrength = 1.0f;
};
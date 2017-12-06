#version 330 core

layout (location=0) in vec3 inPosition;

uniform mat4 depthMVP;

out vec4 throughPosition;

void main ()
{
	throughPosition = depthMVP * vec4(inPosition, 1.0);
}

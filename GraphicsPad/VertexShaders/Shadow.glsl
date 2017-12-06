#version 330 core

layout (location=0) in vec3 inPosition;

uniform mat4 depthMVP;

void main ()
{
	gl_Position = depthMVP * vec4(inPosition, 1.0);
}

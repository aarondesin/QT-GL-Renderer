#version 330 core

in vec4 throughPosition;

layout (location = 0) out float outDepth;

void main ()
{
	outDepth = gl_FragCoord.z;
}
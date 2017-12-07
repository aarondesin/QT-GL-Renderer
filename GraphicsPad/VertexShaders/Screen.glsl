#version 430

in layout(location=0) vec3 pos;

out vec4 throughPos;

void main()
{
	throughPos = vec4(pos, 1.0);
}
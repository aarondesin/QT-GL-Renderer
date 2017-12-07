#version 430

uniform sampler2D depthTexture;

in vec4 throughPos;

void main()
{
	vec2 uv = vec2 (throughPos.x, throughPos.y);
	vec4 s = texture(depthTexture, uv);
	gl_FragColor = vec4(s.z, s.z, s.z, 1.0);
}
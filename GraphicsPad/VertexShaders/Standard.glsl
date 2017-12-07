#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec4 vertexColor;
in layout(location=2) vec3 normal;
in layout(location=3) vec2 uv;
in layout(location=4) vec3 tangent;

uniform mat4 mvp;
uniform mat4 modelMatrix;
uniform vec3 ambientLight;
uniform mat4 depthBiasMVP;

out vec3 vertexPosition;
out vec3 vertexModelPosition;
out vec4 throughColor;
out vec3 throughNormal;
out vec2 throughUV;
out vec3 throughTangent;
out vec3 throughBitangent;
out vec4 throughShadowCoord;

void main()
{
	vertexModelPosition = position;
	vec4 p = vec4(position, 1.0);
	gl_Position = mvp * p;
	throughColor = vertexColor;
	throughNormal = normalize(modelMatrix * vec4(normal, 0.0)).xyz;
	vertexPosition = (modelMatrix * p).xyz;
	throughUV = uv;
	throughTangent = normalize(modelMatrix * vec4(tangent, 0.0)).xyz;
	throughBitangent = normalize(cross(throughTangent, throughNormal));
	throughShadowCoord = depthBiasMVP * vec4(vertexModelPosition, 1.0);
}
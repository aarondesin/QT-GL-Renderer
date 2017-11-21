#version 430

out vec4 outColor;

in vec4 throughColor;
in vec3 throughNormal;
in vec2 throughUV;
in vec3 throughTangent;
in vec3 throughBitangent;

const float falloffConstantCoef = 0.05;
const float falloffLinearCoef = 0.5;
const float falloffExpCoef = 0.05;

uniform sampler2D diffuseTexture;
uniform float diffuseStrength;

uniform sampler2D normalMap;
uniform float normalStrength;

uniform float emissionStrength;

uniform samplerCube cubemap;
uniform sampler2D renderTexture;

uniform vec3 ambientLight;
uniform vec3 lightPos;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float specularPower;

in vec3 vertexModelPosition;
in vec3 vertexPosition;

uniform vec3 camPos;

uniform float indexOfRefraction;

uniform mat4 modelMatrix;

uniform float reflectivity;
uniform float fresnelValue;

void main()
{
	vec4 vertexColor = throughColor;
	vec4 color = vertexColor;

	// Normal map
	vec3 sampledNormal = 2.0 * texture(normalMap, throughUV).rgb - 1.0;
	mat3 normalRotation = mat3(throughTangent, throughBitangent, throughNormal);
	mat3 tangentToObject = normalRotation;
	vec3 sampledNormalInWorldSpace = normalize(tangentToObject * sampledNormal);
	vec3 normal = normalize(throughNormal + mix(vec3(0.0, 0.0, 0.0), sampledNormalInWorldSpace, normalStrength));

	// Get texture color
	vec4 texColor = texture(diffuseTexture, throughUV);
	color = mix(color, texColor, diffuseStrength);

	// Attenuation values
	float d = distance (lightPos, vertexPosition);
	float linearFalloff = d * falloffLinearCoef;
	float expFalloff = d * d * falloffExpCoef;
	float falloff = 1.0 / (falloffConstantCoef + linearFalloff + expFalloff);

	// Ambient light
	vec4 ambient = vec4 (ambientLight, 1.0);

	// Diffuse light
	vec3 lightVec = normalize(lightPos - vertexPosition);
	float diffuseValue = clamp (dot (lightVec, normal), 0.0, 1.0);
	vec4 diffuse = vec4((diffuseValue * diffuseColor), 1.0) * falloff;

	// Specular light
	vec3 camVec = normalize (camPos - vertexPosition);
	vec3 reflectedLightVec = reflect(-lightVec, normal);
	float specularValue = pow (clamp (dot (reflectedLightVec, camVec), 0.0, 1.0), specularPower);
	vec4 specular = vec4((specularValue * specularColor), 1.0) * falloff;
	
	vec4 totalLight = mix (ambient + diffuse + specular, vec4(1.0, 1.0, 1.0, 1.0), emissionStrength);

	color = totalLight * color;

	// Reflectivity
	vec3 reflectedCamVec = normalize (reflect(-camVec, normal));
	reflectedCamVec.y = -reflectedCamVec.y;
	vec4 reflectionSample = texture(cubemap, reflectedCamVec);
	color = mix (color, reflectionSample, reflectivity);

	// Fresnel
	vec3 refractedCamVec = normalize(refract(-camVec, normal, indexOfRefraction));
	vec4 fresnelSkyboxSample = texture(cubemap, refractedCamVec);
	//vec4 fresnelLightSample = clamp(dot(-refractedCamVec, lightVec), 0.0, 1.0) * vec4(diffuseColor, 1.0);
	color = mix  (color, clamp(fresnelSkyboxSample, 0.0, 1.0), fresnelValue);

	// Final mix
	outColor = color;
}
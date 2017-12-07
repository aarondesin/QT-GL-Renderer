#version 430

out vec4 outColor;

in vec4 throughColor;
in vec3 throughNormal;
in vec2 throughUV;
in vec3 throughTangent;
in vec3 throughBitangent;
in vec4 throughShadowCoord;

const float FALLOFF_CONSTANT = 0.05;
const float FALLOFF_LINEAR = 0.5;
const float FALLOFF_EXP = 0.05;

const float SHADOW_BIAS = 0.005;

uniform sampler2D diffuseTexture;
uniform float diffuseStrength;

uniform sampler2D normalMap;
uniform float normalStrength;

uniform float emissionStrength;

uniform float useCubemap;
uniform samplerCube cubemap;
uniform sampler2D renderTexture;

uniform vec3 ambientLight = vec3 (0.0, 0.0, 0.0);
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

uniform sampler2D ambientOcclusion;
uniform float ambientOcclusionScale;

uniform sampler2D metallicSmoothness;

uniform sampler2D shadowMap;

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

	// AO
	vec4 ambientOcclusion = texture(ambientOcclusion, throughUV);

	// Get texture color
	vec4 texColor = texture(diffuseTexture, throughUV) * ambientOcclusion;
	color = mix(color, texColor, diffuseStrength);

	// Get cubemap color
	vec4 cubemapColor = texture(cubemap, normalize(vertexModelPosition) * vec3(1.0, -1.0, 1.0));
	color = mix(color, cubemapColor, useCubemap);

	// Attenuation values
	float d = distance (lightPos, vertexPosition);
	float linearFalloff = d * FALLOFF_LINEAR;
	float expFalloff = d * d * FALLOFF_EXP;
	float falloff = 1.0 / (FALLOFF_CONSTANT + linearFalloff + expFalloff);

	// Ambient light
	vec4 ambient = vec4 (ambientLight, 1.0);
	ambient = mix (ambient, ambient * ambientOcclusion, ambientOcclusionScale);

	// Diffuse light
	vec3 lightVec = normalize(lightPos - vertexPosition);
	float diffuseValue = clamp (dot (lightVec, normal), 0.0, 1.0);
	vec4 diffuse = vec4((diffuseValue * diffuseColor), 1.0) * falloff;

	// Sample MS
	vec4 metallicSmoothness = texture(metallicSmoothness, throughUV);
	float metallic = metallicSmoothness.r;
	float smoothness = metallicSmoothness.a;

	// Specular light
	vec3 camVec = normalize (camPos - vertexPosition);
	vec3 reflectedLightVec = reflect(-lightVec, normal);
	float specularValue = pow (clamp (dot (reflectedLightVec, camVec), 0.0, 1.0), specularPower * smoothness);
	vec4 specular = vec4((specularValue * specularColor), 1.0) * falloff;

	//float localBias = clamp(SHADOW_BIAS * tan(acos(dot(normal, lightVec))), 0.00, 0.01);
	float localBias = SHADOW_BIAS;

	float v = 1.0;
	if (texture(shadowMap, throughShadowCoord.xy).z < throughShadowCoord.z - localBias)
	{
		v = 0.0;
	}

	vec4 totalLight = ambient + (diffuse + specular) * v;
	vec4 unlitColor = vec4(color.r, color.g, color.b, 1.0);
	color = color * totalLight;

	// Reflectivity
	vec3 reflectedCamVec = normalize (reflect(-camVec, normal));
	reflectedCamVec.y = -reflectedCamVec.y;
	vec4 reflectionSample = texture(cubemap, reflectedCamVec);
	color = mix (color, reflectionSample, clamp((metallic * reflectivity), 0.0, 1.0));

	// Fresnel
	vec3 refractedCamVec = normalize(refract(-camVec, normal, indexOfRefraction));
	vec4 fresnelSkyboxSample = texture(cubemap, refractedCamVec);
	color = mix  (color, clamp(fresnelSkyboxSample, 0.0, 1.0), fresnelValue);

	// Emission
	color = mix (color, unlitColor, emissionStrength);

	// Final mix
	outColor = color;
	//outColor = vec4(throughShadowCoord.x, throughShadowCoord.y, throughShadowCoord.x, 1.0);
	//outColor = mix (vec4 (0.0, 0.0, 0.0, 1.0), color, v);
	//float s = texture(shadowMap, throughUV).b;
	//outColor = mix(vec4(s,s,s,1.0), color, useCubemap);
}

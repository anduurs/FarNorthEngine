#version 330 core

layout (location = 0) out vec4 fragColor;

in vs_data
{
	vec3 position;
	vec3 normal;
	vec2 textureCoord;
	vec3 cameraViewDirection;
} fs_in;

struct DirectionalLight
{
	vec3 color;
	float intensity;
	vec3 direction;
};

struct Material
{
    sampler2D diffuseMap;
    sampler2D specularMap;
    float shininess;
};

uniform DirectionalLight directionalLight;
uniform Material material;

void main()
{
    float ambientFactor = 0.1;

    vec3 n = normalize(fs_in.normal);
    vec3 lightDirection = normalize(directionalLight.direction);

    vec3 diffuseMap = texture(material.diffuseMap, fs_in.textureCoord).xyz;

    float diffuseFactor  = max(dot(n, lightDirection), 0.0);
    vec3 diffuseLight = directionalLight.color * directionalLight.intensity * diffuseFactor * diffuseMap;

	vec3 viewDirection = fs_in.cameraViewDirection;
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);

    float normalizationFactor = ((material.shininess + 2.0) / 8.0);
    float specularFactor = pow(max(dot(n, halfwayDirection), 0.0), material.shininess) * normalizationFactor;

    vec3 specularLight = directionalLight.color * directionalLight.intensity * specularFactor;

    vec4 specularMap = texture(material.specularMap, fs_in.textureCoord);

    specularLight *= specularMap.a;

    fragColor = vec4(ambientFactor + diffuseLight + specularLight, 1.0);
} 
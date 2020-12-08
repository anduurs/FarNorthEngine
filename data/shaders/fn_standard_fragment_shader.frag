#version 330 core

layout (location = 0) out vec4 fragColor;

in vs_data
{
	vec3 position;
	vec2 textureCoord;
	vec3 cameraViewPosition;
    vec3 lightDirection;
} fs_in;

struct DirectionalLight
{
	vec3 color;
	float intensity;
};

struct Material
{
    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D normalMap;
    float shininess;
};

uniform DirectionalLight directionalLight;
uniform Material material;

void main()
{
    float ambientFactor = 0.05;

    //transform rgb normal from range [0,1] to a direction vector in range [-1, 1]
    vec3 normal = normalize(texture(material.normalMap, fs_in.textureCoord).rgb * 2.0 - 1.0);

    vec3 lightDirection = normalize(fs_in.lightDirection);

    vec3 diffuseMap = texture(material.diffuseMap, fs_in.textureCoord).rgb;

    float diffuseFactor  = max(dot(normal, lightDirection), 0.0);
    vec3 diffuseLight = directionalLight.color * directionalLight.intensity * diffuseFactor * diffuseMap;

	vec3 viewDirection = normalize(fs_in.cameraViewPosition - fs_in.position);
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);

    float normalizationFactor = ((material.shininess + 2.0) / 8.0);
    float specularFactor = pow(max(dot(normal, halfwayDirection), 0.0), material.shininess) * normalizationFactor;

    vec3 specularLight = directionalLight.color * directionalLight.intensity * specularFactor;

    vec4 specularMap = texture(material.specularMap, fs_in.textureCoord);

    specularLight *= specularMap.rgb;
    fragColor = vec4(ambientFactor + diffuseLight + specularLight, 1.0);
} 
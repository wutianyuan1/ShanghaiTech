#version 460 core

in vec3 Normal;
in vec3 FragPos;
in vec3 Color;

out vec4 FragColor;

uniform vec3 cameraPos;
uniform float specular;
uniform float shininess;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
uniform int lights_size;
uniform Light lights[32];

struct SunLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform int sunLights_size;
uniform SunLight sunLights[32];

struct SpotLight {
	vec3 position;
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

    float constant;
    float linear;
    float quadratic;

	float cutOff;
	float outerCutOff;
};
uniform int spotLights_size;
uniform SpotLight spotLights[32];

vec3 calcSunLight(SunLight light, vec3 viewDir);
vec3 calcPointLight(Light light, vec3 viewDir);
vec3 calcSpotLight(SpotLight light, vec3 viewDir);

void main()
{
    vec3 viewDir = normalize(cameraPos - FragPos);

    vec3 result = vec3(0);

    for (int i = 0; i < sunLights_size; i++)
    {
    	result += calcSunLight(sunLights[i], viewDir);
    }

    for (int i = 0; i < lights_size; i++)
    {
    	result += calcPointLight(lights[i], viewDir);
    }

    for (int i = 0; i < spotLights_size; i++)
    {
    	result += calcSpotLight(spotLights[i], viewDir);
    }

    FragColor = vec4(result * Color, 1.0);
}


vec3 calcSunLight(SunLight light, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
    
    // Diffuse
	vec3 diffuse = light.diffuse * max(dot(Normal, lightDir), 0.0);
	// Specular
	vec3 reflectDir = reflect(-lightDir, Normal);
	vec3 specular = light.specular * pow(max(dot(viewDir, reflectDir), 0.0f), shininess);

	return light.ambient + diffuse + specular;
}


vec3 calcPointLight(Light light, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - FragPos);

    // Diffuse
    vec3 diffuse = light.diffuse * max(dot(Normal, lightDir), 0.0);

    // Specular
    vec3 reflectDir = reflect(-lightDir, Normal);
    vec3 specular = light.specular * pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    // Attenuation
    float dis = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dis + light.quadratic * (dis * dis));  

    return (light.ambient + diffuse + specular) * attenuation;
}


vec3 calcSpotLight(SpotLight light, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - FragPos);

    // Diffuse
    vec3 diffuse = light.diffuse * max(dot(Normal, lightDir), 0.0);

    // Specular
    vec3 reflectDir = reflect(-lightDir, Normal);
    vec3 specular = light.specular * pow(max(dot(viewDir, reflectDir), 0.0), shininess);

	// Attenuation
	float dis = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * dis + light.quadratic * (dis * dis));

    // Cut Off
    float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    return (light.ambient + (diffuse + specular) * intensity) * attenuation;
}
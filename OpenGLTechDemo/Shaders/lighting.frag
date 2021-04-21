#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec2 texCoord;
in vec3 Normal;

struct Material {	
	float specularIntensity;
    float shininess;
}; 

struct Light
{
	vec3 colour;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct DirLight {
	Light base;

    vec3 direction;
};

struct PointLight {
	Light base;

    vec3 position;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
	PointLight base;

    vec3 direction;
    float cutOff;
    float outerCutOff;     
};

#define NR_POINT_LIGHTS 4

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform sampler2D texture1;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{    

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);    

    // phase 1: directional lighting
    vec3 finalColor = CalcDirLight(dirLight, norm, viewDir);

    // phase 2: point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        finalColor += CalcPointLight(pointLights[i], norm, FragPos, viewDir);  

    // phase 3: spot light
    finalColor += CalcSpotLight(spotLight, norm, FragPos, viewDir);    

    FragColor = texture(texture1, texCoord) * vec4(finalColor, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // combine results
    vec3 ambient = light.base.ambient;
    vec3 diffuse = light.base.diffuse * diff;
    vec3 specular = light.base.specular * spec;

	return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	// attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

	// combine results
    vec3 ambient = light.base.ambient;
    vec3 diffuse = light.base.diffuse * diff;
    vec3 specular = light.base.specular * spec;

	ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

	return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.base.position - fragPos);

	// diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

	// specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	// attenuation
    float distance = length(light.base.position - fragPos);
    float attenuation = 1.0 / (light.base.constant + light.base.linear * distance + light.base.quadratic * (distance * distance));    

	// spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	// combine results
    vec3 ambient = light.base.base.ambient;
    vec3 diffuse = light.base.base.diffuse * diff;
    vec3 specular = light.base.base.specular * spec;

	ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

	return (ambient + diffuse + specular);
}
#version 330 core

out vec4 FragColor;

in VS_OUT {
	vec3 FragPos;
	vec2 TexCoords;
	vec3 Normal;
	vec4 FragPosLightSpace;
} fs_in;

struct Material {	
	float specularIntensity;
    float shininess;
}; 

struct Light
{
	vec3 color;
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
#define NR_SPOT_LIGHTS 1

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform Material material;
uniform sampler2D floor;
uniform bool blinnPhong;
uniform sampler2D shadowMap;


// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float ShadowCalculation(vec4 fragPosLightSpace);

void main()
{    
    vec3 norm = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);    

    // phase 1: directional lighting
    vec3 finalColor = CalcDirLight(dirLight, norm, viewDir);

    // phase 2: point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        finalColor += CalcPointLight(pointLights[i], norm, fs_in.FragPos, viewDir);  

    // phase 3: spot light
	for(int i = 0; i < NR_SPOT_LIGHTS; i++)
		finalColor += CalcSpotLight(spotLights[i], norm, fs_in.FragPos, viewDir);  
		
    FragColor = texture(floor, fs_in.TexCoords) * vec4(finalColor, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
	float spec = 0;
	if(blinnPhong){
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	} else {
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	}

    // combine results
    vec3 ambient = light.base.ambient;
    vec3 diffuse = light.base.diffuse * diff;
    vec3 specular = light.base.specular * spec;

	// calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));

	return lighting * light.base.color;
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
	float spec = 0;
    if(blinnPhong){
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess * 4);
	} else {
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	}

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

	return (ambient + diffuse + specular) * light.base.color;
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.base.position - fragPos);

	// diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

	// specular shading
	float spec = 0;
    if(blinnPhong){
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess * 4);
	} else {
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	}

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

	return (ambient + diffuse + specular) * light.base.base.color;
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	// transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 

	// get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

	// calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

	// check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}
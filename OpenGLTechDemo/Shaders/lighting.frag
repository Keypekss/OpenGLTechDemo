#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec2 texCoord;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

uniform sampler2D texture1;

void main ()
{
	// ambient light
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	// diffuse light
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diffStrength = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = diffStrength * lightColor;

	// specular light
	float specularIntensity = 0.5f;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float specStrength = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	vec3 specular = specStrength * lightColor;


	vec3 finalColor = (ambient + diffuse + specular) * objectColor;
	FragColor = vec4(finalColor, 1.0f) * texture(texture1, texCoord);
}
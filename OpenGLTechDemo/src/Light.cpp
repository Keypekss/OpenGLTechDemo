#include "Light.h"

Light::Light(Shader &shader)
{
	this->shader = shader;
	Color		= glm::vec3(1.0f);
	Ambient		= glm::vec3(1.0f);
	Diffuse		= glm::vec3(0.0f);
}

Light::Light(Shader &shader, glm::vec3 color, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
{
	this->shader = shader;
	Color		= color;
	Ambient		= ambient;
	Diffuse		= diffuse;
	Specular	= specular;
}
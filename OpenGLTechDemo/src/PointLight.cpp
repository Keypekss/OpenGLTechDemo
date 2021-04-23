#include <string>
#include "PointLight.h"


PointLight::PointLight(Shader &shader) : Light(shader)
{
	Position = glm::vec3(0.0f, 0.0f, 0.0f);
	Constant = 1.0f;
	Linear	 = 0.0f;
	Quadratic = 0.0f;
}

PointLight::PointLight(Shader &shader, 
	glm::vec3 color,
	glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
	glm::vec3 position,
	float constant, float linear, float quadratic) : Light(shader, color, ambient, diffuse, specular)
{
	Position = position;
	Constant = constant;
	Linear   = linear;
	Quadratic = quadratic;
}

void PointLight::UseLight(unsigned int id)
{	
	shader.setVec3("pointLights[" + std::to_string(id) + ']' + ".base.color", Color);
	shader.setVec3("pointLights[" + std::to_string(id) + ']' + ".base.ambient", Ambient);
	shader.setVec3("pointLights[" + std::to_string(id) + ']' + ".base.diffuse", Diffuse);
	shader.setVec3("pointLights[" + std::to_string(id) + ']' + ".base.specular", Specular);
	shader.setVec3("pointLights[" + std::to_string(id) + ']' + ".position", Position);
	shader.setFloat("pointLights[" + std::to_string(id) + ']' + ".constant", Constant);
	shader.setFloat("pointLights[" + std::to_string(id) + ']' + ".linear", Linear);
	shader.setFloat("pointLights[" + std::to_string(id) + ']' + ".quadratic", Quadratic);
}

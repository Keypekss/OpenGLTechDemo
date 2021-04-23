#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(Shader &shader) : Light(shader)
{
	Direction = glm::vec3(0.0f, -1.0f, 0.0f);
}

DirectionalLight::DirectionalLight(
	Shader &shader,
	glm::vec3 color,
	glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
	glm::vec3 direction) : Light(shader, color, ambient, diffuse, specular)
{
	Direction = direction;
}

void DirectionalLight::UseLight()
{
	shader.setVec3("dirLight.base.color", Color);
	shader.setVec3("dirLight.base.ambient", Ambient);
	shader.setVec3("dirLight.base.diffuse", Diffuse);
	shader.setVec3("dirLight.base.specular", Specular);
	shader.setVec3("dirLight.direction", Direction);
}
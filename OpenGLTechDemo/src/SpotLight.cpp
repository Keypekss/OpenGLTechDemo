#include "SpotLight.h"
SpotLight::SpotLight(Shader &shader) : PointLight(shader)
{
	Direction = glm::vec3(0.0f, -1.0f, 0.0f);
	Edge = 0.0f;
	ProcEdge = cosf(glm::radians(Edge));
}

SpotLight::SpotLight(Shader &shader,
	glm::vec3 color,
	glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
	glm::vec3 position,
	glm::vec3 direction,
	float constant, float linear, float quadratic,
	float edge, float procEdge) : PointLight(shader, color, ambient, diffuse, specular, position, constant, linear, quadratic)
{
	Direction = glm::normalize(direction);

	Edge = edge;
	ProcEdge = procEdge;
}

void SpotLight::UseLight(unsigned int id)
{
	shader.setVec3 ("spotLights[" + std::to_string(id) + ']' + ".base.base.color", Color);
	shader.setVec3 ("spotLights[" + std::to_string(id) + ']' + ".base.base.ambient", Ambient);
	shader.setVec3 ("spotLights[" + std::to_string(id) + ']' + ".base.base.diffuse", Diffuse);
	shader.setVec3 ("spotLights[" + std::to_string(id) + ']' + ".base.base.specular", Specular);
	shader.setVec3 ("spotLights[" + std::to_string(id) + ']' + ".base.position", Position);
	shader.setFloat("spotLights[" + std::to_string(id) + ']' + ".base.constant", Constant);
	shader.setFloat("spotLights[" + std::to_string(id) + ']' + ".base.linear", Linear);
	shader.setFloat("spotLights[" + std::to_string(id) + ']' + ".base.quadratic", Quadratic);
	shader.setVec3 ("spotLights[" + std::to_string(id) + ']' + ".direction", Direction);
	shader.setFloat("spotLights[" + std::to_string(id) + ']' + ".cutOff", Edge);
	shader.setFloat("spotLights[" + std::to_string(id) + ']' + ".outerCutOff", ProcEdge);
}

void SpotLight::SetFlash(glm::vec3 pos, glm::vec3 dir)
{
	Position = pos;
	Direction = dir;
}

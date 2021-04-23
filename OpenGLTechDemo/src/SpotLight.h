#pragma once
#include "PointLight.h"
#include "Shader.h"
class SpotLight :
	public PointLight {
public:
	SpotLight(Shader &shader);

	SpotLight(Shader &shader,
		glm::vec3 color,
		glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
		glm::vec3 position,
		glm::vec3 direction,
		float con, float lin, float exp,
		float edg, float procEdge);

	void UseLight(unsigned int id);

	void SetFlash(glm::vec3 pos, glm::vec3 dir);

	~SpotLight() = default;

private:
	glm::vec3 Direction;

	GLfloat Edge, ProcEdge;
};



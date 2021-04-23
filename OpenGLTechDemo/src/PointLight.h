#pragma once
#include "Light.h"
#include "Shader.h"

class PointLight : public Light {
public:
	PointLight(Shader &shader);
	PointLight(Shader &shader,
		glm::vec3 color,
		glm::vec3 aIntensity, glm::vec3 dIntensity, glm::vec3 specular,
		glm::vec3 position,
		float constant, float linear, float quadratic);

	void UseLight(unsigned int id);

	~PointLight() = default;

protected:
	glm::vec3 Position;

	float Constant, Linear, Quadratic;
};


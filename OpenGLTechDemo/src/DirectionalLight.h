#pragma once
#include "Light.h"
#include "Shader.h"

class DirectionalLight : public Light {
public:
	DirectionalLight(Shader &shader);
	DirectionalLight(
		Shader &shader,
		glm::vec3 color,
		glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
		glm::vec3 direction
	);

	void UseLight();

	~DirectionalLight() = default;

private:
	glm::vec3 Direction;
};


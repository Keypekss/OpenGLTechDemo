#pragma once
#include <glad\glad.h>
#include <glm\glm.hpp>

#include "Shader.h"

class Light {
public:
	Light(Shader &shader);
	Light(Shader &shader, glm::vec3 color, glm::vec3 ambient, glm::vec3 diffuse , glm::vec3 specular);

	~Light() = default;

protected:
	Shader shader;
	glm::vec3 Color;
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
};

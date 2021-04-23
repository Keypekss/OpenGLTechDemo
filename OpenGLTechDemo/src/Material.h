#pragma once

#include <glad/glad.h>

#include "Shader.h"

class Material {
public:	
	Material() = delete;
	Material(Shader &shader);
	Material(Shader &shader, float sIntensity, float shine);

	void UseMaterial(float specularIntensity, float shininess);

	~Material();

private:
	Shader shader;
	float SpecularIntensity;
	float Shininess;
};

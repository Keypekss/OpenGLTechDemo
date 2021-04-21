#pragma once

#include <glad/glad.h>

class Material {
public:
	float SpecularIntensity;
	float Shininess;

	Material();
	Material(float sIntensity, float shine);

	void UseMaterial(unsigned int specularIntensityLocation, unsigned int shininessLocation);

	~Material();

private:

};

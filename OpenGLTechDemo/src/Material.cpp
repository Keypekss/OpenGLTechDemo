#include "Material.h"

Material::Material()
{
	SpecularIntensity = 0.0f;
	Shininess = 0.0f;
}

Material::Material(float sIntensity, float shine)
{
	SpecularIntensity = sIntensity;
	Shininess = shine;
}

void Material::UseMaterial(unsigned int specularIntensityLocation, unsigned int shininessLocation)
{
	glUniform1f(specularIntensityLocation, SpecularIntensity);
	glUniform1f(shininessLocation, Shininess);
}

Material::~Material() = default;


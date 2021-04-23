#include "Material.h"

Material::Material(Shader &shader)
{
	this->shader = shader;
	SpecularIntensity = 0.0f;
	Shininess = 0.0f;
}

Material::Material(Shader &shader, float sIntensity, float shine)
{
	this->shader = shader;
	SpecularIntensity = sIntensity;
	Shininess = shine;
}

void Material::UseMaterial(float specularIntensity, float shininess)
{
	this->shader.setFloat("material.specularIntensity", specularIntensity);
	this->shader.setFloat("material.shininess", shininess);
}

Material::~Material() = default;


#include "Material.h"

Material::Material()
{
	specularIntensity = 0.0f;
	shininess = 0.0f;
}


Material::Material(GLfloat sIntensity, GLfloat shine)
{
	specularIntensity = sIntensity;
	shininess = shine;
}


void Material::UseMaterial(GLuint specularIntensityLocation, GLuint shinessLocation)
{
	glUniform1f(specularIntensityLocation, specularIntensity);
	glUniform1f(shinessLocation, shininess);
}


Material::~Material()
{
}

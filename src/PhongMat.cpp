
#include "PhongMat.hpp"

PhongMat::PhongMat(const std::string& name)
	: Material(getShaderProgram(), name) {

	setAmbient(glm::vec3(0.05f, 0.05f, 0.05f));
	setSpecular(glm::vec3(0.5f));
	setShininess(1000.0f);
	setDiffuse(glm::vec3(1.0f, 1.0f, 1.0f));

	if (defaultDiffuseMap == nullptr) {
		defaultDiffuseMap = Texture::createColorTexture(2, 2, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		defaultNormalMap = Texture::createColorTexture(2, 2, glm::vec4(0.5f, 0.5f, 1.0f, 1.0f));
		defaultSpecularMap = Texture::createColorTexture(2, 2, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		defaultShininessMap = Texture::createColorTexture(2, 2, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	}

	setDiffuseMap(defaultDiffuseMap);
	setNormalMap(defaultNormalMap);
	setSpecularMap(defaultSpecularMap);
	setShininessMap(defaultShininessMap);
}

PhongMat::PhongMat(const PhongMat& other)
	: Material(other) {

}

void PhongMat::setDiffuseMap(const std::shared_ptr<Texture>& texture) {
	setTexture("diffuseMap", texture);
}

void PhongMat::setNormalMap(const std::shared_ptr<Texture>& texture) {
	setTexture("normalMap", texture);
}

void PhongMat::setSpecularMap(const std::shared_ptr<Texture>& texture) {
	setTexture("specularMap", texture);
}

void PhongMat::setShininessMap(const std::shared_ptr<Texture>& texture) {
	setTexture("shininessMap", texture);
}

void PhongMat::setDiffuse(const glm::vec3& diffuse) {
	setVec3("diffuse", diffuse);
}

void PhongMat::setSpecular(const glm::vec3& specular) {
	setVec3("specular", specular);
}

void PhongMat::setShininess(float shininess) {
	setFloat("shininess", shininess);
}

std::shared_ptr<ShaderProgram> PhongMat::getShaderProgram() {
	if (phongShaderProgram == nullptr) {
		Shader<GL_VERTEX_SHADER> vertexShader("assets/shaders/vertex.vert", true);
		Shader<GL_FRAGMENT_SHADER> fragmentShader("assets/shaders/phong.frag", true);
		phongShaderProgram = std::make_shared<ShaderProgram>(vertexShader, fragmentShader);
	}
	return phongShaderProgram;
}

void PhongMat::setAmbient(const glm::vec3& ambient) {
	setVec3("ambient", ambient);
}

std::shared_ptr<ShaderProgram> PhongMat::phongShaderProgram = nullptr;

std::shared_ptr<Texture> PhongMat::defaultDiffuseMap = nullptr;
std::shared_ptr<Texture> PhongMat::defaultNormalMap = nullptr;
std::shared_ptr<Texture> PhongMat::defaultSpecularMap = nullptr;
std::shared_ptr<Texture> PhongMat::defaultShininessMap = nullptr;

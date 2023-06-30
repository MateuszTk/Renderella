#pragma once
#include "Material.hpp"

class PhongMat : public Material {
public:
	PhongMat(const std::string& name = "")
		: Material(getShaderProgram(), name) {

		setVec3("lightPos", glm::vec3(0.0f, 3.0f, 0.0f));
		setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		setVec3("viewPos", glm::vec3(0.0f, 0.0f, 0.0f));

		setAmbient(glm::vec3(0.1f, 0.1f, 0.1f));
		setSpecular(0.5f);
		setShininess(32.0f);
	}

	PhongMat(const PhongMat& other)
		: Material(other) {

	}

	void setSpecular(float specular) {
		setFloat("specular", specular);
	}

	void setShininess(float shininess) {
		setFloat("shininess", shininess);
	}

	static std::shared_ptr<ShaderProgram> getShaderProgram() {
		if (phongShaderProgram == nullptr) {
			Shader<GL_VERTEX_SHADER> vertexShader("assets/shaders/vertex.vert", true);
			Shader<GL_FRAGMENT_SHADER> fragmentShader("assets/shaders/phong.frag", true);
			phongShaderProgram = std::make_shared<ShaderProgram>(vertexShader, fragmentShader);
		}
		return phongShaderProgram;
	}

private:
	static std::shared_ptr<ShaderProgram> phongShaderProgram;

	void setAmbient(const glm::vec3& ambient) {
		setVec3("ambient", ambient);
	}
};

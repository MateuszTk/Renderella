#pragma once
#include "Material.hpp"

class PhongMat : public Material {
public:
	PhongMat(const std::string& name = "")
		: Material(getShaderProgram(), name) {

		this->includeLightsUniforms = true;
		this->includeCameraPosUniform = true;
		setVec3("viewPos", glm::vec3(0.0f, 0.0f, 0.0f));

		setAmbient(glm::vec3(0.05f, 0.05f, 0.05f));
		setSpecular(0.2f);
		setShininess(32.0f);
	}

	PhongMat(const PhongMat& other)
		: Material(other) {

	}

	void addDiffuseMap(const std::shared_ptr<Texture>& texture) {
		addTexture("diffuseMap", texture);
	}

	void addNormalMap(const std::shared_ptr<Texture>& texture) {
		addTexture("normalMap", texture);
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

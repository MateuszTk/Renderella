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
		setSpecular(0.5f);
		setShininess(32.0f);
		setNormalMap(Texture::createColorTexture(2, 2, glm::vec4(0.5f, 0.5f, 1.0f, 1.0f)));
	}

	PhongMat(const PhongMat& other)
		: Material(other) {

	}

	void setDiffuseMap(const std::shared_ptr<Texture>& texture) {
		setTexture("diffuseMap", texture);
	}

	void setNormalMap(const std::shared_ptr<Texture>& texture) {
		setTexture("normalMap", texture);
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

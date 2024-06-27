#pragma once
#include "Material.hpp"

class PhongMat : public Material {
public:
	PhongMat(const std::string& name = "")
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

	PhongMat(const PhongMat& other)
		: Material(other) {

	}

	void setDiffuseMap(const std::shared_ptr<Texture>& texture) {
		setTexture("diffuseMap", texture);
	}

	void setNormalMap(const std::shared_ptr<Texture>& texture) {
		setTexture("normalMap", texture);
	}

	void setSpecularMap(const std::shared_ptr<Texture>& texture) {
		setTexture("specularMap", texture);
	}

	void setShininessMap(const std::shared_ptr<Texture>& texture) {
		setTexture("shininessMap", texture);
	}

	void setDiffuse(const glm::vec3& diffuse) {
		setVec3("diffuse", diffuse);
	}

	void setSpecular(const glm::vec3& specular) {
		setVec3("specular", specular);
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

	static std::shared_ptr<Texture> defaultDiffuseMap;
	static std::shared_ptr<Texture> defaultNormalMap;
	static std::shared_ptr<Texture> defaultSpecularMap;
	static std::shared_ptr<Texture> defaultShininessMap;
};

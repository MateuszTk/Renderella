#pragma once
#include <vector>
#include <memory>
#include "ShaderProgram.hpp"
#include "Texture.hpp"

class Material {
public:
	Material(const std::shared_ptr<ShaderProgram>& shaderProgram) : shaderProgram(shaderProgram) {

	}

	Material(const Material& other) : shaderProgram(other.shaderProgram), textures(other.textures) {

	}

	Material() {
		shaderProgram = nullptr;
	}

	void setShaderProgram(const std::shared_ptr<ShaderProgram>& shaderProgram) {
		this->shaderProgram = shaderProgram;
	}

	std::shared_ptr<ShaderProgram> getShaderProgram() const {
		return shaderProgram;
	}

	void addTexture(std::string name, const std::shared_ptr<Texture>& texture) {
		textures.push_back(make_pair(name, texture));
	}

	void addVec3(std::string name, glm::vec3 vec) {
		vec3s.push_back(make_pair(name, vec));
	}

	void use() {
		shaderProgram->use();
		for (unsigned int i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			textures[i].second->bind();
			shaderProgram->setInt(textures[i].first, i);
		}
		for (unsigned int i = 0; i < vec3s.size(); i++) {
			shaderProgram->setVec3(vec3s[i].first, vec3s[i].second);
		}
	}

private:
	std::shared_ptr<ShaderProgram> shaderProgram;
	std::vector<std::pair<std::string, std::shared_ptr<Texture>>> textures;
	std::vector<std::pair<std::string, glm::vec3>> vec3s;
};

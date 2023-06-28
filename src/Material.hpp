#pragma once
#include <vector>
#include "ShaderProgram.hpp"
#include "Texture.hpp"

class Material {
public:
	Material(const std::shared_ptr<ShaderProgram>& shaderProgram) : shaderProgram(shaderProgram) {

	}

	Material(const Material& other) : shaderProgram(other.shaderProgram), textures(other.textures) {

	}

	std::shared_ptr<ShaderProgram> getShaderProgram() const {
		return shaderProgram;
	}

	void addTexture(std::string name, const std::shared_ptr<Texture>& texture) {
		textures.push_back(make_pair(name, texture));
	}

	void use() {
		shaderProgram->use();
		for (unsigned int i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			textures[i].second->bind();
			shaderProgram->setInt(textures[i].first, i);
		}
	}

private:
	std::shared_ptr<ShaderProgram> shaderProgram;
	std::vector<std::pair<std::string, std::shared_ptr<Texture>>> textures;
};

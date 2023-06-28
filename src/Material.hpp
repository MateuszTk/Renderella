#pragma once
#include <vector>
#include "ShaderProgram.hpp"
#include "Texture.hpp"

class Material {
public:
	Material(ShaderProgram* shaderProgram) : shaderProgram(shaderProgram) {

	}

	void addTexture(std::string name, Texture* texture) {
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
	ShaderProgram* shaderProgram;
	std::vector<std::pair<std::string, Texture*>> textures;
};

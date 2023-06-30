#pragma once
#include <vector>
#include <memory>
#include "ShaderProgram.hpp"
#include "Texture.hpp"
#include <map>

class Material {
public:
	Material(const std::shared_ptr<ShaderProgram>& shaderProgram, const std::string& name = "")
		: shaderProgram(shaderProgram), name(name) {

	}

	Material(const Material& other) 
		: shaderProgram(other.shaderProgram), textures(other.textures), vec3s(other.vec3s), floats(other.floats), name(other.name) {

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

	void addTexture(const std::string& name, const std::shared_ptr<Texture>& texture) {
		textures.push_back(make_pair(name, texture));
	}

	void setVec3(const std::string& name, const glm::vec3& vec) {
		vec3s[name] = vec;
	}

	void setFloat(const std::string& name, float value) {
		floats[name] = value;
	}

	void setName(std::string name) {
		this->name = name;
	}

	std::string getName() const {
		return name;
	}

	void use() {
		shaderProgram->use();
		for (unsigned int i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			textures[i].second->bind();
			shaderProgram->setInt(textures[i].first, i);
		}
		for (auto& vect : vec3s) {
			shaderProgram->setVec3(vect.first, vect.second);
		}
		for (auto& flt : floats) {
			shaderProgram->setFloat(flt.first, flt.second);
		}
	}

protected:
	std::shared_ptr<ShaderProgram> shaderProgram;
	std::vector<std::pair<std::string, std::shared_ptr<Texture>>> textures;
	std::map<std::string, glm::vec3> vec3s;
	std::map<std::string, float> floats;
	std::string name;
};

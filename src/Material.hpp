#pragma once
#include <vector>
#include <memory>
#include "ShaderProgram.hpp"
#include "Texture.hpp"
#include <unordered_map>
#include "Light.hpp"
#include "Camera.hpp"

class Material {
public:
	Material(const std::shared_ptr<ShaderProgram>& shaderProgram, const std::string& name = "")
		: shaderProgram(shaderProgram), name(name) {
		lightPosLoc = UniLocation("lightPos", shaderProgram);
		lightColorLoc = UniLocation("lightColor", shaderProgram);
		lightDirLoc = UniLocation("lightDir", shaderProgram);
		lightSpaceMatrixLoc = UniLocation("lightSpaceMatrix", shaderProgram);
		usedLightsLoc = UniLocation("usedLights", shaderProgram);
		viewPosLoc = UniLocation("viewPos", shaderProgram);
		viewDirLoc = UniLocation("viewDir", shaderProgram);
	}

	Material(const Material& other) 
		: shaderProgram(other.shaderProgram), textures(other.textures), vec3s(other.vec3s), floats(other.floats), name(other.name), includeLightsUniforms(other.includeLightsUniforms), includeCameraPosUniform(other.includeCameraPosUniform),
		lightPosLoc(other.lightPosLoc), lightColorLoc(other.lightColorLoc), 
		lightDirLoc(other.lightDirLoc), lightSpaceMatrixLoc(other.lightSpaceMatrixLoc),
		usedLightsLoc(other.usedLightsLoc), viewPosLoc(other.viewPosLoc), viewDirLoc(other.viewDirLoc) {

	}

	Material() {
		shaderProgram = nullptr;
	}

	void setShaderProgram(const std::shared_ptr<ShaderProgram>& shaderProgram) {
		this->shaderProgram = shaderProgram;
		if (shaderProgram == nullptr) return;

		this->lightPosLoc.update(shaderProgram);
		this->lightColorLoc.update(shaderProgram);
		this->lightDirLoc.update(shaderProgram);
		this->lightSpaceMatrixLoc.update(shaderProgram);
		this->usedLightsLoc.update(shaderProgram);
		this->viewPosLoc.update(shaderProgram);
		this->viewDirLoc.update(shaderProgram);

		for (auto& texture : textures) {
			texture.first.update(shaderProgram);
		}

		{
			// Update vec3s by copying them to a new map, because I cannot update the UniLocation objects in the map
			auto newMap = std::unordered_map<UniLocation, glm::vec3>();
			for (auto& vect : vec3s) {
				newMap[UniLocation(vect.first, shaderProgram)] = vect.second;
			}
			this->vec3s = std::move(newMap);
		}

		{
			auto newMap = std::unordered_map<UniLocation, float>();
			for (auto& flt : floats) {
				newMap[UniLocation(flt.first, shaderProgram)] = flt.second;
			}
			this->floats = std::move(newMap);
		}

		{
			auto newMap = std::unordered_map<UniLocation, glm::mat4>();
			for (auto& mat : mat4s) {
				newMap[UniLocation(mat.first, shaderProgram)] = mat.second;
			}
			this->mat4s = std::move(newMap);
		}
	}

	std::shared_ptr<ShaderProgram> getShaderProgram() const {
		return shaderProgram;
	}

	void addTexture(const std::string& name, const std::shared_ptr<Texture>& texture) {
		textures.push_back(make_pair(UniLocation(name, shaderProgram, true), texture));
	}

	void setVec3(const std::string& name, const glm::vec3& vec) {
		vec3s[UniLocation(name, shaderProgram, true)] = vec;
	}

	void setFloat(const std::string& name, float flt) {
		floats[UniLocation(name, shaderProgram, true)] = flt;
	}

	void setMat4(const std::string& name, const glm::mat4& mat) {
		mat4s[UniLocation(name, shaderProgram, true)] = mat;
	}

	void setName(std::string name) {
		this->name = name;
	}

	std::string getName() const {
		return name;
	}

	void setIncludeLightsUniforms(bool includeLightsUniforms) {
		this->includeLightsUniforms = includeLightsUniforms;
	}

	bool getIncludeLightsUniforms() const {
		return includeLightsUniforms;
	}

	void setIncludeCameraPosUniform(bool includeCameraPosUniform) {
		this->includeCameraPosUniform = includeCameraPosUniform;
	}

	bool getIncludeCameraPosUniform() const {
		return includeCameraPosUniform;
	}

	void setIncludeCameraDirUniform(bool includeCameraDirUniform) {
		this->includeCameraDirUniform = includeCameraDirUniform;
	}

	bool getIncludeCameraDirUniform() const {
		return includeCameraDirUniform;
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
		for (auto& mat : mat4s) {
			shaderProgram->setMat4(mat.first, mat.second);
		}
		if (includeLightsUniforms) {
			shaderProgram->setVec4s(lightPosLoc, Light::getLightPositions(), Light::getMaxLights());
			shaderProgram->setVec3s(lightColorLoc, Light::getLightColors(), Light::getMaxLights());
			shaderProgram->setVec3s(lightDirLoc, Light::getLightDirections(), Light::getMaxLights());
			shaderProgram->setMat4s(lightSpaceMatrixLoc, Light::getLightSpaceMatrices(), Light::getMaxLights());
			shaderProgram->setInt(usedLightsLoc, Light::getUsedLightsCnt());
		}
		if (includeCameraPosUniform) {
			shaderProgram->setVec3(viewPosLoc, Camera::getActiveCamera()->getPosition());
		}
		if (includeCameraDirUniform) {
			shaderProgram->setVec3(viewDirLoc, Camera::getActiveCamera()->getDirection());
		}
	}

protected:
	std::shared_ptr<ShaderProgram> shaderProgram;
	std::vector<std::pair<UniLocation, std::shared_ptr<Texture>>> textures;
	std::unordered_map<UniLocation, glm::vec3> vec3s;
	std::unordered_map<UniLocation, float> floats;
	std::unordered_map<UniLocation, glm::mat4> mat4s;
	std::string name;
	bool includeLightsUniforms = false;
	bool includeCameraPosUniform = false;
	bool includeCameraDirUniform = false;

	UniLocation lightPosLoc = UniLocation("lightPos", shaderProgram);
	UniLocation lightColorLoc = UniLocation("lightColor", shaderProgram);
	UniLocation lightDirLoc = UniLocation("lightDir", shaderProgram);
	UniLocation lightSpaceMatrixLoc = UniLocation("lightSpaceMatrix", shaderProgram);
	UniLocation usedLightsLoc = UniLocation("usedLights", shaderProgram);
	UniLocation viewPosLoc = UniLocation("viewPos", shaderProgram);
	UniLocation viewDirLoc = UniLocation("viewDir", shaderProgram);
};

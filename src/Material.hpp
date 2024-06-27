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
	enum class BlendMode {
		ALPHA_CLIP,
		ALPHA_BLEND
	};

	Material(const std::shared_ptr<ShaderProgram>& shaderProgram, const std::string& name = "")
		: shaderProgram(shaderProgram), name(name) {
		lightPosLoc = UniLocation("lightPos", shaderProgram);
		lightColorLoc = UniLocation("lightColor", shaderProgram);
		lightDirLoc = UniLocation("lightDir", shaderProgram);
		lightSpaceMatrixLoc = UniLocation("lightSpaceMatrices", shaderProgram);
		usedLightsLoc = UniLocation("usedLights", shaderProgram);
		viewPosLoc = UniLocation("viewPos", shaderProgram);
		viewDirLoc = UniLocation("viewDir", shaderProgram);
		nearFarLoc = UniLocation("nearFar", shaderProgram);
		frameCounterLoc = UniLocation("frameCounter", shaderProgram);

		UniLocation projectionViewLoc = UniLocation("projectionView", shaderProgram);
		UniLocation viewLoc = UniLocation("view", shaderProgram);
		UniLocation projectionLoc = UniLocation("projection", shaderProgram);
	}

	Material(const Material& other) 
		: shaderProgram(other.shaderProgram), textures(other.textures), vec3s(other.vec3s), floats(other.floats), name(other.name), includeLightsUniforms(other.includeLightsUniforms), 
		includeCameraUniform(other.includeCameraUniform), includeFrameCounterUniform(other.includeFrameCounterUniform),
		lightPosLoc(other.lightPosLoc), lightColorLoc(other.lightColorLoc), 
		lightDirLoc(other.lightDirLoc), lightSpaceMatrixLoc(other.lightSpaceMatrixLoc),
		usedLightsLoc(other.usedLightsLoc), viewPosLoc(other.viewPosLoc), viewDirLoc(other.viewDirLoc), nearFarLoc(other.nearFarLoc), frameCounterLoc(other.frameCounterLoc),
		blendMode(other.blendMode) {

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
		this->nearFarLoc.update(shaderProgram);
		this->frameCounterLoc.update(shaderProgram);

		{
			auto newMap = std::unordered_map<UniLocation, std::shared_ptr<Texture>>();
			for (auto& texture : textures) {
				newMap[UniLocation(texture.first, shaderProgram)] = texture.second;
			}
			this->textures = std::move(newMap);
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

	void setTexture(const std::string& name, const std::shared_ptr<Texture>& texture) {
		if (overrideMaterial == nullptr || overrideMaterial.get() == this)
			textures[UniLocation(name, shaderProgram)] = texture;
		else
			overrideMaterial->setTexture(name, texture);
	}

	void setVec3(const std::string& name, const glm::vec3& vec) {
		if (overrideMaterial == nullptr || overrideMaterial.get() == this)
			vec3s[UniLocation(name, shaderProgram, true)] = vec;
		else
			overrideMaterial->setVec3(name, vec);
	}

	void setFloat(const std::string& name, float flt) {
		if (overrideMaterial == nullptr || overrideMaterial.get() == this)
			floats[UniLocation(name, shaderProgram, true)] = flt;
		else
			overrideMaterial->setFloat(name, flt);
	}

	void setMat4(const std::string& name, const glm::mat4& mat) {
		if (overrideMaterial == nullptr || overrideMaterial.get() == this)
			mat4s[UniLocation(name, shaderProgram, true)] = mat;
		else
			overrideMaterial->setMat4(name, mat);
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

	void setIncludeCameraUniform(bool includeCameraUniform) {
		this->includeCameraUniform = includeCameraUniform;
	}

	bool getIncludeCameraUniform() const {
		return includeCameraUniform;
	}

	void setIncludeFrameCounterUniform(bool includeFrameCounterUniform) {
		this->includeFrameCounterUniform = includeFrameCounterUniform;
	}

	bool getIncludeFrameCounterUniform() const {
		return includeFrameCounterUniform;
	}

	void setBlendMode(BlendMode blendMode) {
		this->blendMode = blendMode;
	}

	BlendMode getBlendMode() const {
		return blendMode;
	}

	void use() {
		if (overrideMaterial != nullptr && overrideMaterial.get() != this) {
			overrideMaterial->use();
			return;
		}

		if (shaderProgram == nullptr) { 
			std::cout << "Material::use() called with shaderProgram = nullptr\n";
			return;
		}

		shaderProgram->use();
		unsigned int i = 0;
		for (auto& tex : textures) {
			glActiveTexture(GL_TEXTURE0 + i);
			tex.second->bind();
			shaderProgram->setInt(tex.first, i);
			i++;
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

		std::vector<glm::mat4> projectionViewMatrices;
		std::vector<glm::mat4> viewMatrices;
		std::vector<glm::mat4> projectionMatrices;
		for (auto& camera : Camera::getActiveCameras()) {
			projectionViewMatrices.push_back(camera->getCameraMatrix());
			viewMatrices.push_back(camera->getViewMatrix());
			projectionMatrices.push_back(camera->getProjectionMatrix());
		}

		shaderProgram->setMat4s(projectionViewLoc, projectionViewMatrices.data(), projectionViewMatrices.size());
		shaderProgram->setMat4s(viewLoc, viewMatrices.data(), viewMatrices.size());
		shaderProgram->setMat4s(projectionLoc, projectionMatrices.data(), projectionMatrices.size());

		if (includeLightsUniforms) {
			shaderProgram->setVec4s(lightPosLoc, Light::getLightPositions(), Light::getMaxLights());
			shaderProgram->setVec3s(lightColorLoc, Light::getLightColors(), Light::getMaxLights());
			shaderProgram->setVec3s(lightDirLoc, Light::getLightDirections(), Light::getMaxLights());
			shaderProgram->setMat4s(lightSpaceMatrixLoc, Light::getLightSpaceMatrices(), Light::getMaxLights() * 4); //TODO
			shaderProgram->setInt(usedLightsLoc, Light::getUsedLightsCnt());
		}
		if (includeCameraUniform) {
			Camera* activeCamera = Camera::getActiveCamera();
			shaderProgram->setVec3(viewPosLoc, activeCamera->getPosition());
			shaderProgram->setVec3(viewDirLoc, activeCamera->getDirection());
			shaderProgram->setVec2(nearFarLoc, glm::vec2(activeCamera->getNearPlane(), activeCamera->getFarPlane()));
		}
		if (includeFrameCounterUniform) {
			shaderProgram->setInt(frameCounterLoc, WindowManager::getFrameCounter());
		}
	}

	static void setOverrideMaterial(const std::shared_ptr<Material>& material) {
		overrideMaterial = material;
	}

protected:
	std::shared_ptr<ShaderProgram> shaderProgram;
	std::unordered_map<UniLocation, std::shared_ptr<Texture>> textures;
	std::unordered_map<UniLocation, glm::vec3> vec3s;
	std::unordered_map<UniLocation, float> floats;
	std::unordered_map<UniLocation, glm::mat4> mat4s;
	std::string name;
	BlendMode blendMode = BlendMode::ALPHA_CLIP;
	bool includeLightsUniforms = false;
	bool includeCameraUniform = false;
	bool includeFrameCounterUniform = false;

private:
	UniLocation lightPosLoc = UniLocation("lightPos", shaderProgram);
	UniLocation lightColorLoc = UniLocation("lightColor", shaderProgram);
	UniLocation lightDirLoc = UniLocation("lightDir", shaderProgram);
	UniLocation lightSpaceMatrixLoc = UniLocation("lightSpaceMatrices", shaderProgram);
	UniLocation usedLightsLoc = UniLocation("usedLights", shaderProgram);
	UniLocation viewPosLoc = UniLocation("viewPos", shaderProgram);
	UniLocation viewDirLoc = UniLocation("viewDir", shaderProgram);
	UniLocation nearFarLoc = UniLocation("nearFar", shaderProgram);
	UniLocation frameCounterLoc = UniLocation("frameCounter", shaderProgram);

	UniLocation projectionViewLoc = UniLocation("projectionView", shaderProgram);
	UniLocation viewLoc = UniLocation("view", shaderProgram);
	UniLocation projectionLoc = UniLocation("projection", shaderProgram);

	static std::shared_ptr<Material> overrideMaterial;
};

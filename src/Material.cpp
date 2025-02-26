
#include "Material.hpp"

Material::Material(const std::shared_ptr<ShaderProgram>& shaderProgram, const std::string& name)
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

	projectionViewLoc = UniLocation("projectionView", shaderProgram);
	viewLoc = UniLocation("view", shaderProgram);
	projectionLoc = UniLocation("projection", shaderProgram);
}

Material::Material(const Material& other)
	: shaderProgram(other.shaderProgram), textures(other.textures), vec3s(other.vec3s), floats(other.floats), name(other.name), includeLightsUniforms(other.includeLightsUniforms),
	includeCameraUniform(other.includeCameraUniform), includeFrameCounterUniform(other.includeFrameCounterUniform),
	lightPosLoc(other.lightPosLoc), lightColorLoc(other.lightColorLoc),
	lightDirLoc(other.lightDirLoc), lightSpaceMatrixLoc(other.lightSpaceMatrixLoc),
	usedLightsLoc(other.usedLightsLoc), viewPosLoc(other.viewPosLoc), viewDirLoc(other.viewDirLoc), nearFarLoc(other.nearFarLoc), frameCounterLoc(other.frameCounterLoc),
	projectionViewLoc(other.projectionViewLoc), viewLoc(other.viewLoc), projectionLoc(other.projectionLoc),
	blendMode(other.blendMode) {

}

Material::Material() {
	shaderProgram = nullptr;
}

void Material::setShaderProgram(const std::shared_ptr<ShaderProgram>& shaderProgram) {
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
		auto newMap = ResourceMap<std::shared_ptr<Texture>>();
		for (auto& texture : textures.map) {
			newMap[UniLocation(texture.first, shaderProgram)] = texture.second;
		}
		this->textures = std::move(newMap);
	}

	{
		// Update vec3s by copying them to a new map, because I cannot update the UniLocation objects in the map
		auto newMap = ResourceMap<glm::vec3>();
		for (auto& vect : vec3s.map) {
			newMap[UniLocation(vect.first, shaderProgram)] = vect.second;
		}
		this->vec3s = std::move(newMap);
	}

	{
		auto newMap = ResourceMap<float>();
		for (auto& flt : floats.map) {
			newMap[UniLocation(flt.first, shaderProgram)] = flt.second;
		}
		this->floats = std::move(newMap);
	}

	{
		auto newMap = ResourceMap<glm::mat4>();
		for (auto& mat : mat4s.map) {
			newMap[UniLocation(mat.first, shaderProgram)] = mat.second;
		}
		this->mat4s = std::move(newMap);
	}
}

std::shared_ptr<ShaderProgram> Material::getShaderProgram() const {
	return shaderProgram;
}

void Material::setTexture(const std::string& name, const std::shared_ptr<Texture>& texture) {
	if (overrideMaterial == nullptr || overrideMaterial.get() == this)
		textures[UniLocation(name, shaderProgram)] = texture;
	else
		overrideMaterial->setTexture(name, texture);
}

void Material::setVec3(const std::string& name, const glm::vec3& vec) {
	if (overrideMaterial == nullptr || overrideMaterial.get() == this)
		vec3s[UniLocation(name, shaderProgram, true)] = vec;
	else
		overrideMaterial->setVec3(name, vec);
}

void Material::setFloat(const std::string& name, float flt) {
	if (overrideMaterial == nullptr || overrideMaterial.get() == this)
		floats[UniLocation(name, shaderProgram, true)] = flt;
	else
		overrideMaterial->setFloat(name, flt);
}

void Material::setMat4(const std::string& name, const glm::mat4& mat) {
	if (overrideMaterial == nullptr || overrideMaterial.get() == this)
		mat4s[UniLocation(name, shaderProgram, true)] = mat;
	else
		overrideMaterial->setMat4(name, mat);
}

void Material::setName(std::string name) {
	this->name = name;
}

std::string Material::getName() const {
	return name;
}

void Material::setIncludeLightsUniforms(bool includeLightsUniforms) {
	this->includeLightsUniforms = includeLightsUniforms;
}

bool Material::getIncludeLightsUniforms() const {
	return includeLightsUniforms;
}

void Material::setIncludeCameraUniform(bool includeCameraUniform) {
	this->includeCameraUniform = includeCameraUniform;
}

bool Material::getIncludeCameraUniform() const {
	return includeCameraUniform;
}

void Material::setIncludeFrameCounterUniform(bool includeFrameCounterUniform) {
	this->includeFrameCounterUniform = includeFrameCounterUniform;
}

bool Material::getIncludeFrameCounterUniform() const {
	return includeFrameCounterUniform;
}

void Material::setBlendMode(BlendMode blendMode) {
	this->blendMode = blendMode;
}

Material::BlendMode Material::getBlendMode() const {
	return blendMode;
}

// with allowResourceReuse = true, the material will not be updated if it was already used in the last frame
// some resources will still be updated, when they were changed
// when setting allowResourceReuse to true, remember that some changes to the material may not be visible, unless calling resetLastMaterial()
void Material::use(bool allowResourceReuse) {
	if (overrideMaterial != nullptr && overrideMaterial.get() != this) {
		overrideMaterial->use();
		return;
	}

	if (shaderProgram == nullptr) {
		std::cout << "Material::use() called with shaderProgram = nullptr\n";
		return;
	}

	shaderProgram->use();

	if (!allowResourceReuse || lastMaterial != this || textures.dirty == true) {
		unsigned int i = 0;
		for (auto& tex : textures.map) {
			glActiveTexture(GL_TEXTURE0 + i);
			tex.second->bind();
			shaderProgram->setInt(tex.first, i);
			i++;
		}
		textures.dirty = false;
	}

	if (!allowResourceReuse || lastMaterial != this || vec3s.dirty == true) {
		for (auto& vect : vec3s.map) {
			shaderProgram->setVec3(vect.first, vect.second);
		}
		vec3s.dirty = false;
	}

	if (!allowResourceReuse || lastMaterial != this || floats.dirty == true) {
		for (auto& flt : floats.map) {
			shaderProgram->setFloat(flt.first, flt.second);
		}
		floats.dirty = false;
	}

	if (!allowResourceReuse || lastMaterial != this || mat4s.dirty == true) {
		for (auto& mat : mat4s.map) {
			shaderProgram->setMat4(mat.first, mat.second);
		}
		mat4s.dirty = false;
	}

	if (!allowResourceReuse || lastMaterial != this) {
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
			shaderProgram->setMat4s(lightSpaceMatrixLoc, Light::getLightSpaceMatrices(), Light::getMaxLightSpaceMatrices());
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

	lastMaterial = this;
}

void Material::setOverrideMaterial(const std::shared_ptr<Material>& material) {
	overrideMaterial = material;
}

void Material::resetLastMaterial() {
	lastMaterial = nullptr;
}

std::shared_ptr<Material> Material::overrideMaterial = nullptr;
Material* Material::lastMaterial = nullptr;

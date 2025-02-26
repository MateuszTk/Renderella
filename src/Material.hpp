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

		Material(const std::shared_ptr<ShaderProgram>& shaderProgram, const std::string& name = "");
		Material(const Material& other);
		Material();
		virtual ~Material() = default;

		void setShaderProgram(const std::shared_ptr<ShaderProgram>& shaderProgram);

		std::shared_ptr<ShaderProgram> getShaderProgram() const;

		void setTexture(const std::string& name, const std::shared_ptr<Texture>& texture);
		void setVec3(const std::string& name, const glm::vec3& vec);
		void setFloat(const std::string& name, float flt);
		void setMat4(const std::string& name, const glm::mat4& mat);

		void setName(std::string name);
		std::string getName() const;

		void setIncludeLightsUniforms(bool includeLightsUniforms);
		bool getIncludeLightsUniforms() const;

		void setIncludeCameraUniform(bool includeCameraUniform);
		bool getIncludeCameraUniform() const;

		void setIncludeFrameCounterUniform(bool includeFrameCounterUniform);
		bool getIncludeFrameCounterUniform() const;

		void setBlendMode(BlendMode blendMode);
		BlendMode getBlendMode() const;

		// with allowResourceReuse = true, the material will not be updated if it was already used in the last frame
		// some resources will still be updated, when they were changed
		// when setting allowResourceReuse to true, remember that some changes to the material may not be visible, unless calling resetLastMaterial()
		void use(bool allowResourceReuse = false);

		static void setOverrideMaterial(const std::shared_ptr<Material>& material);
		static void resetLastMaterial();

	protected:

		template <typename T>
		struct ResourceMap {
			std::unordered_map<UniLocation, T> map;
			bool dirty = true;

			T& operator[](const UniLocation& loc) {
				dirty = true;
				return map[loc];
			}
		};

		std::shared_ptr<ShaderProgram> shaderProgram;
		ResourceMap<std::shared_ptr<Texture>> textures;
		ResourceMap<glm::vec3> vec3s;
		ResourceMap<float> floats;
		ResourceMap<glm::mat4> mat4s;

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
		static Material* lastMaterial;

};

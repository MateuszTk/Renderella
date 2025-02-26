#pragma once
#include "Material.hpp"

class PhongMat : public Material {

	public:

		PhongMat(const std::string& name);
		PhongMat(const PhongMat& other);

		void setDiffuseMap(const std::shared_ptr<Texture>& texture);
		void setNormalMap(const std::shared_ptr<Texture>& texture);
		void setSpecularMap(const std::shared_ptr<Texture>& texture);
		void setShininessMap(const std::shared_ptr<Texture>& texture);
		void setDiffuse(const glm::vec3& diffuse);
		void setSpecular(const glm::vec3& specular);
		void setShininess(float shininess);

		static std::shared_ptr<ShaderProgram> getShaderProgram();

	private:

		static std::shared_ptr<ShaderProgram> phongShaderProgram;

		void setAmbient(const glm::vec3& ambient);

		static std::shared_ptr<Texture> defaultDiffuseMap;
		static std::shared_ptr<Texture> defaultNormalMap;
		static std::shared_ptr<Texture> defaultSpecularMap;
		static std::shared_ptr<Texture> defaultShininessMap;
};

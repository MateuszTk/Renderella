#pragma once

#include "Mesh.hpp"

class InstancedMesh : public Mesh {

	public:

		InstancedMesh(const Mesh& mesh, const std::vector<glm::mat4>& modelMatrices);
		~InstancedMesh() = default;

		void draw(bool drawAll = true, Material::BlendMode blendModeOnly = Material::BlendMode::ALPHA_CLIP, bool unbind = true, bool allowResourceReuse = false) override;

		void clearInstances();

		void addInstances(const std::vector<glm::mat4>& modelMatrices);

	private:

		unsigned int instanceVBO;
		std::vector<glm::mat4> modelMatrices;

		void clear() override;

};

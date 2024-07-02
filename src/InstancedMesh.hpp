#pragma once

#include "Mesh.hpp"

class InstancedMesh : public Mesh {
public:
	InstancedMesh(const Mesh& mesh, const std::vector<glm::mat4>& modelMatrices) : Mesh(mesh), modelMatrices(modelMatrices) {
		glGenBuffers(1, &this->instanceVBO);
		addInstances(modelMatrices);
		glBindVertexArray(this->VAO);
		
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(9);

		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
		glVertexAttribDivisor(8, 1);

		glBindVertexArray(0);
	}

	void draw(bool drawAll = true, Material::BlendMode blendModeOnly = Material::BlendMode::ALPHA_CLIP, bool unbind = true, bool allowResourceReuse = false) override {
		bool firstDraw = true;
		for (auto& submesh : this->submeshes) {
			if (drawAll || submesh.material->getBlendMode() == blendModeOnly) {
				if (firstDraw) {
					firstDraw = false;
					glBindVertexArray(VAO);
				}
				glm::mat4 firstModel = (modelMatrices.size() > 0) ? modelMatrices[0] : glm::mat4(1.0f);
				submesh.material->setMat4("model", firstModel);
				submesh.material->use(allowResourceReuse);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, submesh.EBO);
				glDrawElementsInstanced(GL_TRIANGLES, submesh.elements.size(), GL_UNSIGNED_INT, 0, modelMatrices.size());
			}
		}
		if (!firstDraw && unbind) {
			Mesh::unbind();
		}
	}

	void clearInstances() {
		this->modelMatrices.clear();
	}

	void addInstances(const std::vector<glm::mat4>& modelMatrices) {
		this->modelMatrices.insert(this->modelMatrices.end(), modelMatrices.begin(), modelMatrices.end());
		glBindVertexArray(this->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, this->instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, this->modelMatrices.size() * sizeof(glm::mat4), &(this->modelMatrices[0]), GL_STATIC_DRAW);
		glBindVertexArray(0);
	}

	~InstancedMesh() {

	}

private:
	unsigned int instanceVBO;
	std::vector<glm::mat4> modelMatrices;

	void clear() override {
		Mesh::clear();
		if (!moved) {
			glDeleteBuffers(1, &this->instanceVBO);
		}
	}
};

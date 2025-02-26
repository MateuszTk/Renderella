
#include "InstancedMesh.hpp"

InstancedMesh::InstancedMesh(const Mesh& mesh, const std::vector<glm::mat4>& modelMatrices) 
	: Mesh(mesh), modelMatrices(modelMatrices) {

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

void InstancedMesh::draw(bool drawAll, Material::BlendMode blendModeOnly, bool unbind, bool allowResourceReuse) {
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

void InstancedMesh::clearInstances() {
	this->modelMatrices.clear();
}

void InstancedMesh::addInstances(const std::vector<glm::mat4>& modelMatrices) {
	this->modelMatrices.insert(this->modelMatrices.end(), modelMatrices.begin(), modelMatrices.end());
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, this->modelMatrices.size() * sizeof(glm::mat4), &(this->modelMatrices[0]), GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void InstancedMesh::clear() {
	Mesh::clear();
	if (!moved) {
		glDeleteBuffers(1, &this->instanceVBO);
	}
}

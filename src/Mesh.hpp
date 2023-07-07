#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "Vertex.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ShaderProgram.hpp"
#include "Material.hpp"
#include "Camera.hpp"

struct SubMesh {
public:
	SubMesh(const std::vector<unsigned int>& elements, const Material& material) : elements(elements), material(material) {

	}

	SubMesh() {

	}

	std::vector<unsigned int> elements;
	Material material;

	friend class Mesh;
private:
	unsigned int EBO;
};

class Mesh {
public:
	Mesh(const std::vector<Vertex>& vertices, std::vector<SubMesh> newSubmeshes) : submeshes(newSubmeshes), position(0.0f), rotation(0.0f), scale(1.0f) {
		moved = false;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		setVertices(vertices);

		this->EBOs = new unsigned int[newSubmeshes.size()];
		glGenBuffers(newSubmeshes.size(), this->EBOs);

		for (int i = 0; i < this->submeshes.size(); i++) {
			submeshes[i].EBO = (this->EBOs)[i];
		}

		updateElements();
		updateModel();
	}

	Mesh(Mesh&& other) {
		other.moved = true;
		this->moved = false;
		this->VAO = other.VAO;
		this->VBO = other.VBO;
		this->EBOs = other.EBOs;
		this->submeshes = std::move(other.submeshes);
		this->vertices = std::move(other.vertices);
		this->position = other.position;
		this->rotation = other.rotation;
		this->scale = other.scale;
		updateModel();
	}

	std::vector<Vertex>& getVertices() {
		return vertices;
	}

	std::vector<SubMesh>& getSubmeshes() {
		return submeshes;
	}

	void setVertices(const std::vector<Vertex>& vertices) {
		this->vertices = vertices;
		updateVertices();
	}

	void updateVertices() {
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &(this->vertices[0]), GL_STATIC_DRAW);

		//takes data from VBO and feeds it to the vertex shader
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 6));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 8));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 11));
		glEnableVertexAttribArray(4);

		glBindVertexArray(0);
	}

	void updateElements() {
		glBindVertexArray(VAO);
		for (SubMesh& submesh : submeshes) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, submesh.EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, submesh.elements.size() * sizeof(unsigned int), &(submesh.elements[0]), GL_STATIC_DRAW);
		}
		glBindVertexArray(0);
	}

	void draw() {
		glBindVertexArray(VAO);

		glm::mat4 finalTransformationMatrix = Camera::getActiveCamera()->getCameraMatrix() * model;

		for (auto& submesh : this->submeshes) {			
			submesh.material.setMat4("transformations", finalTransformationMatrix);
			submesh.material.setMat4("model", model);
			submesh.material.setMat4("projectionView", Camera::getActiveCamera()->getCameraMatrix());
			submesh.material.use();

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, submesh.EBO);
			glDrawElements(GL_TRIANGLES, submesh.elements.size(), GL_UNSIGNED_INT, 0);
		}

		glBindVertexArray(0);
	}

	void clear() {
		if (!moved) {
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
			glDeleteBuffers(submeshes.size(), EBOs);
			delete[] EBOs;
		}
	}

	glm::vec3 getPosition() const {
		return this->position;
	}

	void setPosition(const glm::vec3& position) {
		this->position = position;
		updateModel();
	}

	glm::vec3 getRotation() const {
		return this->rotation;
	}

	void setRotation(const glm::vec3& rotation) {
		this->rotation = rotation;
		updateModel();
	}

	glm::vec3 getScale() const {
		return this->scale;
	}

	void setScale(const glm::vec3& scale) {
		this->scale = scale;
		updateModel();
	}

	~Mesh() {
		clear();
	}

private:
	unsigned int* EBOs;
	unsigned int VAO, VBO;
	std::vector<Vertex> vertices;
	std::vector<SubMesh> submeshes;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	bool moved;
	glm::mat4 model;

	void updateModel() {
		//apply transformations
		model = glm::mat4(1.0f);
		model = glm::translate(model, this->position);
		model = glm::rotate(model, glm::radians(this->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, this->scale);
	}
};

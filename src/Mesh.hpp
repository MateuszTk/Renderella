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

struct SubMesh {
public:
	SubMesh(const std::vector<unsigned int>& elements, const Material& material) : elements(elements), material(material) {

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
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		setVertices(vertices);

		this->EBOs = new unsigned int[newSubmeshes.size()];
		glGenBuffers(newSubmeshes.size(), this->EBOs);

		for (int i = 0; i < this->submeshes.size(); i++) {
			submeshes[i].EBO = (this->EBOs)[i];
		}

		updateElements();		
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
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(1);

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

		//apply transformations
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, this->position);
		model = glm::rotate(model, glm::radians(this->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, this->scale);

		glm::mat4 view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

		glm::mat4 finalTransformationMatrix = projection * view * model;

		for (auto& submesh : this->submeshes) {
			submesh.material.use();
			submesh.material.getShaderProgram()->setMat4("transformations", finalTransformationMatrix);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, submesh.EBO);
			glDrawElements(GL_TRIANGLES, submesh.elements.size(), GL_UNSIGNED_INT, 0);
		}

		glBindVertexArray(0);
	}

	void clear() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(submeshes.size(), EBOs);
		delete[] EBOs;
	}

	glm::vec3 getPosition() const {
		return this->position;
	}

	void setPosition(const glm::vec3& position) {
		this->position = position;
	}

	glm::vec3 getRotation() const {
		return this->rotation;
	}

	void setRotation(const glm::vec3& rotation) {
		this->rotation = rotation;
	}

	glm::vec3 getScale() const {
		return this->scale;
	}

	void setScale(const glm::vec3& scale) {
		this->scale = scale;
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
};

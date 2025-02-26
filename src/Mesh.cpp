
#include "Mesh.hpp"

Mesh::Mesh(const std::vector<Vertex>& vertices, std::vector<SubMesh> newSubmeshes) 
	: submeshes(newSubmeshes), position(0.0f), rotation(0.0f), scale(1.0f) {

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

Mesh::Mesh(const Mesh& other) 
	: submeshes(other.submeshes), position(other.position), rotation(other.rotation), scale(other.scale) {
	
	moved = false;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	setVertices(other.vertices);

	this->EBOs = new unsigned int[submeshes.size()];
	glGenBuffers(submeshes.size(), this->EBOs);

	for (int i = 0; i < submeshes.size(); i++) {
		submeshes[i].EBO = (this->EBOs)[i];
	}

	updateElements();
	updateModel();
}

Mesh::Mesh(Mesh&& other) {
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

std::vector<Vertex>& Mesh::getVertices() {
	return vertices;
}

std::vector<SubMesh>& Mesh::getSubmeshes() {
	return submeshes;
}

void Mesh::setVertices(const std::vector<Vertex>& vertices) {
	this->vertices = vertices;
	updateVertices();
}

void Mesh::updateVertices() {
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

void Mesh::updateElements() {
	glBindVertexArray(VAO);
	for (SubMesh& submesh : submeshes) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, submesh.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, submesh.elements.size() * sizeof(unsigned int), &(submesh.elements[0]), GL_STATIC_DRAW);
	}
	glBindVertexArray(0);
}

void Mesh::draw(bool drawAll, Material::BlendMode blendModeOnly, bool unbind, bool allowResourceReuse) {
	bool firstDraw = true;
	for (auto& submesh : this->submeshes) {
		if (drawAll || submesh.material->getBlendMode() == blendModeOnly) {
			if (firstDraw) {
				firstDraw = false;
				glBindVertexArray(VAO);
			}
			submesh.material->setMat4("model", model);
			submesh.material->use(allowResourceReuse);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, submesh.EBO);
			glDrawElements(GL_TRIANGLES, submesh.elements.size(), GL_UNSIGNED_INT, 0);
		}
	}
	if (!firstDraw && unbind) {
		Mesh::unbind();
	}
}

void Mesh::unbind() {
	glBindVertexArray(0);
}

glm::vec3 Mesh::getPosition() const {
	return this->position;
}

void Mesh::setPosition(const glm::vec3& position) {
	this->position = position;
	updateModel();
}

glm::vec3 Mesh::getRotation() const {
	return this->rotation;
}

void Mesh::setRotation(const glm::vec3& rotation) {
	this->rotation = rotation;
	updateModel();
}

glm::vec3 Mesh::getScale() const {
	return this->scale;
}

void Mesh::setScale(const glm::vec3& scale) {
	this->scale = scale;
	updateModel();
}

void Mesh::setScale(float scale) {
	this->scale = glm::vec3(scale);
	updateModel();
}

glm::mat4 Mesh::toModelMatrix(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, scale);
	return model;
}

Mesh::~Mesh() {
	clear();
}

void Mesh::updateModel() {
	//apply transformations
	model = toModelMatrix(this->position, this->rotation, this->scale);
}

void Mesh::clear() {
	if (!moved) {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(submeshes.size(), EBOs);
		delete[] EBOs;
	}
}

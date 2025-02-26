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

		SubMesh(const std::vector<unsigned int>& elements, std::shared_ptr<Material> material) 
			: elements(elements), material(material) {}

		SubMesh() {}

		std::vector<unsigned int> elements;
		std::shared_ptr<Material> material;

		friend class Mesh;
		friend class InstancedMesh;

	private:

		unsigned int EBO;
};

class Mesh {

	public:

		Mesh(const std::vector<Vertex>& vertices, std::vector<SubMesh> newSubmeshes);
		Mesh(const Mesh& other);
		Mesh(Mesh&& other);
		virtual ~Mesh();

		std::vector<Vertex>& getVertices();
		std::vector<SubMesh>& getSubmeshes();
		void setVertices(const std::vector<Vertex>& vertices);
		void updateVertices();
		void updateElements();

		virtual void draw(bool drawAll = true, Material::BlendMode blendModeOnly = Material::BlendMode::ALPHA_CLIP, bool unbind = true, bool allowResourceReuse = false);
		static void unbind();

		glm::vec3 getPosition() const;
		void setPosition(const glm::vec3& position);

		glm::vec3 getRotation() const;
		void setRotation(const glm::vec3& rotation);

		glm::vec3 getScale() const;
		void setScale(const glm::vec3& scale);
		void setScale(float scale);

		static glm::mat4 toModelMatrix(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
	
	protected:

		unsigned int* EBOs;
		unsigned int VAO, VBO;
		std::vector<Vertex> vertices;
		std::vector<SubMesh> submeshes;
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
		bool moved;
		glm::mat4 model;

		void updateModel();
		virtual void clear();

};

#pragma once

#include "Mesh.hpp"
#include "PerlinNoise.hpp"

struct Tree {
	std::shared_ptr<InstancedMesh> trunk;
	std::shared_ptr<InstancedMesh> leaves;

	Tree()
		: trunk(nullptr),
		leaves(nullptr) {

	}

	Tree(const std::shared_ptr<Mesh>& trunk, const std::shared_ptr<Mesh>& leaves) 
		: trunk(std::make_shared<InstancedMesh>(*trunk, std::vector<glm::mat4>())),
		leaves(std::make_shared<InstancedMesh>(*leaves, std::vector<glm::mat4>())) {

	}

	Tree(const Tree& tree)
		: trunk(std::make_shared<InstancedMesh>(*tree.trunk, std::vector<glm::mat4>())),
		leaves(std::make_shared<InstancedMesh>(*tree.leaves, std::vector<glm::mat4>())) {

	}

	Tree(Tree&& tree)
		: trunk(std::move(tree.trunk)),
		leaves(std::move(tree.leaves)) {

	}

	Tree& operator=(const Tree& tree) {
		trunk = std::make_shared<InstancedMesh>(*tree.trunk, std::vector<glm::mat4>());
		leaves = std::make_shared<InstancedMesh>(*tree.leaves, std::vector<glm::mat4>());
		return *this;
	}

	void clear() {
		trunk->clearInstances();
		leaves->clearInstances();
	}

	void add(const std::vector<glm::mat4>& models) {
		trunk->addInstances(models);
		leaves->addInstances(models);
	}
};

using ChunkMap = std::map<int, std::shared_ptr<Mesh>>;
using TreeMap = std::map<int, Tree>;

class WorldGen {
public:
	WorldGen(int seed, int chunkSize, int worldSize, std::shared_ptr<Material> material, const Tree& tree)
		: seed(seed), chunkSize(chunkSize), worldSize(worldSize), material(material), perlin(seed) {
		for (int y = 0; y < worldSize; y++) {
			for (int x = 0; x < worldSize; x++) {
				SubMesh submesh;
				submesh.material = material;
				chunks[x + y * worldSize * chunkSize] = (std::make_shared<Mesh>(std::vector<Vertex>(), std::vector<SubMesh>({submesh})));
			
				trees[x + y * worldSize] = tree;
			}
		}
	}

	void generateChunk(glm::ivec2 pos, int vertexCount) {
		std::shared_ptr<Mesh>& mesh = chunks[pos.x + pos.y * worldSize * chunkSize];
		mesh->setPosition(glm::vec3(pos.x * chunkSize, 0, pos.y * chunkSize));

		std::vector<Vertex>& vertices = mesh->getVertices();

		std::vector<unsigned int>& indices = mesh->getSubmeshes()[0].elements;

		vertices.clear();
		indices.clear();

		for (int y = 0; y < vertexCount; y++) {
			for (int x = 0; x < vertexCount; x++) {
				Vertex vertex;

				vertex.position.x = (x / (float)(vertexCount - 1)) * chunkSize;
				vertex.position.z = (y / (float)(vertexCount - 1)) * chunkSize;

				double nx = (vertex.position.x + pos.x * chunkSize);
				double ny = (vertex.position.z + pos.y * chunkSize);
				vertex.position.y = sampleHeight(nx, ny);

				vertex.normal = glm::vec3(0, 0, 0);	

				vertex.texture.x = x / (float)(vertexCount - 1);
				vertex.texture.y = y / (float)(vertexCount - 1);

				vertices.push_back(vertex);
			}
		}

		for (int y = 0; y < vertexCount - 1; y++) {
			for (int x = 0; x < vertexCount - 1; x++) {
				indices.push_back(y * vertexCount + x);
				indices.push_back((y + 1) * vertexCount + x);
				indices.push_back((y + 1) * vertexCount + x + 1);

				indices.push_back(y * vertexCount + x);
				indices.push_back((y + 1) * vertexCount + x + 1);
				indices.push_back(y * vertexCount + x + 1);

				Vertex& v0 = vertices[y * vertexCount + x];
				Vertex& v1 = vertices[(y + 1) * vertexCount + x];
				Vertex& v2 = vertices[(y + 1) * vertexCount + x + 1];
				Vertex& v3 = vertices[y * vertexCount + x + 1];

				glm::vec3 normal = glm::normalize(glm::cross(v1.position - v0.position, v2.position - v0.position));
				v0.normal += normal;
				v1.normal += normal;
				v2.normal += normal;

				
				glm::vec3 tangent = glm::normalize(glm::cross(v1.position - v0.position, v2.position - v0.position));
				v0.tangent += tangent;
				v1.tangent += tangent;
				v2.tangent += tangent;

				glm::vec3 bitangent = glm::normalize(glm::cross(v1.position - v0.position, v2.position - v0.position));
				v0.bitangent += bitangent;
				v1.bitangent += bitangent;
				v2.bitangent += bitangent;

				normal = glm::normalize(glm::cross(v2.position - v0.position, v3.position - v0.position));
				v0.normal += normal;
				v2.normal += normal;
				v3.normal += normal;

				tangent = glm::normalize(glm::cross(v2.position - v0.position, v3.position - v0.position));
				v0.tangent += tangent;
				v2.tangent += tangent;
				v3.tangent += tangent;

				bitangent = glm::normalize(glm::cross(v2.position - v0.position, v3.position - v0.position));
				v0.bitangent += bitangent;
				v2.bitangent += bitangent;
				v3.bitangent += bitangent;
			}
		}

		mesh->updateVertices();
		mesh->updateElements();

		Tree& tree = trees[pos.x + pos.y * worldSize];
		tree.clear();
		std::vector<glm::mat4> instances;
		for (int i = 0; i < 100; i++) {
			float x = rand() % chunkSize;
			float z = rand() % chunkSize;
			x += pos.x * chunkSize;
			z += pos.y * chunkSize;
			float y = sampleHeight(x, z);
			glm::mat4 model = Mesh::toModelMatrix(glm::vec3(x, y, z), glm::vec3(0, 0, 0), glm::vec3(0.05, 0.05, 0.05));
			instances.push_back(model);
		}
		tree.add(instances);
	}

	void generateWorld(int vertexCount) {
		for (int y = 0; y < worldSize; y++) {
			for (int x = 0; x < worldSize; x++) {
				generateChunk(glm::ivec2(x, y), vertexCount);
			}
		}
	}

	const ChunkMap& getChunks() const {
		return chunks;
	}

	const TreeMap& getTrees() const {
		return trees;
	}

private:
	int seed;
	int chunkSize;
	int worldSize;
	std::shared_ptr<Material> material;

	siv::PerlinNoise perlin;

	ChunkMap chunks;
	TreeMap trees;

	float sampleHeight(float x, float z) {
		float h = perlin.octave2D_01(x / 10, z / 10, 4) * 10;
		h *= perlin.octave2D_01(x * 0.1, z * 0.1, 4);
		return h;
	}
};


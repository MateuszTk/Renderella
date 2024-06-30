#pragma once

#include "Mesh.hpp"
#include "PerlinNoise.hpp"
/*
struct Comparator {
	size_t operator()(const glm::vec2& k) const {
		size_t h1 = std::hash<float>()(k.x);
		size_t h2 = std::hash<float>()(k.y);
		return h1 ^ h2;
	}

	bool operator()(const glm::vec2& a, const glm::vec2& b) const {
		return a.x == b.x && a.y == b.y;
	}
};

using ChunkMap = std::map<glm::vec2, std::shared_ptr<Mesh>, Comparator, Comparator>;
*/

using ChunkMap = std::map<int, std::shared_ptr<Mesh>>;

class WorldGen {
public:
	WorldGen(int seed, int chunkSize, int worldSize, std::shared_ptr<Material> material)
		: seed(seed), chunkSize(chunkSize), worldSize(worldSize), material(material), perlin(seed) {
		for (int y = 0; y < worldSize; y++) {
			for (int x = 0; x < worldSize; x++) {
				SubMesh submesh;
				submesh.material = material;
				chunks[x + y * worldSize * chunkSize] = (std::make_shared<Mesh>(std::vector<Vertex>(), std::vector<SubMesh>({submesh})));
			}
		}
	}

	void generateChunk(glm::ivec2 pos, int vertexCount) {
		std::shared_ptr<Mesh>& mesh = chunks[pos.x + pos.y * worldSize * chunkSize]; //chunks[pos];
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

				double nx = (vertex.position.x + pos.x * chunkSize) / 10;
				double ny = (vertex.position.z + pos.y * chunkSize) / 10;
				vertex.position.y = perlin.octave2D_01(nx, ny, 4) * 10;
				vertex.position.y *= perlin.octave2D_01(nx * 0.05, ny * 0.05, 4);

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

private:
	int seed;
	int chunkSize;
	int worldSize;
	std::shared_ptr<Material> material;

	siv::PerlinNoise perlin;

	ChunkMap chunks;
};


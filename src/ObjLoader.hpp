#pragma once
#include "Mesh.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <algorithm>
#include "Material.hpp"
#include "Texture.hpp"
#include <memory>
#include <map>

class ObjLoader {
public:
	static std::unique_ptr<std::map<std::string, Material>> loadMtl(std::string path, std::shared_ptr<ShaderProgram> shaderProgram) {
		if (path.find(".obj") != std::string::npos) {
			path = path.substr(0, path.length() - 3) + "mtl";
			std::cout << "[mtl] Loading mtl file: " << path << "\n";
		}

		auto materials = std::make_unique<std::map<std::string, Material>>();
		std::string directry = path.substr(0, path.find_last_of("/\\") + 1);

		std::ifstream file(path);
		if (!file.is_open()) {
			std::cout << "[mtl] Error opening mtl file\n";
		}
		else {
			std::string currentMaterialName = "";
			std::string line;
			while (std::getline(file, line)) {
				if (line[0] == 'n' && line[1] == 'e' && line[2] == 'w') {
					// newmtl
					std::istringstream iss(line.substr(7));
					iss >> currentMaterialName;
					//std::cout << "Found material: " << currentMaterialName << "\n";
					materials->insert({ currentMaterialName, Material(shaderProgram, currentMaterialName) });
				}
				else {
					auto option = line.substr(0, 6);
					if (option == "map_Kd") {
						std::string texturePath = directry + line.substr(7);
						materials->at(currentMaterialName).addTexture("texture1", std::make_shared<Texture>(texturePath));
					}
				}
			}
		}

		return materials;
	}

	static std::vector<Mesh> load(const std::string& path, std::shared_ptr<ShaderProgram> shaderProgram = nullptr) {
		std::vector<Mesh> meshes;
		std::vector<Vertex> vertices;
		std::vector<glm::vec2> textureCoords;
		SubMesh subMesh;
		std::unique_ptr<std::map<std::string, Material>> materials = nullptr;
		unsigned int vertexOffset = 0;

		std::ifstream file(path);
		if (!file.is_open()) {
			std::cout << "[obj] Error opening obj file\n";
		}
		else {
			std::cout << "[obj] Loading obj file: " << path << "\n";
			std::string line;
			while (std::getline(file, line)) {
				if (line[0] == 'o') {
					if (subMesh.elements.size() > 0) {
						vertexOffset += vertices.size();
						meshes.push_back(Mesh(vertices, { subMesh }));
						vertices.clear();
						subMesh.elements.clear();
					}
					std::cout << '.';
				}
				else if (line[0] == 'v') {
					if (line[1] == ' ') {
						std::istringstream iss(line.substr(2));
						Vertex vertex;
						iss >> vertex.x >> vertex.y >> vertex.z;
						vertices.push_back(vertex);
					}
					else if (line[1] == 't') {
						std::istringstream iss(line.substr(3));
						glm::vec2 uv;
						iss >> uv.x >> uv.y;
						textureCoords.push_back(uv);
					}
				}
				else if (line[0] == 'f' && line[1] == ' ') {
					std::istringstream iss(line.substr(2));
					//vertexstring is a string of the form "vertexIndex/textureIndex/normalIndex"
					std::string vertexString;
					while (iss >> vertexString) {
						unsigned int vertexIndex = std::stoi(vertexString);
						unsigned int textureIndex = 0;
						unsigned int normalIndex = 0;

						int separatorIndex = vertexString.find('/');
						if (separatorIndex != std::string::npos) {
							vertexString = vertexString.substr(separatorIndex + 1);
							textureIndex = std::stoi(vertexString);

							separatorIndex = vertexString.find('/');
							if (separatorIndex != std::string::npos) {
								normalIndex = std::stoi(vertexString.substr(separatorIndex + 1));
							}
						}

						unsigned int localVertexIndex = vertexIndex - 1 - vertexOffset;
						if (textureIndex > 0) {
							vertices[localVertexIndex].textureX = textureCoords[textureIndex - 1].x;
							vertices[localVertexIndex].textureY = textureCoords[textureIndex - 1].y;
						}
						subMesh.elements.push_back(localVertexIndex);
					}
				}
				else if (line[0] == 'u' && line[1] == 's') {
					//usemtl
					if (materials != nullptr) {
						try {
							subMesh.material = materials->at(line.substr(7));
						}
						catch (std::out_of_range e) {
							std::cout << "[obj] Material not matched: " << line.substr(7) << "\n";
						}
					}
				}
				else if (line[0] == 'm' && line[1] == 't') {
					//mtllib
					std::string mtlPath = path.substr(0, path.find_last_of("/\\") + 1) + line.substr(7);
					materials = loadMtl(mtlPath, shaderProgram);
				}
			}
			file.close();
			std::cout << '\n';
		}

		if (subMesh.elements.size() > 0) {
			meshes.push_back(Mesh(vertices, { subMesh }));
		}

		std::cout << "[obj] Loaded \'" << path << "\' (" << meshes.size() << " mesh" << ((meshes.size() > 1) ? "es" : "") << ")\n";

		return meshes;
	}
};

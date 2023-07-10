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
#include "PhongMat.hpp"
#include "Texture.hpp"
#include <memory>
#include <map>

class ObjLoader {
public:
	static std::unique_ptr<std::map<std::string, PhongMat>> loadMtl(std::string path) {
		if (path.find(".obj") != std::string::npos) {
			path = path.substr(0, path.length() - 3) + "mtl";
			std::cout << "[mtl] Loading mtl file: " << path << "\n";
		}

		auto materials = std::make_unique<std::map<std::string, PhongMat>>();
		std::string directry = path.substr(0, path.find_last_of("/\\") + 1);

		std::ifstream file(path);
		if (!file.is_open()) {
			std::cout << "[mtl] Error opening mtl file\n";
		}
		else {
			std::string currentMaterialName = "";
			std::string line;
			while (std::getline(file, line)) {
				// Remove leading whitespace
				int offset = 0;
				while (offset < line.size() && line[offset] <= ' ') {
					offset++;
				}
				line = line.substr(offset);

				if (line[0] == 'n' && line[1] == 'e' && line[2] == 'w') {
					// newmtl
					std::istringstream iss(line.substr(7));
					iss >> currentMaterialName;
					//std::cout << "Found material: " << currentMaterialName << "\n";
					PhongMat material(currentMaterialName);
					materials->insert({ currentMaterialName, material });
				}
				else if (line[0] == 'N' && line[1] == 's') {
					float shininess = std::stof(line.substr(3));
					materials->at(currentMaterialName).setShininess(shininess);
				}
				else if (line[0] == 'K' && line[1] == 's'){
					// specular
					// TODO: parse specular color as color
					std::istringstream iss(line.substr(3));
					float x;
					iss >> x;
					materials->at(currentMaterialName).setSpecular(x);
				}
				else {
					auto option = line.substr(0, 6);
					if (option == "map_Kd") {
						std::string texturePath = directry + line.substr(7);
						auto texture = std::make_shared<Texture>(texturePath);
						if (texture->getNrChannels() != 0) {
							materials->at(currentMaterialName).setDiffuseMap(texture);
						}
					}
					else if (option == "map_Bu" || option == "map_bu") {
						std::string texturePath = directry + line.substr(9);
						auto texture = std::make_shared<Texture>(texturePath);
						if (texture->getNrChannels() != 0) {
							materials->at(currentMaterialName).setNormalMap(texture);
						}
					}
				}
			}
		}

		return materials;
	}
	
	// path: path to obj file; material: material template for creating new materials
	static std::vector<Mesh> load(const std::string& path) {
		std::vector<Mesh> meshes;
		std::vector<Vertex> vertices;
		std::vector<glm::vec2> textureCoords;
		std::vector<glm::vec3> normals;
		std::vector<SubMesh> subMeshes = { SubMesh() };
		std::unique_ptr<std::map<std::string, PhongMat>> materials = nullptr;
		SubMesh* currentSubMesh = &(subMeshes[0]);
		unsigned int vertexOffset = 0;

		std::ifstream file(path);
		if (!file.is_open()) {
			std::cout << "[obj] Error opening obj file\n";
		}
		else {
			std::stringstream fileBuffer;
			fileBuffer << file.rdbuf();
			file.close();

			std::cout << "[obj] Loading obj file: " << path << "\n";
			std::string line;
			while (std::getline(fileBuffer, line)) {
				if (line[0] == 'o') {
					if (subMeshes.size() > 1 || (subMeshes.size() == 1 && subMeshes[0].elements.size() > 0)) {
						meshes.push_back(Mesh(vertices, subMeshes));
					}
					vertexOffset += vertices.size();
					vertices.clear();
					subMeshes.clear();
					subMeshes.push_back(SubMesh());
					currentSubMesh = &(subMeshes[0]);
					std::cout << '.';
				}
				else if (line[0] == 'g') {
					//group
					//if (subMeshes.back().elements.size() > 0) {
					//	subMeshes.push_back(SubMesh());
					//}
				}
				else if (line[0] == 'v') {
					if (line[1] == ' ') {
						std::istringstream iss(line.substr(2));
						Vertex vertex;
						iss >> vertex.position.x >> vertex.position.y >> vertex.position.z;
						vertices.push_back(vertex);
					}
					else if (line[1] == 't') {
						std::istringstream iss(line.substr(3));
						glm::vec2 uv;
						iss >> uv.x >> uv.y;
						textureCoords.push_back(uv);
					}
					else if (line[1] == 'n') {
						std::istringstream iss(line.substr(3));
						glm::vec3 normal;
						iss >> normal.x >> normal.y >> normal.z;
						normals.push_back(normal);
					}
				}
				else if (line[0] == 'f' && line[1] == ' ') {
					std::istringstream iss(line.substr(2));
					//vertexstring is a string of the form "vertexIndex/textureIndex/normalIndex"
					std::string vertexString;
					while (iss >> vertexString) {
						int vertexIndex = std::stoi(vertexString);
						if (vertexIndex < 0) {
							vertexIndex = vertices.size() + vertexIndex + 1;
						}
						int textureIndex = 0;
						int normalIndex = 0;

						int separatorIndex = vertexString.find('/');
						if (separatorIndex != std::string::npos) {
							vertexString = vertexString.substr(separatorIndex + 1);
							if (vertexString[0] != '/') {
								textureIndex = std::stoi(vertexString);
								if (textureIndex < 0) {
									textureIndex = textureCoords.size() + textureIndex + 1;
								}
							}

							separatorIndex = vertexString.find('/');
							if (separatorIndex != std::string::npos) {
								normalIndex = std::stoi(vertexString.substr(separatorIndex + 1));
								if (normalIndex < 0) {
									normalIndex = normals.size() + normalIndex + 1;
								}
							}
						}

						int localVertexIndex = vertexIndex - 1 - vertexOffset;
						if (textureIndex > 0) {
							vertices[localVertexIndex].texture = textureCoords[textureIndex - 1];
						}
						if (normalIndex > 0) {
							vertices[localVertexIndex].normal = normals[normalIndex - 1];
						}
						currentSubMesh->elements.push_back(localVertexIndex);
					}
					// calculate tangent
					Vertex& vert0 = vertices[currentSubMesh->elements[currentSubMesh->elements.size() - 3]];
					Vertex& vert1 = vertices[currentSubMesh->elements[currentSubMesh->elements.size() - 2]];
					Vertex& vert2 = vertices[currentSubMesh->elements[currentSubMesh->elements.size() - 1]];

					glm::vec3 v0 = vert0.position;
					glm::vec3 v1 = vert1.position;
					glm::vec3 v2 = vert2.position;

					glm::vec2 uv0 = vert0.texture;
					glm::vec2 uv1 = vert1.texture;
					glm::vec2 uv2 = vert2.texture;

					glm::vec3 edge1 = v1 - v0;
					glm::vec3 edge2 = v2 - v0;

					glm::vec2 deltaUV1 = uv1 - uv0;
					glm::vec2 deltaUV2 = uv2 - uv0;

					float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
					glm::vec3 tangent = (edge1 * deltaUV2.y - edge2 * deltaUV1.y) * r;
					glm::vec3 bitangent = (edge2 * deltaUV1.x - edge1 * deltaUV2.x) * r;

					vert0.tangent = tangent;
					vert1.tangent = tangent;
					vert2.tangent = tangent;
					// TODO: bitangent could be calculated in shader, but for some reason it produces artifacts
					vert0.bitangent = bitangent;
					vert1.bitangent = bitangent;
					vert2.bitangent = bitangent;
				}
				else if (line[0] == 'u' && line[1] == 's') {
					//usemtl
					if (materials != nullptr) {
						// find if submesh with this material already exists
						bool found = false;
						std::string matName = line.substr(7);
						for (auto& subMesh : subMeshes) {
							const std::string& curMatName = subMesh.material.getName();
							if (curMatName.length() == 0 || curMatName == matName) {
								found = true;
								currentSubMesh = &subMesh;
								break;
							}
						}

						if (!found || currentSubMesh->material.getName().length() == 0) {
							if (!found) {
								// create new submesh
								subMeshes.push_back(SubMesh());
								currentSubMesh = &(subMeshes.back());
							}
							// assign material
							try {
								currentSubMesh->material = materials->at(line.substr(7));
							}
							catch (std::out_of_range e) {
								std::cout << "[obj] Material not matched: " << line.substr(7) << "\n";
							}
						}
					}
				}
				else if (line[0] == 'm' && line[1] == 't') {
					//mtllib
					std::string mtlPath = path.substr(0, path.find_last_of("/\\") + 1) + line.substr(7);
					materials = loadMtl(mtlPath);
				}
			}
			std::cout << '\n';
		}

		for (Vertex& vertex : vertices) {
			vertex.normal = glm::normalize(vertex.normal);
			vertex.tangent = glm::normalize(vertex.tangent);
			vertex.bitangent = glm::normalize(vertex.bitangent);
		}

		if (subMeshes.size() > 1 || (subMeshes.size() == 1 && subMeshes[0].elements.size() > 0)) {
			meshes.push_back(Mesh(vertices, subMeshes));
		}

		std::cout << "[obj] Loaded \'" << path << "\' (" << meshes.size() << " mesh" << ((meshes.size() > 1) ? "es" : "") << ")\n";
		for (int i = 0; i < meshes.size(); i++) {
			std::cout << "   [" << i << "] (" << subMeshes.size() << "submesh" << ((subMeshes.size() > 1) ? "es" : "") << ")\n";
		}	
		std::cout << '\n';
		return meshes;
	}
};

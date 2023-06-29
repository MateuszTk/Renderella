#pragma once
#include "Mesh.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <algorithm>

class ObjLoader {
public:
	static std::vector<Mesh> load(std::string path) {
		std::vector<Mesh> meshes;
		std::vector<Vertex> vertices;
		SubMesh subMesh;
		unsigned int vertexOffset = 0;

		std::ifstream file(path);
		if (!file.is_open()) {
			std::cout << "Error opening obj file\n";
		}
		else {
			std::string line;
			while (std::getline(file, line)) {
				if (line[0] == 'o') {
					if (subMesh.elements.size() > 0) {
						vertexOffset += vertices.size();
						meshes.push_back(Mesh(vertices, { subMesh }));
						vertices.clear();
						subMesh.elements.clear();
					}
				}
				else if (line[0] == 'v' && line[1] == ' ') {
					std::istringstream iss(line.substr(2));
					Vertex vertex;
					iss >> vertex.x >> vertex.y >> vertex.z;

					//temporarily generate random texture coordinates to make the mesh look less flat
					vertex.textureX = (rand() % 256) / 255.0f;
					vertex.textureY = (rand() % 256) / 255.0f;

					vertices.push_back(vertex);
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
							std::string textureString = vertexString.substr(separatorIndex + 1);
							textureIndex = std::stoi(textureString);

							separatorIndex = textureString.find('/');
							if (separatorIndex != std::string::npos) {
								normalIndex = std::stoi(textureString.substr(separatorIndex + 1));
							}
						}

						subMesh.elements.push_back(vertexIndex - 1 - vertexOffset);
					}
				}
			}
			file.close();
		}

		if (subMesh.elements.size() > 0) {
			meshes.push_back(Mesh(vertices, { subMesh }));
		}

		return meshes;
	}
};

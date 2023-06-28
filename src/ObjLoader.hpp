#pragma once
#include "Mesh.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

class ObjLoader {
public:
	static Mesh load(std::string path) {
		std::vector<Vertex> vertices;
		SubMesh subMesh;

		std::ifstream file(path);
		if (!file.is_open()) {
			std::cout << "Error opening obj file\n";
		}
		else {
			std::string line;
			while (std::getline(file, line)) {
				if (line[0] == 'v' && line[1] == ' ') {
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
					unsigned int vertexIndex;
					while (iss >> vertexIndex) {
						subMesh.elements.push_back(vertexIndex - 1);
					}
				}
			}
			file.close();
		}

		return std::move(Mesh(vertices, { subMesh }));
	}
};


#include "ObjLoader.hpp"

std::unique_ptr<std::map<std::string, std::shared_ptr<PhongMat>>> ObjLoader::loadMtl(std::string path) {
	if (path.find(".obj") != std::string::npos) {
		path = path.substr(0, path.length() - 3) + "mtl";
		std::cout << "[mtl] Loading mtl file: " << path << "\n";
	}

	auto materials = std::make_unique<std::map<std::string, std::shared_ptr<PhongMat>>>();
	std::string directry = path.substr(0, path.find_last_of("/\\") + 1);

	std::ifstream file(path);
	if (!file.is_open()) {
		std::cout << "[mtl] Error opening mtl file\n";
	}
	else {
		std::string currentMaterialName = "";
		std::string line;
		std::unique_ptr<TextureData> diffuseMap = nullptr;

		TextureLoader textureLoader;

		std::cout << "[mtl] Loading mtl file: " << path << "\n";

		while (std::getline(file, line)) {
			// Remove leading whitespace
			int offset = 0;
			while (offset < line.size() && line[offset] <= ' ') {
				offset++;
			}
			line = line.substr(offset);

			if (line[0] == 'n' && line[1] == 'e' && line[2] == 'w') {
				// newmtl

				// add previous albedo map
				if (diffuseMap != nullptr) {
					auto texture = textureLoader.create(*diffuseMap, "dM_" + diffuseMap->getPath());
					materials->at(currentMaterialName)->setDiffuseMap(texture);
					diffuseMap = nullptr;
				}

				// read material name
				std::istringstream iss(line.substr(7));
				iss >> currentMaterialName;
				//std::cout << "Found material: " << currentMaterialName << "\n";
				auto material = std::make_shared<PhongMat>(currentMaterialName);
				materials->insert({ currentMaterialName, material });
			}
			else if (line[0] == 'N' && line[1] == 's') {
				float shininess = std::stof(line.substr(3));
				materials->at(currentMaterialName)->setShininess(shininess);
			}
			else if (line[0] == 'K' && line[1] == 's') {
				// specular
				std::istringstream iss(line.substr(3));
				glm::vec3 specular;
				iss >> specular.x >> specular.y >> specular.z;
				materials->at(currentMaterialName)->setSpecular(specular);
			}
			else if (line[0] == 'K' && line[1] == 'd') {
				// color
				std::istringstream iss(line.substr(3));
				glm::vec3 color;
				iss >> color.x >> color.y >> color.z;
				materials->at(currentMaterialName)->setDiffuse(color);
			}
			else {
				auto option = line.substr(0, 6);
				if (option == "map_Kd") {
					std::string diffuseMapPath = directry + line.substr(7);
					diffuseMap = std::make_unique<TextureData>(diffuseMapPath);
				}
				else if (option == "map_Bu" || option == "map_bu") {
					std::string name = line.substr(9);
					if (name[0] == '-') {
						name = name.substr(13);
					}
					std::string texturePath = directry + name;

					auto texture = textureLoader.load(texturePath);
					if (texture->getNrChannels() != 0) {
						materials->at(currentMaterialName)->setNormalMap(texture);
					}
				}
				else if (option == "map_d ") {
					std::string texturePath = directry + line.substr(6);
					if (texturePath != diffuseMap->getPath()) {
						TextureData textureData(texturePath);
						if (textureData.getChannels() != 0 && diffuseMap != nullptr) {
							diffuseMap->addAlpha(textureData);
						}
					}
				}
				else if (option == "map_Ks") {
					std::string texturePath = directry + line.substr(7);
					TextureData textureData(texturePath);
					textureData.optimizeAlphaOnly();
					auto texture = textureLoader.create(textureData, "Ks_" + texturePath);
					if (texture->getNrChannels() != 0) {
						materials->at(currentMaterialName)->setSpecularMap(texture);
					}
				}
				else if (option == "map_re") {
					// map_refl
					// TODO: should be metalness map
					std::string texturePath = directry + line.substr(9);
					auto texture = textureLoader.load(texturePath);
					if (texture->getNrChannels() != 0) {
						materials->at(currentMaterialName)->setSpecularMap(texture);
					}
				}
				else if (option == "map_Pm") {
					// TODO: should be metalness map
					std::string texturePath = directry + line.substr(7);
					auto texture = textureLoader.load(texturePath);
					if (texture->getNrChannels() != 0) {
						materials->at(currentMaterialName)->setSpecularMap(texture);
					}
				}
				else if (option == "map_Ns") {
					std::string texturePath = directry + line.substr(7);
					TextureData textureData(texturePath);
					textureData.optimizeAlphaOnly();
					auto texture = textureLoader.create(textureData, "Ns_" + texturePath);
					if (texture->getNrChannels() != 0) {
						materials->at(currentMaterialName)->setShininessMap(texture);
					}
				}
				else if (option == "map_Pr") {
					// roughness map
					std::string texturePath = directry + line.substr(7);
					TextureData textureData(texturePath);
					textureData.optimizeAlphaOnly();
					textureData.invert();
					auto texture = textureLoader.create(textureData, "Pr_" + texturePath);
					if (texture->getNrChannels() != 0) {
						materials->at(currentMaterialName)->setShininessMap(texture);
					}
				}
				else if (option == "illum ") {
					int illum = std::stoi(line.substr(6));
					if (illum == 4 || illum == 6 || illum == 7 || illum == 9) {
						// transparency mode
						materials->at(currentMaterialName)->setBlendMode(Material::BlendMode::ALPHA_BLEND);
					}
				}
			}
		}
		if (diffuseMap != nullptr) {
			auto texture = textureLoader.create(*diffuseMap, "dM_" + diffuseMap->getPath());
			materials->at(currentMaterialName)->setDiffuseMap(texture);
		}
	}

	return materials;
}

// path: path to obj file; material: material template for creating new materials
std::list<std::shared_ptr<Mesh>> ObjLoader::load(const std::string& path) {
	std::list<std::shared_ptr<Mesh>> meshes;
	std::vector<Vertex> vertices;
	std::vector<int> verticesDuplicatesLinkedList;
	std::vector<glm::vec2> textureCoords;
	std::vector<glm::vec3> normals;
	std::vector<SubMesh> subMeshes = { SubMesh() };
	std::unique_ptr<std::map<std::string, std::shared_ptr<PhongMat>>> materials = nullptr;
	SubMesh* currentSubMesh = &(subMeshes[0]);
	unsigned int vertexOffset = 0;
	unsigned int extraVertexCount = 0;
	unsigned int extraVertexOffset = 0;
	std::string activeMaterialName = "";

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
					meshes.push_back(std::make_shared<Mesh>(vertices, subMeshes));
					int submeshCount = meshes.back()->getSubmeshes().size();
					std::cout << "   [" << meshes.size() << "] (" << submeshCount << "submesh" << ((submeshCount > 1) ? "es), (" : "), (")
						<< vertices.size() - extraVertexCount << " + " << extraVertexCount << " = " << vertices.size() << " : " << (vertices.size() * 100) / (vertices.size() - extraVertexCount) << "% vertices)\n";
				}
				vertexOffset += vertices.size() - extraVertexCount;
				vertices.clear();
				extraVertexCount = 0;
				verticesDuplicatesLinkedList.clear();
				subMeshes.clear();
				SubMesh subMesh;
				try {
					subMesh.material = materials->at(activeMaterialName);
				}
				catch (std::out_of_range e) {

				}
				subMeshes.push_back(std::move(subMesh));
				currentSubMesh = &(subMeshes[0]);
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
					verticesDuplicatesLinkedList.push_back(-1);
					extraVertexOffset = extraVertexCount;
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
				int vertexNumber = 0;
				while (iss >> vertexString) {
					vertexNumber++;
					int vertexIndex = std::stoi(vertexString);
					if (vertexIndex < 0) {
						vertexIndex = vertices.size() + vertexIndex + 1 + extraVertexOffset - extraVertexCount;
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

					int localVertexIndex = vertexIndex - 1 - vertexOffset + extraVertexOffset;
					Vertex& vert = vertices[localVertexIndex];
					Vertex newVertex = vert;
					if (textureIndex > 0) {
						newVertex.texture = textureCoords[textureIndex - 1];
					}
					if (normalIndex > 0) {
						newVertex.normal = normals[normalIndex - 1];
					}

					if (vert.normal == glm::vec3(0.0f) && vert.texture == glm::vec2(0.0f)) {
						// this vertex has not been processed yet
						vert = newVertex;
						currentSubMesh->elements.push_back(localVertexIndex);
					}
					else {
						if (vert.texture == newVertex.texture && vert.normal == newVertex.normal) {
							// this vertex has been processed and is the same as the new one
							currentSubMesh->elements.push_back(localVertexIndex);
						}
						else {
							while (true) {
								// this vertex has been processed and is different from the new one
								// check if there is a vertex with the same position and texture
								int duplicateVertexIndex = verticesDuplicatesLinkedList[localVertexIndex];
								if (duplicateVertexIndex == -1) {
									// there is no vertex with the same position and texture
									// create a new vertex
									vertices.push_back(newVertex);
									verticesDuplicatesLinkedList.push_back(-1);
									verticesDuplicatesLinkedList[localVertexIndex] = vertices.size() - 1;
									currentSubMesh->elements.push_back(vertices.size() - 1);
									extraVertexCount++;
									break;
								}
								else {
									if (vertices[duplicateVertexIndex].texture == newVertex.texture && vertices[duplicateVertexIndex].normal == newVertex.normal) {
										// there is a vertex with the same position and texture, use it
										currentSubMesh->elements.push_back(duplicateVertexIndex);
										break;
									}
									else {
										localVertexIndex = duplicateVertexIndex;
									}
								}
							}
						}
					}
				}

				if (vertexNumber > 3) {
					// triangulate
					int firstVertex = currentSubMesh->elements[currentSubMesh->elements.size() - 4];
					int secondVertex = currentSubMesh->elements[currentSubMesh->elements.size() - 3];
					int thirdVertex = currentSubMesh->elements[currentSubMesh->elements.size() - 2];
					int fourthVertex = currentSubMesh->elements[currentSubMesh->elements.size() - 1];
					currentSubMesh->elements.pop_back();

					currentSubMesh->elements.push_back(thirdVertex);
					currentSubMesh->elements.push_back(fourthVertex);
					currentSubMesh->elements.push_back(firstVertex);
				}

				const int triangleCount = (vertexNumber < 4) ? 1 : 2;
				for (int triangle = 0; triangle < triangleCount; triangle++) {
					// calculate tangent
					Vertex& vert0 = vertices[currentSubMesh->elements[currentSubMesh->elements.size() - 3 - triangle * 3]];
					Vertex& vert1 = vertices[currentSubMesh->elements[currentSubMesh->elements.size() - 2 - triangle * 3]];
					Vertex& vert2 = vertices[currentSubMesh->elements[currentSubMesh->elements.size() - 1 - triangle * 3]];

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
			}
			else if (line[0] == 'u' && line[1] == 's') {
				//usemtl
				if (materials != nullptr) {
					// find if submesh with this material already exists
					bool found = false;
					activeMaterialName = line.substr(7);
					for (auto& subMesh : subMeshes) {
						if (subMesh.material == nullptr || subMesh.material->getName() == activeMaterialName) {
							found = true;
							currentSubMesh = &subMesh;
							break;
						}
					}

					if (!found || currentSubMesh->material == nullptr) {
						// if submesh with desired material not found and current submesh is not empty; else reuse current submesh by applying new material
						if (!found && (currentSubMesh->elements.size() != 0)) {
							// create new submesh
							subMeshes.push_back(SubMesh());
							currentSubMesh = &(subMeshes.back());
						}
						// assign material
						try {
							currentSubMesh->material = materials->at(activeMaterialName);
						}
						catch (std::out_of_range e) {
							std::cout << "[obj] Material not matched: " << activeMaterialName << "\n";
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
	}

	if (subMeshes.size() > 1 || (subMeshes.size() == 1 && subMeshes[0].elements.size() > 0)) {
		meshes.push_back(std::make_shared<Mesh>(vertices, subMeshes));
		int submeshCount = meshes.back()->getSubmeshes().size();
		std::cout << "   [" << meshes.size() << "] (" << submeshCount << "submesh" << ((submeshCount > 1) ? "es), (" : "), (")
			<< vertices.size() - extraVertexCount << " + " << extraVertexCount << " = " << vertices.size() << " : " << (vertices.size() * 100) / (vertices.size() - extraVertexCount) << "% vertices)\n";
	}

	std::cout << "[obj] Loaded \'" << path << "\' (" << meshes.size() << " mesh" << ((meshes.size() > 1) ? "es" : "") << ")\n";
	return meshes;
}

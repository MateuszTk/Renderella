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
#include "TextureData.hpp"
#include <list>
#include <memory>
#include <map>
#include "TextureLoader.hpp"

class ObjLoader {

	public:

		static std::unique_ptr<std::map<std::string, std::shared_ptr<PhongMat>>> loadMtl(std::string path);
	
		// path: path to obj file; material: material template for creating new materials
		static std::list<std::shared_ptr<Mesh>> load(const std::string& path);

};

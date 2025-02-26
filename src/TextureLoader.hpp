#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <iostream>
#include "Texture.hpp"

class TextureLoader {

	public:

		TextureLoader() = default;

		std::shared_ptr<Texture> load(const std::string& path);

		std::shared_ptr<Texture> create(const TextureData& data, std::string name = "");

	private:

		std::map<std::string, std::shared_ptr<Texture>> textures;

};

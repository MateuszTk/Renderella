
#include "TextureLoader.hpp"

std::shared_ptr<Texture> TextureLoader::load(const std::string& path) {
	if (textures.find(path) != textures.end()) {
		std::cout << "Reusing texture: " << path << "\n";
		return textures[path];
	}
	else {
		std::shared_ptr<Texture> texture = std::make_shared<Texture>(path);
		textures[path] = texture;
		return texture;
	}
}

std::shared_ptr<Texture> TextureLoader::create(const TextureData& data, std::string name) {
	if (name.empty()) {
		name = data.getName();
	}
	if (textures.find(name) != textures.end()) {
		std::cout << "Reusing texture: " << name << "\n";
		return textures[name];
	}
	else {
		std::shared_ptr<Texture> texture = std::make_shared<Texture>(data);
		textures[name] = texture;
		return texture;
	}
}

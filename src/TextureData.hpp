#pragma once
#include <iostream>
#include "stb_image.h"
#include <string>
#include <fstream>

#ifndef CACHE_LOCATION
	#define CACHE_LOCATION "cache/"
#endif

class TextureData {
public:
	TextureData(const std::string& path) {
		this->path = path;
		this->name = path.substr(path.find_last_of("/\\") + 1);
		if (!loadTextureCache()) {
			stbi_set_flip_vertically_on_load(true);
			this->data = stbi_load(path.c_str(), &width, &height, &channels, 0);
			if (!this->data) {
				std::cout << "Error: failed to load texture \"" << path << "\"\n";
				width = 0;
				height = 0;
				channels = 0;
				data = nullptr;
			}
			else {
				createTextureCache();
			}
		}
	}

	unsigned char* getData() const {
		return this->data;
	}

	int getWidth() const {
		return this->width;
	}

	int getHeight() const {
		return this->height;
	}

	int getChannels() const {
		return this->channels;
	}

	std::string getName() const {
		return this->name;
	}

	std::string getPath() const {
		return this->path;
	}

	void addAlpha(const TextureData& mask) {
		if (this->width != mask.getWidth() || this->height != mask.getHeight()) {
			std::cout << "Error: opacity mask \"" << mask.getPath() << "\" does not match texture \"" << this->name << "\"\n";
			return;
		}

		if (mask.channels > 0 && this->channels > 0) {
			unsigned char* newData = (unsigned char*)malloc(this->width * this->height * 4);
			for (int i = 0; i < this->width * this->height; i++) {
				newData[4 * i + 0] = this->data[this->channels * i + 0];
				newData[4 * i + 1] = (this->channels > 1) ? this->data[this->channels * i + 1] : 0;
				newData[4 * i + 2] = (this->channels > 2) ? this->data[this->channels * i + 2] : 0;
				newData[4 * i + 3] = mask.getData()[mask.channels * i];
			}
			free(this->data);
			this->data = newData;
			this->channels = 4;
		}
		else {
			std::cout << "Error: opacity mask \"" << mask.getPath() << "\" has " << mask.getChannels() << " channels, expected 1\n";
		}
	}

	// if the color is all 0, make it single channel
	void optimizeAlphaOnly() {
		if (this->channels == 4) {
			// check if color is all 0
			bool allZero = true;
			for (int i = 0; i < this->width * this->height; i += 4) {
				if (this->data[i] != 0 && this->data[i + 1] != 0 && this->data[i + 2] != 0) {
					allZero = false;
					return;
				}
			}
			// if all 0, make it single channel
			if (allZero) {
				unsigned char* newData = (unsigned char*)malloc(this->width * this->height);
				for (int i = 0; i < this->width * this->height; i++) {
					unsigned char alpha = this->data[4 * i + 3];
					newData[i] = alpha;
				}
				free(this->data);
				this->data = newData;
				this->channels = 1;
			}
		}
	}

	void invert() {
		const int dataSize = this->width * this->height * this->channels;
		for (int i = 0; i < dataSize; i++) {
			unsigned char roughness = this->data[i];
			this->data[i] = 255 - roughness;
		}
	}

	~TextureData() {
		if (this->data != nullptr) {
			free(this->data);
		}
	}

private:
	unsigned char* data;
	int width;
	int height;
	int channels;
	std::string name;
	std::string path;

	void createTextureCache() {
#ifdef ENABLE_TEXTURE_CACHE
		std::size_t pathHash = std::hash<std::string>{}(this->path);
		std::ofstream file(CACHE_LOCATION + this->name + std::to_string(pathHash) + ".cache", std::ios::binary);
		if (file.is_open()) {

			file.write((char*)&this->width, sizeof(int));
			file.write((char*)&this->height, sizeof(int));
			file.write((char*)&this->channels, sizeof(int));
			file.write((char*)this->data, this->width * this->height * this->channels);
			file.close();
			std::cout << "Created texture cache for \"" << this->name << "\"\n";
		}
		else {
			std::cout << "Error: failed to create texture cache for \"" << this->name << "\"\n";
		}
#endif
	}

	bool loadTextureCache() {
#ifdef ENABLE_TEXTURE_CACHE
		std::size_t pathHash = std::hash<std::string>{}(this->path);
		std::ifstream file(CACHE_LOCATION + this->name + std::to_string(pathHash) + ".cache", std::ios::binary);
		if (file.is_open()) {
			file.read((char*)&this->width, sizeof(int));
			file.read((char*)&this->height, sizeof(int));
			file.read((char*)&this->channels, sizeof(int));
			this->data = (unsigned char*)malloc(this->width * this->height * this->channels);
			file.read((char*)this->data, this->width * this->height * this->channels);
			file.close();
			std::cout << "Loaded texture \"" << this->name << "\" from cache\n";
			return true;
		}
		else {
			return false;
		}
#else
		return false;
#endif
	}
};

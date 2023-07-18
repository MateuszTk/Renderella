#pragma once
#include <iostream>
#include "stb_image.h"
#include <string>

class TextureData {
public:
	TextureData(const std::string& path, const std::string& name = "") : name((name == "") ? path : name){
		stbi_set_flip_vertically_on_load(true);
		this->data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		if (!this->data) {
			std::cout << "Error: failed to load texture \"" << path << "\"\n";
			width = 0;
			height = 0;
			channels = 0;
			data = nullptr;
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

	void addAlpha(const TextureData& mask) {
		if (this->width != mask.getWidth() || this->height != mask.getHeight()) {
			std::cout << "Error: opacity mask \"" << mask.getName() << "\" does not match texture \"" << this->name << "\"\n";
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
			std::cout << "Error: opacity mask \"" << mask.getName() << "\" has " << mask.getChannels() << " channels, expected 1\n";
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
};

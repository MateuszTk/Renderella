#pragma once
#include <glad/glad.h>
#include <iostream>
#include "stb_image.h"
#include <string>
#include <memory>
#include "TextureData.hpp"

class Texture {
public:
	Texture(const TextureData& textureData) {
		this->master = true;
		glGenTextures(1, &this->texture);
		glBindTexture(GL_TEXTURE_2D, this->texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		this->width = textureData.getWidth();
		this->height = textureData.getHeight();
		this->nrChannels = textureData.getChannels();

		if (textureData.getData() != nullptr) {
			if (nrChannels == 3) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData.getData());
			}
			else if (nrChannels == 4) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData.getData());
			}
			else if (nrChannels == 2) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, textureData.getData());
			}
			else if (nrChannels == 1) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, textureData.getData());
				GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_RED };
				glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
			}
			else {
				std::cout << "Error: unsupported number of channels \"" << textureData.getPath() << "\"\n";
			}
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture(const std::string& path) : Texture(TextureData(path)) {

	}


	Texture(int width, int height, GLint internalFormat = GL_RGBA, const GLvoid* data = NULL) : width(width), height(height), nrChannels(4) {
		this->master = true;
		glGenTextures(1, &this->texture);
		glBindTexture(GL_TEXTURE_2D, this->texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture() {
		this->width = 0;
		this->height = 0;
		this->nrChannels = 0;
		this->master = true;
		glGenTextures(1, &this->texture);
	}

	Texture(Texture&& other) {
		this->width = other.width;
		this->height = other.height;
		this->nrChannels = other.nrChannels;
		this->texture = other.texture;
		this->master = true;
		other.master = false;
	}

	static std::shared_ptr<Texture> createColorTexture(int width, int height, glm::vec4 color){
		unsigned char* data = new unsigned char[width * height * 4];
		for (int i = 0; i < width * height; i++) {
			data[i * 4 + 0] = (unsigned char)(color.r * 255);
			data[i * 4 + 1] = (unsigned char)(color.g * 255);
			data[i * 4 + 2] = (unsigned char)(color.b * 255);
			data[i * 4 + 3] = (unsigned char)(color.a * 255);
		}
		std::shared_ptr<Texture> texture = std::make_shared<Texture>(width, height, GL_RGBA, data);
		delete[] data;
		return texture;
	}

	unsigned int getTexture() const {
		return this->texture;
	}

	void bind() const {
		glBindTexture(GL_TEXTURE_2D, this->texture);
	}

	void unbind() const {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	int getWidth() const {
		return this->width;
	}

	int getHeight() const {
		return this->height;
	}

	int getNrChannels() const {
		return this->nrChannels;
	}

	~Texture() {
		if (this->master) {
			glDeleteTextures(1, &this->texture);
		}
	}

private:
	unsigned int texture;
	bool master;
	int width;
	int height;
	int nrChannels;
};

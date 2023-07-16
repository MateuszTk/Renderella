#pragma once
#include <glad/glad.h>
#include <iostream>
#include "stb_image.h"
#include <string>

class Texture {
public:
	Texture(const std::string& path) {
		this->master = true;
		glGenTextures(1, &this->texture);
		glBindTexture(GL_TEXTURE_2D, this->texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			if (nrChannels == 3) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else if (nrChannels == 4) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}
			else if (nrChannels == 2) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, data);
			}
			else if (nrChannels == 1) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
				GLint swizzleMask[] = { GL_RED, GL_RED, GL_RED, GL_ONE };
				glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
			}
			else {
				std::cout << "Error: unsupported number of channels \"" << path << "\"\n";
			}
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Error: failed to load texture \"" << path << "\"\n";
			width = 0;
			height = 0;
			nrChannels = 0;
		}
		stbi_image_free(data);
		glBindTexture(GL_TEXTURE_2D, 0);
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
	int width, height, nrChannels;
};

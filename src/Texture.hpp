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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);

		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			if (nrChannels == 3) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else if (nrChannels == 4) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}
			else {
				std::cout << "Error: unsupported number of channels\n";
			}
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Error: failed to load texture \"" << path << "\"\n";
		}
		stbi_image_free(data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture(int width, int height, const GLvoid* data = NULL) {
		this->master = true;
		glGenTextures(1, &this->texture);
		glBindTexture(GL_TEXTURE_2D, this->texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Texture(Texture&& other) {
		this->texture = other.texture;
		this->master = true;
		other.master = false;
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

	~Texture() {
		if (this->master) {
			glDeleteTextures(1, &this->texture);
		}
	}

private:
	unsigned int texture;
	bool master;
};

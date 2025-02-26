#pragma once
#include <glad/glad.h>
#include <iostream>
#include "stb_image.h"
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include "TextureData.hpp"

class Texture {

	public:

		Texture(const TextureData& textureData);
		Texture(const std::string& path);
		Texture(int width, int height, GLint internalFormat = GL_RGBA, const GLvoid* data = NULL, int layers = 1, bool init = true);
		Texture();
		Texture(Texture&& other);
		~Texture();

		static std::shared_ptr<Texture> createColorTexture(int width, int height, glm::vec4 color);

		unsigned int getTexture() const;

		void bind() const;
		void unbind() const;

		int getWidth() const;
		int getHeight() const;
		int getLayers() const;
		int getNrChannels() const;	

	private:

		unsigned int texture;
		bool master;
		int width;
		int height;
		int layers;
		int nrChannels;

};

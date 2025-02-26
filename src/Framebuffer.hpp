#pragma once
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Texture.hpp"
#include "Mesh.hpp"
#include "ShaderProgram.hpp"
#include "Material.hpp"
#include "Primitives.hpp"

class Framebuffer {

	public:

		Framebuffer(unsigned int width, unsigned int height, int colorAttachments, bool floatColor = false, bool shadow = false, unsigned int layers = 1);
		~Framebuffer();

		// Produces a mesh with a plane that covers the entire screen with no framebuffer specific textures.
		static Mesh produceEmptyFbPlane(const char* vert, const char* frag);

		// Produces a mesh with a plane that covers the entire screen with the framebuffer's color and depth textures.
		Mesh produceFbPlane(const char* vert, const char* frag);

		const std::vector<std::shared_ptr<Texture>>& getColorTexs();
		std::shared_ptr<Texture> getDepthTex();

		unsigned int getFbo();

		int getWidth();
		int getHeight();

		void bind(bool setViewport = false);
		void unbind();

		static void bindDefault();	

	private:

		unsigned int fbo;
		std::shared_ptr<Texture> depthTex;
		std::vector<std::shared_ptr<Texture>> colorTexs;
		std::vector<unsigned int> attachments;
		int width, height, layers;

};

#pragma once
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Texture.hpp"
#include "Mesh.hpp"
#include "ShaderProgram.hpp"
#include "Primitives.hpp"

class Framebuffer {
public:
	Framebuffer(unsigned int width, unsigned int height, int colorAttachments, bool floatColor = false) : width(width), height(height) {
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		if (colorAttachments > 0) {
			colorTexs.reserve(colorAttachments);
			for (int i = 0; i < colorAttachments; i++) {
				GLint format = floatColor ? GL_RGBA16F : GL_RGBA;
				auto colorTex = std::make_shared<Texture>(width, height, format);
				colorTex->bind();
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorTex->getTexture(), 0);
				colorTex->unbind();
				colorTexs.push_back(colorTex);
				attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
			}
			glDrawBuffers(colorAttachments, attachments.data());
		}
		else {
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}

		depthTex = std::make_shared<Texture>();
		depthTex->bind();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex->getTexture(), 0);
		depthTex->unbind();

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "Framebuffer not complete!\n";
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// Produces a mesh with a plane that covers the entire screen with no framebuffer specific textures.
	static Mesh produceEmptyFbPlane(const char* vert, const char* frag) {
		Shader<GL_VERTEX_SHADER> screenVert(vert, true);
		Shader<GL_FRAGMENT_SHADER> screenFrag(frag, true);
		auto screenShader = std::make_shared<ShaderProgram>(screenVert, screenFrag);

		Material planeMat(screenShader);
		SubMesh planeSub(primitives::plane::planeInd, planeMat);
		return Mesh(primitives::plane::planeVert, { planeSub });
	}

	// Produces a mesh with a plane that covers the entire screen with the framebuffer's color and depth textures.
	Mesh produceFbPlane(const char* vert, const char* frag) {
		Shader<GL_VERTEX_SHADER> screenVert(vert, true);
		Shader<GL_FRAGMENT_SHADER> screenFrag(frag, true);
		auto screenShader = std::make_shared<ShaderProgram>(screenVert, screenFrag);

		Material planeMat(screenShader);
		for (int i = 0; i < colorTexs.size(); i++) {
			planeMat.setTexture("screenTexture" + std::to_string(i), colorTexs[i]);
		}
		planeMat.setTexture("depthTexture", depthTex);
		SubMesh planeSub(primitives::plane::planeInd, planeMat);
		return Mesh(primitives::plane::planeVert, { planeSub });
	}

	const std::vector<std::shared_ptr<Texture>>& getColorTexs() {
		return colorTexs;
	}

	std::shared_ptr<Texture> getDepthTex() {
		return depthTex;
	}

	unsigned int getFbo() {
		return fbo;
	}

	void bind(bool setViewport = false) {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		if (setViewport) {
			glViewport(0, 0, width, height);
		}
	}

	void unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	~Framebuffer() {
		glDeleteFramebuffers(1, &fbo);
	}

private:
	unsigned int fbo;
	std::shared_ptr<Texture> depthTex;
	std::vector<std::shared_ptr<Texture>> colorTexs;
	std::vector<unsigned int> attachments;
	int width, height;
};

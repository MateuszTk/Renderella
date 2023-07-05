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
	Framebuffer(unsigned int width, unsigned int height, bool color) : width(width), height(height) {
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		if (color) {
			texture = std::make_shared<Texture>(width, height);
			texture->bind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getTexture(), 0);
			texture->unbind();
		}
		else {
			texture = nullptr;
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

	Mesh produceFbPlane(const char* vert, const char* frag) {
		Shader<GL_VERTEX_SHADER> screenVert(vert, true);
		Shader<GL_FRAGMENT_SHADER> screenFrag(frag, true);
		auto screenShader = std::make_shared<ShaderProgram>(screenVert, screenFrag);

		Material planeMat(screenShader);
		planeMat.addTexture("screenTexture", texture);
		planeMat.addTexture("depthTexture", depthTex);
		SubMesh planeSub(primitives::plane::planeInd, planeMat);
		return Mesh(primitives::plane::planeVert, { planeSub });
	}

	std::shared_ptr<Texture> getTexture() {
		return texture;
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
	std::shared_ptr<Texture> texture;
	int width, height;
};

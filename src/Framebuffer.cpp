
#include "Framebuffer.hpp"

Framebuffer::Framebuffer(unsigned int width, unsigned int height, int colorAttachments, bool floatColor, bool shadow, unsigned int layers) 
	: width(width), height(height), layers(layers) {

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

	depthTex = std::make_shared<Texture>((int)width, (int)height, (GLint)GL_DEPTH_COMPONENT, (const GLvoid*)NULL, (int)layers, false);
	depthTex->bind();
	if (shadow) {
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT, width, height, layers, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTex->getTexture(), 0);
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex->getTexture(), 0);
	}
	depthTex->unbind();

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer not complete!\n";
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer() {
	glDeleteFramebuffers(1, &fbo);
}

// Produces a mesh with a plane that covers the entire screen with no framebuffer specific textures.
Mesh Framebuffer::produceEmptyFbPlane(const char* vert, const char* frag) {
	Shader<GL_VERTEX_SHADER> screenVert(vert, true);
	Shader<GL_FRAGMENT_SHADER> screenFrag(frag, true);
	auto screenShader = std::make_shared<ShaderProgram>(screenVert, screenFrag);

	std::shared_ptr<Material> planeMat = std::make_shared<Material>(screenShader);
	SubMesh planeSub(primitives::plane::planeInd, planeMat);
	return Mesh(primitives::plane::planeVert, { planeSub });
}

// Produces a mesh with a plane that covers the entire screen with the framebuffer's color and depth textures.
Mesh Framebuffer::produceFbPlane(const char* vert, const char* frag) {
	Shader<GL_VERTEX_SHADER> screenVert(vert, true);
	Shader<GL_FRAGMENT_SHADER> screenFrag(frag, true);
	auto screenShader = std::make_shared<ShaderProgram>(screenVert, screenFrag);

	std::shared_ptr<Material> planeMat = std::make_shared<Material>(screenShader);
	for (int i = 0; i < colorTexs.size(); i++) {
		planeMat->setTexture("screenTexture" + std::to_string(i), colorTexs[i]);
	}
	planeMat->setTexture("depthTexture", depthTex);
	SubMesh planeSub(primitives::plane::planeInd, planeMat);
	return Mesh(primitives::plane::planeVert, { planeSub });
}

const std::vector<std::shared_ptr<Texture>>& Framebuffer::getColorTexs() {
	return colorTexs;
}

std::shared_ptr<Texture> Framebuffer::getDepthTex() {
	return depthTex;
}

unsigned int Framebuffer::getFbo() {
	return fbo;
}

int Framebuffer::getWidth() {
	return width;
}

int Framebuffer::getHeight() {
	return height;
}

void Framebuffer::bind(bool setViewport) {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	if (setViewport) {
		glViewport(0, 0, width, height);
	}
}

void Framebuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bindDefault() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

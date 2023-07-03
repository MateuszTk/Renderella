#include "WindowManager.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include "Mesh.hpp"
#include "Vertex.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "ObjLoader.hpp"
#include "Camera.hpp"
#include "PhongMat.hpp"
#include "Light.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main() {
	WindowManager window(1280, 720, "Window");

	auto sponza = ObjLoader::load("assets/sponza/obj/sponza.obj");

	Camera camera(true, glm::vec3(0.0f, 1.0f, 0.0f));
	camera.use();

	Light light(Light::Type::POINT, glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(1.0f), glm::vec3(0.0f), 0.5f);
	Light light1(Light::Type::DIRECTIONAL, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.4f, -1.0f, 0.4f));

	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	auto texture = std::make_shared<Texture>(window.getWidth(), window.getHeight());
	texture->bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getTexture(), 0);
	texture->unbind();

	unsigned int depthTex;
	glGenTextures(1, &depthTex);
	glBindTexture(GL_TEXTURE_2D, depthTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, window.getWidth(), window.getHeight(), 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer not complete!\n";
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	const std::vector<Vertex> planeVert {
		{ { -1.0f, -1.0f, 0.0f }, {}, { 0.0f, 0.0f } },
		{ { -1.0f,  1.0f, 0.0f }, {}, { 0.0f, 1.0f } },
		{ { 1.0f,  1.0f, 0.0f }, {}, { 1.0f, 1.0f } },
		{ { 1.0f, -1.0f, 0.0f }, {}, { 1.0f, 0.0f } }
	};

	const std::vector<unsigned int> planeInd {
		0, 1, 3,
		1, 2, 3
	};

	Shader<GL_VERTEX_SHADER> screenVert("assets/shaders/screen.vert", true);
	Shader<GL_FRAGMENT_SHADER> screenFrag("assets/shaders/screen.frag", true);
	auto screenShader = std::make_shared<ShaderProgram>(screenVert, screenFrag);

	Material planeMat(screenShader);
	planeMat.addTexture("screenTexture", texture);
	SubMesh planeSub(planeInd, planeMat);
	Mesh plane(planeVert, { planeSub });

	while (window.frame(true, true)) {
		camera.update(window);

		light.setColor(glm::vec3(0.5f + sin(glfwGetTime() * 1.1f), 0.5f + sin(glfwGetTime()), 0.5f + sin(glfwGetTime() * 0.5f)));
		light.setPos(glm::vec3(sin(glfwGetTime()) * 4.0f - 2.0f, 2.0f, 0.0f));

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		for (auto& mesh : sponza) {
			mesh.draw();
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		plane.draw();
	}

	glDeleteFramebuffers(1, &fbo);

	return 0;
}

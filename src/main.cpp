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
#include "Framebuffer.hpp"
#include "Primitives.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main() {
	WindowManager window(1280, 720, "Window");

	auto sponza = ObjLoader::load("assets/sponza/obj/sponza.obj");

	Camera camera(Camera::ProjectionType::PERSPECTIVE, window.getAspectRatio(), true, glm::vec3(0.0f, 2.0f, 0.0f));

	Light light(Light::Type::POINT, glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(1.0f), glm::vec3(0.0f), 0.5f);
	Light light1(Light::Type::DIRECTIONAL, glm::vec3(0.0f, 16.0f, 0.0f), glm::vec3(1.0f), glm::vec3(0.1f, -0.9f, 0.4f));
	light1.setFov(16.0f);

	Framebuffer lightFramebuffer(1024, 1024, false);

	Framebuffer framebuffer(window.getWidth(), window.getHeight(), true);
	Mesh plane = framebuffer.produceFbPlane("assets/shaders/screen.vert", "assets/shaders/screen.frag");
	plane.getSubmeshes()[0].material.addTexture("lightDepth", lightFramebuffer.getDepthTex());
	plane.getSubmeshes()[0].material.setIncludeLightsUniforms(true);
	plane.getSubmeshes()[0].material.setIncludeCameraPosUniform(true);
	plane.getSubmeshes()[0].material.setIncludeCameraDirUniform(true);

	while (window.frame(false, true)) {
		camera.update(window);

		light.setColor(glm::vec3(0.5f + sin(glfwGetTime() * 1.1f), 0.5f + sin(glfwGetTime()), 0.5f + sin(glfwGetTime() * 0.5f)));
		light.setPosition(glm::vec3(sin(glfwGetTime()) * 4.0f - 2.0f, 2.0f, 0.0f));

		// directional light
		lightFramebuffer.bind(true);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		light1.use();
		for (auto& mesh : sponza) {
			mesh.draw();
		}
		
		// player camera
		framebuffer.bind(true);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		camera.use();
		for (auto& mesh : sponza) {
			mesh.draw();
		}

		// compose
		framebuffer.unbind();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		plane.draw();
	}

	return 0;
}

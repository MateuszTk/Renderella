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

	Light light(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 2.0f);
	Light light1(glm::vec3(0.0f, 8.0f, 0.0f), glm::vec3(1.0f, 0.5f, 1.0f));

	while (window.frame(true, true)) {
		camera.update(window);

		light1.setColor(glm::vec3(0.5f + sin(glfwGetTime() * 1.1f), 0.5f + sin(glfwGetTime()), 0.5f + sin(glfwGetTime() * 0.5f)));
		light1.setPos(glm::vec3(0.0f, 10.0f + sin(glfwGetTime() * 2.0f), 0.0f));
		light.setPos(glm::vec3(sin(glfwGetTime()) * 4.0f - 2.0f, 2.0f, 0.0f));

		for (auto& mesh : sponza) {
			mesh.draw();
		}
	}

	return 0;
}

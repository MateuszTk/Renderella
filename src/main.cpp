#include "WindowManager.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include "Mesh.hpp"
#include "Vertex.hpp"
#include "Primitives.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "ObjLoader.hpp"
#include "Camera.hpp"
#include "PhongMat.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main() {
	WindowManager window(1280, 720, "Window");

	Shader<GL_VERTEX_SHADER> vertexShader("assets/shaders/vertex.vert", true);
	Shader<GL_FRAGMENT_SHADER> plainFragmentShader("assets/shaders/plain.frag", true);
	auto plainShaderProgram = std::make_shared<ShaderProgram>(vertexShader, plainFragmentShader);

	PhongMat material;
	auto texture1 = std::make_shared<Texture>("assets/texture.png");
	material.addTexture("texture1", texture1);

	PhongMat material2;
	auto texture2 = std::make_shared<Texture>("assets/texture1.png");
	material2.addTexture("texture1", texture2);

	// cube
	std::vector<Vertex> vertices2 = primitives::cube::cubeVert;
	std::vector<unsigned int> indices = primitives::plane::planeInd;
	std::vector<unsigned int> indices2 = primitives::cube::cubeInd;
	Mesh cube(vertices2, {
		SubMesh(indices, material),
		SubMesh(indices2, material2)
	});
	cube.setPosition(glm::vec3(0.5f, 2.0f, 0.5f));
	cube.setRotation(glm::vec3(0.0f, 0.0f, 45.0f));

	// monkey
	Mesh monkey = std::move(ObjLoader::load("assets/monkey.obj")[0]);
	Material monkeyMaterial(plainShaderProgram);
	monkeyMaterial.setVec3("color", glm::vec3(1.0f, 1.0f, 0.5f));
	monkey.getSubmeshes()[0].material = monkeyMaterial;
	monkey.setPosition(glm::vec3(-1.5f, 2.5f, 0.5f));

	// sponza
	auto sponza = ObjLoader::load("assets/sponza/obj/sponza.obj");

	Camera camera(true, glm::vec3(0.0f, 1.0f, 0.0f));
	camera.use();

	while (window.frame(true, true)) {
		camera.update(window);

		cube.setRotation(glm::vec3(0, 0.2f, 0.2f) + cube.getRotation());
		monkey.setRotation(glm::vec3(0, 0.2f, 0.2f) + monkey.getRotation());

		cube.draw();
		monkey.draw();

		for (auto& mesh : sponza) {
			//TODO: do this automatically
			mesh.getSubmeshes()[0].material.setVec3("viewPos", camera.getPosition());
			mesh.draw();
		}
	}

	return 0;
}

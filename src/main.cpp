#include "WindowManager.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include "Mesh.hpp"
#include "Vertex.hpp"
#include "Primitives.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "ObjLoader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

ShaderProgram* ShaderProgram::currentProgram = nullptr;

int main() {
	WindowManager window(800, 600, "Window");

	Shader<GL_VERTEX_SHADER> vertexShader("assets/shaders/vertex.vert", true);
	Shader<GL_FRAGMENT_SHADER> fragmentShader("assets/shaders/textured.frag", true);
	Shader<GL_FRAGMENT_SHADER> plainFragmentShader("assets/shaders/plain.frag", true);
	auto texturedShaderProgram = std::shared_ptr<ShaderProgram>(new ShaderProgram(vertexShader, fragmentShader));
	auto plainShaderProgram = std::shared_ptr<ShaderProgram>(new ShaderProgram(vertexShader, plainFragmentShader));

	Material material(texturedShaderProgram);
	auto texture1 = std::shared_ptr<Texture>(new Texture("assets/texture.png"));
	material.addTexture("texture1", texture1);

	Material material2(texturedShaderProgram);
	auto texture2 = std::shared_ptr<Texture>(new Texture("assets/texture1.png"));
	material2.addTexture("texture1", texture2);

	std::vector<Vertex> vertices = primitives::plane::planeVert;
	std::vector<unsigned int> indices = primitives::plane::planeInd;
	Mesh mesh(vertices, { 
		SubMesh(indices, material)
	});

	std::vector<Vertex> vertices2 = primitives::cube::cubeVert;
	std::vector<unsigned int> indices2 = primitives::cube::cubeInd;
	Mesh cube(vertices2, {
		SubMesh(indices, material),
		SubMesh(indices2, material2)
	});

	cube.setPosition(glm::vec3(0.5f, 0.0f, 0.5f));
	cube.setRotation(glm::vec3(0.0f, 0.0f, 45.0f));
	cube.setScale(glm::vec3(0.5f, 0.5f, 0.5f));
	mesh.setPosition(glm::vec3(0.0f, -1.0f, 0.4f));
	mesh.setRotation(glm::vec3(90.0f, 0.0f, 0.0f));

	// monkey
	Mesh monkey = ObjLoader::load("assets/monkey.obj");
	Material monkeyMaterial(plainShaderProgram);
	monkeyMaterial.addVec3("color", glm::vec3(1.0f, 1.0f, 0.5f));
	monkey.getSubmeshes()[0].material = monkeyMaterial;

	monkey.setPosition(glm::vec3(-0.5f, 0.0f, 0.5f));
	monkey.setScale(glm::vec3(0.5f, 0.5f, 0.5f));

	while (window.frame(true, true)) {
		cube.setRotation(glm::vec3(0, 0.2f, 0.2f) + cube.getRotation());
		monkey.setRotation(glm::vec3(0, 0.2f, 0.2f) + monkey.getRotation());

		cube.draw();
		mesh.draw();
		monkey.draw();
	}

	return 0;
}

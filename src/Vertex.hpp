#pragma once
#include <glm/glm.hpp>

struct Vertex{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texture;
	glm::vec3 tangent;
	glm::vec3 bitangent;
};

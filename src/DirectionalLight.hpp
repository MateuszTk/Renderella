#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include <glm/glm.hpp>
#include "Camera.hpp"
#include "Light.hpp"

class DirectionalLight : public Light {

	public:

		DirectionalLight(const glm::vec3& pos, const glm::vec3& direction, const glm::vec3& color, float intensity = 1.0f);
		~DirectionalLight() = default;

		void use();

		void setDirection(const glm::vec3& dir);
		glm::vec3 getDirection() const;

		const static int cascadeCount = 4;

	private:

		glm::vec3 direction;
		std::vector<Camera> cameras;

		void updateLists() override;
};

#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include <glm/glm.hpp>
#include "Camera.hpp"
#include "Light.hpp"

class DirectionalLight : public Light {
public:
	DirectionalLight(const glm::vec3& pos, const glm::vec3& direction, const glm::vec3& color, float intensity = 1.0f) 
		: Light(Type::DIRECTIONAL, pos, color, intensity), direction(direction) {
		lightSpaceMatrixCount[(int)Type::DIRECTIONAL] = cascadeCount;

		for (int i = 0; i < cascadeCount; i++) {
			cameras.emplace_back(Camera::ProjectionType::ORTHOGRAPHIC, 1.0f, false, pos, direction, 10.0f * std::pow(2.0f, i), 200.0f, 0.1f);
			cameras[i].setIsLight(true);
		}

		updateLists();
	}

	void use() {
		cameras[0].use(false);
		for (int i = 1; i < cascadeCount; i++) {
			cameras[i].use(true);
		}
	}

	void setDirection(const glm::vec3& dir) {
		direction = dir;
		updateLists();
	}

	glm::vec3 getDirection() const {
		return direction;
	}

	const static int cascadeCount = 4;

private:
	glm::vec3 direction;
	std::vector<Camera> cameras;

	void updateLists() override {
		Light::updateLists();

		for (int i = 0; i < cascadeCount; i++) {
			cameras[i].setDirection(direction);
			cameras[i].setPosition(position);
		}

		int index = 0;
		while (index < usedLights && index < maxLights && lights[index] != this) {
			index++;
		}
		if (index < usedLights) {
			lightDirections[index] = direction;

			int indexLSM = 0;
			for (int i = 0; i < index; i++) {
				Type type = (Type)lightPositions[i].w;
				if (type == Type::DIRECTIONAL) {
					indexLSM += cascadeCount;
				}
			}
			for (int i = 0; i < cascadeCount; i++) {
				lightSpaceMatrices[indexLSM + i] = cameras[i].getCameraMatrix();
			}
		}
	}
};

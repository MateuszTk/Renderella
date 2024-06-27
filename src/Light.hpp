#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include <glm/glm.hpp>
#include "Camera.hpp"

class Light {
public:
	enum class Type {
		POINT = 0,
		DIRECTIONAL = 1
	};

	Light(Type type, const glm::vec3& pos, const glm::vec3& color, float intensity = 1.0f) 
		: type(type), color(color), intensity(intensity), position(pos) {
		if (usedLights >= maxLights) {
			std::cout << "Maximum number of lights reached\n";
		}
		else {
			lights[usedLights] = this;		
			usedLights++;
		}
	}

	~Light() {
		if (usedLights > 1) {
			// remove this light by swapping it with the last element
			int index = 0;
			while (index < usedLights && lights[index] != this) {
				index++;
			}
			if (index < usedLights) {
				lights[index] = lights[usedLights - 1];
				lightPositions[index] = lightPositions[usedLights - 1];
				lightColors[index] = lightColors[usedLights - 1];
				lightDirections[index] = lightDirections[usedLights - 1];
				for (int i = 0; i < 4; i++) {// TODO: use variable from DirectionalLight
					lightSpaceMatrices[index * 4 + i] = lightSpaceMatrices[(usedLights - 1) * 4 + i];
				}
				usedLights--;
			}
		}
		else {
			usedLights = 0;
		}
	}

	const glm::vec3 getColor() const {
		return color;
	}

	float getIntensity() const {
		return intensity;
	}

	void setColor(const glm::vec3& color) {
		this->color = color;
		updateLists();
	}

	void setIntensity(float intensity) {
		this->intensity = intensity;
		updateLists();
	}

	const glm::vec3 getPosition() const {
		return position;
	}

	void setPosition(const glm::vec3& position) {
		this->position = position;
		updateLists();
	}

	Type getType() const {
		return type;
	}

	void setType(Type type) {
		this->type = type;
		updateLists();
	}

	static Light** getLights() {
		return lights;
	}

	static glm::vec4* getLightPositions() {
		return lightPositions;
	}

	static glm::vec3* getLightColors() {
		return lightColors;
	}

	static glm::vec3* getLightDirections() {
		return lightDirections;
	}

	static glm::mat4* getLightSpaceMatrices() {
		return lightSpaceMatrices;
	}

	static int getUsedLightsCnt() {
		return usedLights;
	}

	static int getMaxLights() {
		return maxLights;
	}

protected:
	glm::vec3 position;
	glm::vec3 color;
	Type type;
	float intensity;
	const static int maxLights = 2;
	static int usedLights;
	static Light* lights[maxLights];

	// the w component of the light position is used to store the light type
	static glm::vec4 lightPositions[maxLights];

	static glm::vec3 lightColors[maxLights];

	static glm::vec3 lightDirections[maxLights];

	static glm::mat4 lightSpaceMatrices[maxLights * 4]; // todo: use variable from DirectionalLight

	virtual void updateLists() {
		int index = 0;
		while (index < usedLights && lights[index] != this) {
			index++;
		}
		if (index < usedLights) {
			lightPositions[index] = glm::vec4(position, (float)type);
			lightColors[index] = color * intensity;
		}
	}
};

#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include <glm/glm.hpp>

class Light {
public:
	enum class Type {
		POINT = 0,
		DIRECTIONAL = 1
	};

	Light(Type type, const glm::vec3& pos, const glm::vec3& color, const glm::vec3& direction = glm::vec3(0.0f), float intensity = 1.0f) : type(type), pos(pos), color(color), direction(direction), intensity(intensity) {
		if (usedLights >= maxLights) {
			std::cout << "Maximum number of lights reached\n";
		}
		else {
			lights[usedLights] = this;
			lightPositions[usedLights] = glm::vec4(pos, (int)type);
			lightColors[usedLights] = color * intensity;
			lightDirections[usedLights] = direction;
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
				usedLights--;
			}
		}
		else {
			usedLights = 0;
		}
	}

	const glm::vec3& getPos() const {
		return pos;
	}

	const glm::vec3& getColor() const {
		return color;
	}

	float getIntensity() const {
		return intensity;
	}

	void setPos(const glm::vec3& pos) {
		this->pos = pos;
		update();
	}

	void setColor(const glm::vec3& color) {
		this->color = color;
		update();
	}

	void setIntensity(float intensity) {
		this->intensity = intensity;
		update();
	}

	Type getType() const {
		return type;
	}

	void setType(Type type) {
		this->type = type;
		update();
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

	static int getUsedLightsCnt() {
		return usedLights;
	}

	static int getMaxLights() {
		return maxLights;
	}

private:
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec3 direction;
	Type type;
	float intensity;
	const static int maxLights = 8;
	static int usedLights;
	static Light* lights[maxLights];

	// the w component of the light position is used to store the light type
	static glm::vec4 lightPositions[maxLights];

	static glm::vec3 lightColors[maxLights];

	static glm::vec3 lightDirections[maxLights];

	void update() {
		int index = 0;
		while (index < usedLights && lights[index] != this) {
			index++;
		}
		if (index < usedLights) {
			lightPositions[index] = glm::vec4(pos, (float)type);
			lightColors[index] = color * intensity;
			lightDirections[index] = direction;
		}
	}
};

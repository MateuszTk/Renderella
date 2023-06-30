#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include <glm/glm.hpp>

class Light {
public:
	Light(const glm::vec3& pos, const glm::vec3& color, float intensity = 1.0f) : pos(pos), color(color), intensity(intensity) {
		if (usedLights >= maxLights) {
			std::cout << "Maximum number of lights reached\n";
		}
		else {
			lights[usedLights] = this;
			lightPositions[usedLights] = pos;
			lightColors[usedLights] = color * intensity;
			usedLights++;
		}
	}

	~Light() {
		if (usedLights > 1) {
			// remove this light from the vector by swapping it with the last element and then popping it
			int index = 0;
			while (index < usedLights && lights[index] != this) {
				index++;
			}
			if (index < usedLights) {
				lights[index] = lights[usedLights - 1];
				lightPositions[index] = lightPositions[usedLights - 1];
				lightColors[index] = lightColors[usedLights - 1];
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

	static Light** getLights() {
		return lights;
	}

	static glm::vec3* getLightPositions() {
		return lightPositions;
	}

	static glm::vec3* getLightColors() {
		return lightColors;
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
	float intensity;
	const static int maxLights = 8;
	static int usedLights;
	static Light* lights[maxLights];
	static glm::vec3 lightPositions[maxLights];
	static glm::vec3 lightColors[maxLights];

	void update() {
		int index = 0;
		while (index < usedLights && lights[index] != this) {
			index++;
		}
		if (index < usedLights) {
			lightPositions[index] = pos;
			lightColors[index] = color * intensity;
		}
	}
};

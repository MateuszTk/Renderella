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
			while (index < usedLights && index < maxLights && lights[index] != this) {
				index++;
			}
			if (index < usedLights) {
				glm::mat4 newLightSpaceMatrices[maxLightSpaceMatrices];
				int indexLSM = 0;
				for (int i = 0; i < usedLights - 1; i++) {
					if (i != index) {
						Type type = (Type)lightPositions[i].w;
						if (type == Type::DIRECTIONAL) {
							for (int j = 0; j < lightSpaceMatrixCount[(int)type]; j++) {
								if (indexLSM + j >= maxLightSpaceMatrices) {
									std::cout << "Error: Light space matrices array overflow\n";
									break;
								}
								newLightSpaceMatrices[indexLSM + j] = lightSpaceMatrices[indexLSM + j];
							}
							indexLSM += lightSpaceMatrixCount[(int)type];
						}
					}
					else {
						Type lastType = (Type)lightPositions[usedLights - 1].w;
						if (lastType == Type::DIRECTIONAL) {
							int lastLightLSMIndex = 0;
							for (int j = 0; j < usedLights - 1; j++) {
								Type type = (Type)lightPositions[j].w;
								if (type == Type::DIRECTIONAL) {
									lastLightLSMIndex += lightSpaceMatrixCount[(int)type];
								}
							}
							
							for (int j = 0; j < lightSpaceMatrixCount[(int)type]; j++) {
								if (indexLSM + j >= maxLightSpaceMatrices) {
									std::cout << "Error: Light space matrices array overflow\n";
									break;
								}
								newLightSpaceMatrices[indexLSM + j] = lightSpaceMatrices[lastLightLSMIndex + j];
							}
							indexLSM += lightSpaceMatrixCount[(int)type];
						}
					}
				}
				for (int i = 0; i < maxLightSpaceMatrices; i++) {
					lightSpaceMatrices[i] = newLightSpaceMatrices[i];
				}

				lights[index] = lights[usedLights - 1];
				lightPositions[index] = lightPositions[usedLights - 1];
				lightColors[index] = lightColors[usedLights - 1];
				lightDirections[index] = lightDirections[usedLights - 1];		
			}
			usedLights--;
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

	static int getMaxLightSpaceMatrices() {
		return maxLightSpaceMatrices;
	}

protected:
	glm::vec3 position;
	glm::vec3 color;
	Type type;
	float intensity;
	const static int maxLights = 8;
	const static int maxLightSpaceMatrices = 8;
	static int usedLights;
	static Light* lights[maxLights];

	// the w component of the light position is used to store the light type
	static glm::vec4 lightPositions[maxLights];

	static glm::vec3 lightColors[maxLights];

	static glm::vec3 lightDirections[maxLights];

	static glm::mat4 lightSpaceMatrices[maxLightSpaceMatrices];
	// maps the type of the light to the number of light space matrices it uses
	static int lightSpaceMatrixCount[2];

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

#include "Light.hpp"

Light::Light(Type type, const glm::vec3& pos, const glm::vec3& color, float intensity)
	: type(type), color(color), intensity(intensity), position(pos) {
	if (usedLights >= maxLights) {
		std::cout << "Maximum number of lights reached\n";
	}
	else {
		lights[usedLights] = this;
		usedLights++;
	}
}

Light::~Light() {
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

const glm::vec3 Light::getColor() const {
	return color;
}

float Light::getIntensity() const {
	return intensity;
}

void Light::setColor(const glm::vec3& color) {
	this->color = color;
	updateLists();
}

void Light::setIntensity(float intensity) {
	this->intensity = intensity;
	updateLists();
}

const glm::vec3 Light::getPosition() const {
	return position;
}

void Light::setPosition(const glm::vec3& position) {
	this->position = position;
	updateLists();
}

Light::Type Light::getType() const {
	return type;
}

void Light::setType(Type type) {
	this->type = type;
	updateLists();
}

Light** Light::getLights() {
	return lights;
}

glm::vec4* Light::getLightPositions() {
	return lightPositions;
}

glm::vec3* Light::getLightColors() {
	return lightColors;
}

glm::vec3* Light::getLightDirections() {
	return lightDirections;
}

glm::mat4* Light::getLightSpaceMatrices() {
	return lightSpaceMatrices;
}

int Light::getUsedLightsCnt() {
	return usedLights;
}

int Light::getMaxLights() {
	return maxLights;
}

int Light::getMaxLightSpaceMatrices() {
	return maxLightSpaceMatrices;
}

void Light::updateLists() {
	int index = 0;
	while (index < usedLights && lights[index] != this) {
		index++;
	}
	if (index < usedLights) {
		lightPositions[index] = glm::vec4(position, (float)type);
		lightColors[index] = color * intensity;
	}
}

int Light::usedLights = 0;
Light* Light::lights[];
glm::vec4 Light::lightPositions[];
glm::vec3 Light::lightColors[];
glm::vec3 Light::lightDirections[];
glm::mat4 Light::lightSpaceMatrices[];
int Light::lightSpaceMatrixCount[] = { 0 };

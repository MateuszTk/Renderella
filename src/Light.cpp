#include "Light.hpp"

int Light::usedLights = 0;
Light* Light::lights[Light::maxLights];
glm::vec4 Light::lightPositions[Light::maxLights];
glm::vec3 Light::lightColors[Light::maxLights];
glm::vec3 Light::lightDirections[Light::maxLights];
glm::mat4 Light::lightSpaceMatrices[Light::maxLights];

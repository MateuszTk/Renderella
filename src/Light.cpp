#include "Light.hpp"

int Light::usedLights = 0;
Light* Light::lights[Light::maxLights];
glm::vec3 Light::lightPositions[Light::maxLights];
glm::vec3 Light::lightColors[Light::maxLights];

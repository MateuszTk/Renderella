#include "Light.hpp"

int Light::usedLights = 0;
Light* Light::lights[];
glm::vec4 Light::lightPositions[];
glm::vec3 Light::lightColors[];
glm::vec3 Light::lightDirections[];
glm::mat4 Light::lightSpaceMatrices[];
int Light::lightSpaceMatrixCount[] = { 0 };

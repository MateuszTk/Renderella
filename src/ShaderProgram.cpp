#include "ShaderProgram.hpp"

ShaderProgram* ShaderProgram::currentProgram = nullptr;

void ShaderProgram::setInt(const UniLocation& location, int value) {
	int locationId = location.getLocation();
	if (locationId >= 0)
		glUniform1i(location.getLocation(), value);
}

void ShaderProgram::setFloat(const UniLocation& location, float value) {
	int locationId = location.getLocation();
	if (locationId >= 0)
		glUniform1f(location.getLocation(), value);
}

void ShaderProgram::setVec2(const UniLocation& location, const glm::vec2& value) {
	int locationId = location.getLocation();
	if (locationId >= 0)
		glUniform2f(location.getLocation(), value.x, value.y);
}

void ShaderProgram::setVec3(const UniLocation& location, const glm::vec3& value) {
	int locationId = location.getLocation();
	if (locationId >= 0)
		glUniform3f(location.getLocation(), value.x, value.y, value.z);
}

void ShaderProgram::setVec3s(const UniLocation& location, std::vector<glm::vec3> values) {
	int locationId = location.getLocation();
	if (locationId >= 0)
		glUniform3fv(location.getLocation(), values.size(), glm::value_ptr(values[0]));
}

void ShaderProgram::setVec3s(const UniLocation& location, glm::vec3* values, int size) {
	int locationId = location.getLocation();
	if (locationId >= 0)
		glUniform3fv(location.getLocation(), size, glm::value_ptr(values[0]));
}

void ShaderProgram::setVec4(const UniLocation& location, const glm::vec4& value) {
	int locationId = location.getLocation();
	if (locationId >= 0)
		glUniform4f(location.getLocation(), value.x, value.y, value.z, value.w);
}

void ShaderProgram::setVec4s(const UniLocation& location, glm::vec4* values, int size) {
	int locationId = location.getLocation();
	if (locationId >= 0)
		glUniform4fv(location.getLocation(), size, glm::value_ptr(values[0]));
}

void ShaderProgram::setMat3(const UniLocation& location, const glm::mat3& value) {
	int locationId = location.getLocation();
	if (locationId >= 0)
		glUniformMatrix3fv(location.getLocation(), 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::setMat4(const UniLocation& location, const glm::mat4& value) {
	int locationId = location.getLocation();
	if (locationId >= 0)
		glUniformMatrix4fv(location.getLocation(), 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::setMat4s(const UniLocation& location, glm::mat4* values, int size) {
	int locationId = location.getLocation();
	if (locationId >= 0)
		glUniformMatrix4fv(location.getLocation(), size, GL_FALSE, glm::value_ptr(values[0]));
}

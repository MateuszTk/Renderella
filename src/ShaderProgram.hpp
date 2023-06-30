#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.hpp"
#include <iostream>
#include <string>
#include <vector>

class ShaderProgram {
public:
	ShaderProgram(const Shader<GL_VERTEX_SHADER>& vertexShader, const Shader<GL_FRAGMENT_SHADER>& fragmentShader) {
		this->master = true;
		this->id = glCreateProgram();
		glAttachShader(this->id, vertexShader.getId());
		glAttachShader(this->id, fragmentShader.getId());
		glLinkProgram(this->id);

		int success;
		char infoLog[512];
		glGetProgramiv(this->id, GL_LINK_STATUS, &success);

		if (!success) {
			glGetProgramInfoLog(this->id, 512, NULL, infoLog);
			std::cout << "Error linking shader program\n" << infoLog << '\n';
		}
	}

	ShaderProgram(ShaderProgram&& other) {
		this->id = other.id;
		this->master = true;
		other.master = false;
	}

	unsigned int getId() const {
		return this->id;
	}

	void use() {
		glUseProgram(this->id);
		currentProgram = this;
	}

	static ShaderProgram* getCurrentProgram() {
		if (currentProgram == nullptr) {
			std::cout << "No shader program is currently in use\n";
		}
		return currentProgram;
	}

	void setInt(const std::string& name, int value) {
		glUniform1i(glGetUniformLocation(this->id, name.c_str()), value);
	}

	void setFloat(const std::string& name, float value) {
		glUniform1f(glGetUniformLocation(this->id, name.c_str()), value);
	}

	void setVec2(const std::string& name, const glm::vec2& value) {
		glUniform2f(glGetUniformLocation(this->id, name.c_str()), value.x, value.y);
	}

	void setVec3(const std::string& name, const glm::vec3& value) {
		glUniform3f(glGetUniformLocation(this->id, name.c_str()), value.x, value.y, value.z);
	}

	void setVec3s(const std::string& name, std::vector<glm::vec3> values) {
		glUniform3fv(glGetUniformLocation(this->id, name.c_str()), values.size(), glm::value_ptr(values[0]));
	}

	void setVec3s(const std::string& name, glm::vec3* values, int size) {
		glUniform3fv(glGetUniformLocation(this->id, name.c_str()), size, glm::value_ptr(values[0]));
	}

	void setVec4(const std::string& name, const glm::vec4& value) {
		glUniform4f(glGetUniformLocation(this->id, name.c_str()), value.x, value.y, value.z, value.w);
	}

	void setMat3(const std::string& name, const glm::mat3& value) {
		glUniformMatrix3fv(glGetUniformLocation(this->id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}

	void setMat4(const std::string& name, const glm::mat4& value) {
		glUniformMatrix4fv(glGetUniformLocation(this->id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}

	~ShaderProgram() {
		if (master) {
			glDeleteProgram(this->id);
		}
	}

private:
	unsigned int id;
	static ShaderProgram* currentProgram;
	// If this is true, then this object is responsible for deleting the shader program
	bool master;
};


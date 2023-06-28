#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>

template <unsigned int type>
class Shader {
public:
	Shader(std::string source, bool isFilePath) {
		this->ok = true;
		if (isFilePath) {
			std::ifstream file(source);
			if (!file.is_open()) {
				std::cout << "Error opening shader file\n";
				this->ok = false;
				return;
			}
			source = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			file.close();
		}

		id = glCreateShader(type);
		const char* src = source.c_str();
		glShaderSource(id, 1, &src, NULL);
		glCompileShader(id);

		int success;
		char infoLog[512];
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);

		if (!success) {
			glGetShaderInfoLog(id, 512, NULL, infoLog);
			std::cout << "Error compiling shader\n" << infoLog << '\n';
			this->ok = false;
		}
	}

	unsigned int getId() const {
		return id;
	}

	bool isOk() const {
		return ok;
	}

	~Shader() {
		if (ok) {
			glDeleteShader(id);
		}
	}

private:
	unsigned int id;
	bool ok;
};

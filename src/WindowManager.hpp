#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

class WindowManager {
public:
	WindowManager(int width, int height, std::string name) : width(width), height(height), lastFrameTime(0.0) {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		this->window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);

		if (window == NULL) {
			std::cout << "Failed to create GLFW window\n";
			glfwTerminate();
			return;
		}
		glfwMakeContextCurrent(this->window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cout << "Failed to initialize GLAD\n";
			return;
		}

		glViewport(0, 0, width, height);
		glEnable(GL_DEPTH_TEST);
	}

	GLFWwindow* getWindow() {
		return this->window;
	}

	bool frame(bool clear, bool showFps = false) {
		glfwSwapBuffers(this->window);
		glfwPollEvents();

		if (clear){
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		if (showFps) {
			this->showFPS();
		}

		return !glfwWindowShouldClose(this->window);
	}

	~WindowManager() {
		glfwTerminate();
	}

private:
	int width;
	int height;
	GLFWwindow* window;
	double lastFrameTime;

	void showFPS() {
		double currentFrameTime = glfwGetTime();
		double deltaTime = currentFrameTime - this->lastFrameTime;
		if (deltaTime >= 1.0) {
			this->lastFrameTime = currentFrameTime;
			glfwSetWindowTitle(this->window, (std::to_string(1000.0 / deltaTime) + " FPS").c_str());
		}
	}
};

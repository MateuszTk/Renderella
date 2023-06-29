#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

class WindowManager {
public:
	WindowManager(int width, int height, std::string name) 
		: width(width), height(height), lastFrameTime(0.0), fpsCount(0), lastFpsTime(0.0), deltaTime(0.00001) {
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

	bool frame(bool clear, bool showFps = false) {
		glfwSwapBuffers(this->window);
		glfwPollEvents();

		if (clear){
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		double time = glfwGetTime();
		this->deltaTime = time - this->lastFrameTime;
		this->lastFrameTime = time;
		if (showFps) {
			this->showFPS();
		}

		return !glfwWindowShouldClose(this->window);
	}

	GLFWwindow* getWindow() const {
		return this->window;
	}

	double getDeltaTime() const {
		return this->deltaTime;
	}

	int getWidth() const {
		return this->width;
	}

	int getHeight() const {
		return this->height;
	}

	float getAspectRatio() const {
		return (float)this->width / (float)this->height;
	}

	~WindowManager() {
		glfwTerminate();
	}

private:
	int width;
	int height;
	GLFWwindow* window;
	double lastFrameTime;
	double deltaTime;

	double lastFpsTime;
	unsigned int fpsCount;

	void showFPS() {
		this->lastFpsTime += this->deltaTime;
		this->fpsCount++;
		if (lastFpsTime >= 1.0) {
			glfwSetWindowTitle(this->window, (std::to_string(this->fpsCount / this->lastFpsTime) + " FPS").c_str());
			this->lastFpsTime = 0.0;
			this->fpsCount = 0;
		}
	}
};

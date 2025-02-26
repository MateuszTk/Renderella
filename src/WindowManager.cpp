#include "WindowManager.hpp"

WindowManager::WindowManager(int width, int height, std::string name)
	: width(width), height(height), lastFrameTime(0.0), fpsCount(0), lastFpsTime(0.0), deltaTime(0.00001) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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

WindowManager::~WindowManager() {
	glfwTerminate();
}

bool WindowManager::frame(bool clear, bool showFps) {
	glfwSwapBuffers(this->window);
	glfwPollEvents();

	if (clear) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	double time = glfwGetTime();
	this->deltaTime = time - this->lastFrameTime;
	this->lastFrameTime = time;
	if (showFps) {
		this->showFPS();
	}

	frameCounter = (frameCounter + 1) % 0x7FFFFFFF;

	return !glfwWindowShouldClose(this->window);
}

GLFWwindow* WindowManager::getWindow() const {
	return this->window;
}

double WindowManager::getDeltaTime() const {
	return this->deltaTime;
}

int WindowManager::getWidth() const {
	return this->width;
}

int WindowManager::getHeight() const {
	return this->height;
}

float WindowManager::getAspectRatio() const {
	return (float)this->width / (float)this->height;
}

int WindowManager::getFrameCounter() {
	return frameCounter;
}

void WindowManager::showFPS() {
	this->lastFpsTime += this->deltaTime;
	this->fpsCount++;
	if (lastFpsTime >= 1.0) {
		glfwSetWindowTitle(this->window, (std::to_string(this->fpsCount / this->lastFpsTime) + " FPS").c_str());
		this->lastFpsTime = 0.0;
		this->fpsCount = 0;
	}
}

int WindowManager::frameCounter = 0;

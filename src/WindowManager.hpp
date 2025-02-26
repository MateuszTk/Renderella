#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

class WindowManager {

	public:

		WindowManager(int width, int height, std::string name);
		~WindowManager();

		bool frame(bool clear, bool showFps = false);

		GLFWwindow* getWindow() const;

		double getDeltaTime() const;

		int getWidth() const;
		int getHeight() const;
		float getAspectRatio() const;

		static int getFrameCounter();

	private:

		int width;
		int height;
		GLFWwindow* window;
		double lastFrameTime;
		double deltaTime;

		static int frameCounter;

		double lastFpsTime;
		unsigned int fpsCount;

		void showFPS();

};

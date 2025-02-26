#pragma once
#include <iostream>
#include "stb_image.h"
#include <string>
#include <fstream>

#ifndef CACHE_LOCATION
	#define CACHE_LOCATION "cache/"
#endif

class TextureData {

	public:

		TextureData(const std::string& path);

		unsigned char* getData() const;

		int getWidth() const;
		int getHeight() const;
		int getChannels() const;

		std::string getName() const;
		std::string getPath() const;

		void addAlpha(const TextureData& mask);

		// if the color is all 0, make it single channel
		void optimizeAlphaOnly();

		void invert();

		~TextureData();

	private:

		unsigned char* data;
		int width;
		int height;
		int channels;
		std::string name;
		std::string path;

		void createTextureCache();

		bool loadTextureCache();

};

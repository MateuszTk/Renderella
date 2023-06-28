#pragma once
#include <vector>
#include "Vertex.hpp"

namespace primitives {
	namespace plane {
		const std::vector<Vertex> planeVert {
			{ -0.5f, -0.5f, 0.0f, 0.0f, 0.0f },
			{ 0.5f, -0.5f, 0.0f, 1.0f, 0.0f },
			{ 0.5f, 0.5f, 0.0f, 1.0f, 1.0f },
			{ -0.5f, 0.5f, 0.0f, 0.0f, 1.0f }
		};

		const std::vector<unsigned int> planeInd {
			0, 1, 3,
			1, 2, 3
		};
	}

	namespace cube {
		const std::vector<Vertex> cubeVert {
			//front
			{ -0.5f, -0.5f, 0.5f, 0.0f, 0.0f },
			{ 0.5f, -0.5f, 0.5f, 1.0f, 0.0f },
			{ 0.5f, 0.5f, 0.5f, 1.0f, 1.0f },
			{ -0.5f, 0.5f, 0.5f, 0.0f, 1.0f },
			//back
			{ -0.5f, -0.5f, -0.5f, 0.0f, 0.0f },
			{ 0.5f, -0.5f, -0.5f, 1.0f, 0.0f },
			{ 0.5f, 0.5f, -0.5f, 1.0f, 1.0f },
			{ -0.5f, 0.5f, -0.5f, 0.0f, 1.0f }
		};

		const std::vector<unsigned int> cubeInd {
			0, 1, 3,
			1, 2, 3,
			1, 5, 2,
			5, 6, 2,
			5, 4, 6,
			4, 7, 6,
			4, 0, 7,
			0, 3, 7,
			3, 2, 6,
			3, 6, 7,
			4, 5, 1,
			4, 1, 0
		};
	}
}

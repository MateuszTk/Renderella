#pragma once
#include "Vertex.hpp"
#include <vector>
#include "Vertex.hpp"

namespace primitives {
	namespace plane {
		const std::vector<Vertex> planeVert {
			{ { -1.0f, -1.0f, 0.0f }, {}, { 0.0f, 0.0f } },
			{ { -1.0f,  1.0f, 0.0f }, {}, { 0.0f, 1.0f } },
			{ { 1.0f,  1.0f, 0.0f }, {}, { 1.0f, 1.0f } },
			{ { 1.0f, -1.0f, 0.0f }, {}, { 1.0f, 0.0f } }
		};

		const std::vector<unsigned int> planeInd {
			0, 1, 3,
			1, 2, 3
		};
	}
}

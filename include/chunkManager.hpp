/*
Author: Lydia Jameson
Class: ECE6122
Last Date Modified: 10/31/2024

Description:
header for chunkManager
*/

#include <map>
#include <vector>

#include <glm/glm.hpp>
#include "Chunk.hpp"

void chunkManager(glm::vec3 userPosition, int16_t viewDist, int64_t seed, float chunkSize,
	std::map<glm::vec2, Chunk<std::vector<glm::vec3>>>& chunkMap);
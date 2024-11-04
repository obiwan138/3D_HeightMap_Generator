/*
Author: Lydia Jameson
Class: ECE6122
Last Date Modified: 10/31/2024

Description:
This is the chunk management algorithm. It takes in user position, and current chunk map then adds and removes chunks to the map as appropriate.
*/

#include <map>
#include <vector>

#include <glm/glm.hpp>
#include "Chunk.hpp"

void chunkManager(glm::vec3 userPosition, int16_t viewDist, int64_t seed, float chunkSize, 
	std::map<glm::vec2, Chunk<std::vector<glm::vec3>>> &chunkMap) {

	//find center of loaded chunks
	glm::vec2 chunkMapCenter;
	for (auto i = chunkMap.begin(); i != chunkMap.end(); i++) {

	}

	//need new chunks in the +x direction
	if (userPosition.x > chunkMapCenter.x + chunkSize / 2) {
		for (int i = 0; i < 1 + viewDist * 2; i++) {

		}
	}

	//need new chunks in the -x direction
	if (userPosition.x < chunkMapCenter.x - chunkSize / 2) {

	}

	//need new chunks in the +y direction
	if (userPosition.y > chunkMapCenter.y + chunkSize / 2) {

	}

	//need new chunks in the -y direction
	if (userPosition.y < chunkMapCenter.y + chunkSize / 2) {

	}
}
/*
Author: Lydia Jameson
Class: ECE6122
Last Date Modified: 11/03/2024

Description:

*/

#include <iostream>
#include <cmath>
#include "Chunk.hpp"

Chunk::Chunk(int64_t seed, double chunkSize, double resolution, glm::vec2 chunkCoords) {
	m_chunkSize = chunkSize;
	m_resolution = resolution;
	m_chunkCoords = chunkCoords;
	m_pointsPerSide = static_cast<unsigned int>(m_chunkSize / m_resolution);

	heightMap = std::vector<glm::vec3>(m_pointsPerSide * m_pointsPerSide, glm::vec3(0, 0, 0));

	std::cout << "Made a chunk at " << chunkCoords.x * chunkSize << ", " << chunkCoords.y * chunkSize << std::endl;
}

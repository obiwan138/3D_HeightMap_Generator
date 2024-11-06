/*
Author: Lydia Jameson
Class: ECE6122
Last Date Modified: 11/03/2024

Description:

*/

#include <iostream>
#include "Chunk.hpp"

Chunk::Chunk(int64_t seed, double chunkSize, glm::vec2 chunkCoords) {
	m_chunkSize = chunkSize;
	m_resolution = 16;
	m_spacing = m_chunkSize / (m_resolution - 1);
	m_chunkCoords = chunkCoords;

	heightMap = std::vector<glm::vec3>(m_resolution * m_resolution, glm::vec3(0, 0, 0));

	std::cout << "Made a chunk at " << chunkCoords.x * chunkSize << ", " << chunkCoords.y * chunkSize << std::endl;
}

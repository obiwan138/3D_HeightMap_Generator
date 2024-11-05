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

	glm::vec3 offset = glm::vec3(m_chunkCoords.x - m_chunkSize / 2, 0, m_chunkCoords.y - m_chunkSize / 2);

	heightMap = std::vector<glm::vec3>(m_resolution * m_resolution, glm::vec3(0, 0, 0));

	for (int row = 0; row < m_resolution; row++) {
		for (int col = 0; col < m_resolution; col++) {
			heightMap[row * m_resolution + col].x = offset.x + m_spacing * col;
			heightMap[row * m_resolution + col].z = offset.z + m_spacing * row;


		}
	}
	std::cout << "Made a chunk at " << chunkCoords.x * chunkSize << ", " << chunkCoords.y * chunkSize << std::endl;
}

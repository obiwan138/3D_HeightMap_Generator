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
#include "chunkManager.hpp"

#include <iostream>

ChunkManager::ChunkManager(uint16_t viewDist, int64_t seed, float chunkSize) {

	m_viewDist = viewDist;
	m_seed = seed;
	m_chunkSize = chunkSize;

	m_pos = glm::vec3(0, 0, 0);
	m_prevPos = m_pos;
	m_center = m_pos;

	for (int i = -m_viewDist; i <= m_viewDist; i++) {
		for (int j = -m_viewDist; j <= m_viewDist; j++) {
			chunkMap.emplace(std::pair<int, int>(i, j), Chunk(m_seed, m_chunkSize, glm::vec2(i, j)));
		}
	}
}

void ChunkManager::update(glm::vec3 pos){
	m_prevPos = m_pos;
	m_pos = pos;

	glm::vec3 direction = m_pos - m_prevPos;

	//need new chunks in the +x direction
	if (m_pos.x > m_center.x + m_chunkSize / 2) {
		//for (int i = 0; i < 1 + m_viewDist * 2; i++) {

		//}

		m_center.x += m_chunkSize;
	}

	//need new chunks in the -x direction
	if (m_pos.x < m_center.x - m_chunkSize / 2) {


		m_center.x -= m_chunkSize;
	}

	//need new chunks in the +z direction
	if (m_pos.z > m_center.z + m_chunkSize / 2) {


		m_center.z += m_chunkSize;
	}

	//need new chunks in the -z direction
	if (m_pos.z < m_center.z + m_chunkSize / 2) {


		m_center.z -= m_chunkSize;
	}
}
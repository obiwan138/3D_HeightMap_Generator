/*
Author: Lydia Jameson
Class: ECE6122
Last Date Modified: 11/16/2024

Description:
This is the chunk management algorithm. It takes in user position, and current chunk map then adds and removes chunks to the map as appropriate.
*/

#include <map>
#include <vector>
#include <omp.h>
#include <thread>
#include <atomic>
#include <stdexcept>

#include <glm/glm.hpp>
#include "Chunk.hpp"
#include "ChunkManager.hpp"

#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @author Lydia Jameson
 * @brief Constructor
 */
ChunkManager::ChunkManager(uint16_t viewDist, int64_t seed, float chunkSize, float resolution, ColorMap* cmapPointer) : gradientNoise(seed) {

	m_viewDist = viewDist;
	m_seed = seed;
	m_chunkSize = chunkSize;
	m_resolution = resolution;
	m_cmapPointer = cmapPointer;

	m_pos = glm::vec3(0, 0, 0);
	m_prevPos = m_pos;
	m_center = m_pos;

	for (int i = -m_viewDist; i <= m_viewDist; i++) {
		for (int j = -m_viewDist; j <= m_viewDist; j++) {
			std::pair<int, int> currentPair(i, j);

			if (currentPair == std::pair<int, int>(0, 0)) {
				populateChunk(currentPair);
			} else {
				threadVector.emplace_back(&ChunkManager::populateChunk, this, currentPair);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @author Lydia Jameson
 * @brief create and destroy chunks based on camera position
 */
void ChunkManager::update(glm::vec3 pos){
	m_prevPos = m_pos;
	m_pos = pos;

	glm::vec3 direction = m_pos - m_prevPos;

	//need new chunks in the +x direction
	if (m_pos.x > m_center.x + m_chunkSize / 2) {
		for (int i = m_center.z / m_chunkSize - m_viewDist; i <= m_center.z / m_chunkSize + m_viewDist; i++) {

			std::pair<int, int> currentPair(m_center.x / m_chunkSize + m_viewDist + 1, i);
			threadVector.emplace_back(&ChunkManager::populateChunk, this, currentPair);
			//populateChunk(currentPair);

			std::unique_lock<std::mutex> lck(m_mut);
			chunkMap.erase(std::pair<int, int>(currentPair.first - (2 * m_viewDist + 1), currentPair.second));
			lck.unlock();
		}
		m_center.x += m_chunkSize;
	}

	//need new chunks in the -x direction
	if (m_pos.x < m_center.x - m_chunkSize / 2) {
		for (int i = m_center.z / m_chunkSize - m_viewDist; i <= m_center.z / m_chunkSize + m_viewDist; i++) {

			std::pair<int, int> currentPair(m_center.x / m_chunkSize - m_viewDist - 1, i);
			threadVector.emplace_back(&ChunkManager::populateChunk, this, currentPair);
			//populateChunk(currentPair);

			std::unique_lock<std::mutex> lck(m_mut);
			chunkMap.erase(std::pair<int, int>(currentPair.first + (2 * m_viewDist + 1), currentPair.second));
			lck.unlock();
		}
		m_center.x -= m_chunkSize;
	}

	//need new chunks in the +z direction
	if (m_pos.z > m_center.z + m_chunkSize / 2) {
		for (int i = m_center.x / m_chunkSize - m_viewDist; i <= m_center.x / m_chunkSize + m_viewDist; i++) {

			std::pair<int, int> currentPair(i, m_center.z / m_chunkSize + m_viewDist + 1);
			threadVector.emplace_back(&ChunkManager::populateChunk, this, currentPair);
			//populateChunk(currentPair);

			std::unique_lock<std::mutex> lck(m_mut);
			chunkMap.erase(std::pair<int, int>(currentPair.first, currentPair.second - (2 * m_viewDist + 1)));
			lck.unlock();
		}
		m_center.z += m_chunkSize;
	}

	//need new chunks in the -z direction
	if (m_pos.z < m_center.z - m_chunkSize / 2) {
		for (int i = m_center.x / m_chunkSize - m_viewDist; i <= m_center.x / m_chunkSize + m_viewDist; i++) {

			std::pair<int, int> currentPair(i, m_center.z / m_chunkSize - m_viewDist - 1);
			threadVector.emplace_back(&ChunkManager::populateChunk, this, currentPair);
			//populateChunk(currentPair);

			std::unique_lock<std::mutex> lck(m_mut);
			chunkMap.erase(std::pair<int, int>(currentPair.first, currentPair.second + (2 * m_viewDist + 1)));
			lck.unlock();
		}
		m_center.z -= m_chunkSize;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @author Lydia Jameson
 * @brief create a chunk and populate its height map. Put chunk in chunkmap
 */
void ChunkManager::populateChunk(std::pair<int, int> currentPair) {
	glm::vec3 offset = glm::vec3((m_chunkSize - m_resolution) * (currentPair.first - 0.5f), 0, (m_chunkSize - m_resolution) * (currentPair.second - 0.5f));
	//glm::vec3 offset = glm::vec3((m_chunkSize) * (currentPair.first - 0.5f), 0, (m_chunkSize) * (currentPair.second - 0.5f));

	Chunk tempChunk(m_seed, m_chunkSize, m_resolution, glm::vec2(currentPair.first, currentPair.second));

	for (int row = 0; row < tempChunk.pointsPerSide(); row++) {
		for (int col = 0; col < tempChunk.pointsPerSide(); col++) {
			tempChunk.heightMap[row * tempChunk.pointsPerSide() + col].z = offset.z + tempChunk.resolution() * col;
			tempChunk.heightMap[row * tempChunk.pointsPerSide() + col].x = offset.x + tempChunk.resolution() * row;
			gradientNoise.fractalPerlin2D(tempChunk.heightMap[row * tempChunk.pointsPerSide() + col], 5, 0);
		}
	}

	std::unique_lock<std::mutex> lck(m_mut);
	chunkMap.emplace(currentPair, tempChunk);
	lck.unlock();
}

/////////////////////////////////////////////////////////////////////
/**
 * @author Thomas Etheve
 * @brief Render chunks
 * @param shaderProgram : pointer to the shader program
 */
void ChunkManager::renderChunks(GLuint* shaderProgramPointer)
{
	// Iterate through the chunk map
	for (auto chunkIt = this->chunkMap.begin(); chunkIt != this->chunkMap.end(); chunkIt++)
	{
		// Render the current chunk
		if (!chunkIt->second.preparedToRender()) {
			std::unique_lock<std::mutex> lck(m_mut);
			chunkIt->second.prepareToRender(m_cmapPointer);
			lck.unlock();
		}
		chunkIt->second.renderChunk(shaderProgramPointer);
	}
	
}

///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @author Lydia Jameson
 * @brief Destructor
 */
ChunkManager::~ChunkManager()
{
	for (auto& t : threadVector) {
		t.join();
	}
}

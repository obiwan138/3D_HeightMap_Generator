/*
Author: Lydia Jameson
Class: ECE6122
Last Date Modified: 10/31/2024

Description:
This is the chunk management algorithm. It takes in user position, and current chunk map then adds and removes chunks to the map as appropriate.
*/

#include <map>
#include <vector>
#include <omp.h>
#include <thread>
#include <atomic>

#include <glm/glm.hpp>
#include "Chunk.hpp"
#include "chunkManager.hpp"

#include <iostream>

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

			chunkMap.emplace(std::piecewise_construct,
				std::forward_as_tuple(currentPair),
				std::forward_as_tuple(m_seed, m_chunkSize, m_resolution, glm::vec2(i, j)));

			populateChunk(currentPair);
		}
	}
}

void ChunkManager::update(glm::vec3 pos){
	m_prevPos = m_pos;
	m_pos = pos;

	glm::vec3 direction = m_pos - m_prevPos;

	//need new chunks in the +x direction
	if (m_pos.x > m_center.x + m_chunkSize / 2) {
		for (int i = m_center.z / m_chunkSize - m_viewDist; i <= m_center.z / m_chunkSize + m_viewDist; i++) {
			std::pair<int, int> currentPair(m_center.x / m_chunkSize + m_viewDist + 1, i);
			chunkMap.emplace(std::piecewise_construct,
				std::forward_as_tuple(currentPair),
				std::forward_as_tuple(m_seed, m_chunkSize, m_resolution, glm::vec2(currentPair.first, currentPair.second)));

			threadVector.emplace_back(&ChunkManager::populateChunk, this, currentPair);

			chunkMap.erase(std::pair<int, int>(currentPair.first - (2 * m_viewDist + 1), currentPair.second));
		}
		m_center.x += m_chunkSize;
	}

	//need new chunks in the -x direction
	if (m_pos.x < m_center.x - m_chunkSize / 2) {
		for (int i = m_center.z / m_chunkSize - m_viewDist; i <= m_center.z / m_chunkSize + m_viewDist; i++) {
			std::pair<int, int> currentPair(m_center.x / m_chunkSize - m_viewDist - 1, i);
			chunkMap.emplace(std::piecewise_construct,
				std::forward_as_tuple(currentPair),
				std::forward_as_tuple(m_seed, m_chunkSize, m_resolution, glm::vec2(currentPair.first, currentPair.second)));

			threadVector.emplace_back(&ChunkManager::populateChunk, this, currentPair);

			chunkMap.erase(std::pair<int, int>(currentPair.first + (2 * m_viewDist + 1), currentPair.second));
		}
		m_center.x -= m_chunkSize;
	}

	//need new chunks in the +z direction
	if (m_pos.z > m_center.z + m_chunkSize / 2) {
		for (int i = m_center.x / m_chunkSize - m_viewDist; i <= m_center.x / m_chunkSize + m_viewDist; i++) {
			std::pair<int, int> currentPair(i, m_center.z / m_chunkSize + m_viewDist + 1);
			chunkMap.emplace(std::piecewise_construct,
				std::forward_as_tuple(currentPair),
				std::forward_as_tuple(m_seed, m_chunkSize, m_resolution, glm::vec2(currentPair.first, currentPair.second)));

			threadVector.emplace_back(&ChunkManager::populateChunk, this, currentPair);

			chunkMap.erase(std::pair<int, int>(currentPair.first, currentPair.second - (2 * m_viewDist + 1)));
		}
		m_center.z += m_chunkSize;
	}

	//need new chunks in the -z direction
	if (m_pos.z < m_center.z - m_chunkSize / 2) {
		for (int i = m_center.x / m_chunkSize - m_viewDist; i <= m_center.x / m_chunkSize + m_viewDist; i++) {
			std::pair<int, int> currentPair(i, m_center.z / m_chunkSize - m_viewDist - 1);
			chunkMap.emplace(std::piecewise_construct,
				std::forward_as_tuple(currentPair),
				std::forward_as_tuple(m_seed, m_chunkSize, m_resolution, glm::vec2(currentPair.first, currentPair.second)));

			threadVector.emplace_back(&ChunkManager::populateChunk, this, currentPair);

			chunkMap.erase(std::pair<int, int>(currentPair.first, currentPair.second + (2 * m_viewDist + 1)));
		}
		m_center.z -= m_chunkSize;
	}
}

void ChunkManager::populateChunk(std::pair<int, int> currentPair) {
	glm::vec3 offset = glm::vec3(m_chunkSize * (currentPair.first - 0.5f), 0, m_chunkSize * (currentPair.second - 0.5f));

	for (int row = 0; row < chunkMap[currentPair].pointsPerSide(); row++) {
		for (int col = 0; col < chunkMap[currentPair].pointsPerSide(); col++) {
			chunkMap[currentPair].heightMap[row * chunkMap[currentPair].pointsPerSide() + col].z = offset.z + chunkMap[currentPair].resolution() * col;
			chunkMap[currentPair].heightMap[row * chunkMap[currentPair].pointsPerSide() + col].x = offset.x + chunkMap[currentPair].resolution() * row;
			gradientNoise.fractalPerlin2D(chunkMap[currentPair].heightMap[row * chunkMap[currentPair].pointsPerSide() + col], 5, 0);
		}
	}

	chunkMap[currentPair].displayable.store(true);
}


/////////////////////////////////////////////////////////////////////
/**
 * @author Thomas Etheve
 * @brief Initialize the buffers for the chunk
 * @param cmapPointer : Pointer to the color map
 */

void ChunkManager::prepareToRender(ColorMap* cmapPointer)
{
	m_cmapPointer = cmapPointer;
	// Loop through the chunk map
	for (auto elementIt = this->chunkMap.begin(); elementIt != this->chunkMap.end(); elementIt++)
	{
		// Initialize the buffers for the current chunk
		elementIt->second.prepareToRender(m_cmapPointer);
	}
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
		if (chunkIt->second.displayable.load()) {
			if (!chunkIt->second.preparedToRender()) chunkIt->second.prepareToRender(m_cmapPointer);
			chunkIt->second.renderChunk(shaderProgramPointer);
		}
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

/////////////////////////////////////////////////////////////////////
/**
 * @author Thomas Etheve
 * @brief Get the edges of the chunk map
 * @return Edge2D : The edges of the chunk map
 */
/*
Edge2D ChunkManager::getEdges()
{
	// Create the variable to return
	Edge2D edge;
	edge.left = 0.f; edge.right = 0.f;
	edge.top = 0.f; edge.bottom = 0.f;

	// Loop through the chunk map
	for(auto& ChunkIt : this->chunkMap)
	{
		// Get the chunk coordinates
		std::pair<int,int> chunkCellCoords = ChunkIt.first;
		// Get the chunk size
		double chunkSize = ChunkIt.second.size();

		// Compare the edges
		if(chunkCellCoords.first*chunkSize - chunkSize / 2.0 < edge.left)	// Left
		{
			edge.left = chunkCellCoords.first*chunkSize - chunkSize / 2.0;
		}
		if(chunkCellCoords.first*chunkSize + chunkSize / 2.0 > edge.right)  // Right
		{
			edge.right = chunkCellCoords.first*chunkSize + chunkSize / 2.0;
		}
		if(chunkCellCoords.second*chunkSize + chunkSize / 2.0 > edge.top)   // Top
		{
			edge.top = chunkCellCoords.second*chunkSize + chunkSize / 2.0;
		}
		if(chunkCellCoords.second*chunkSize - chunkSize / 2.0 < edge.bottom) // Bottom
		{
			edge.bottom = chunkCellCoords.second*chunkSize - chunkSize / 2.0;
		}
	}

	// Return extremal edges
	return edge;
}
*/
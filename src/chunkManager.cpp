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

ChunkManager::ChunkManager(uint16_t viewDist, int64_t seed, float chunkSize, float resolution) : gradientNoise(seed) {

	m_viewDist = viewDist;
	m_seed = seed;
	m_chunkSize = chunkSize;
	m_resolution = resolution;

	m_pos = glm::vec3(0, 0, 0);
	m_prevPos = m_pos;
	m_center = m_pos;

	for (int i = -m_viewDist; i <= m_viewDist; i++) {
		for (int j = -m_viewDist; j <= m_viewDist; j++) {
			std::pair<int, int> currentPair(i, j);

			chunkMap.emplace(currentPair, Chunk(m_seed, m_chunkSize, m_resolution, glm::vec2(i, j)));

			glm::vec3 offset = glm::vec3(i - m_chunkSize / 2.0f, 0, j - m_chunkSize / 2.0f);

			for (int row = 0; row < chunkMap[currentPair].pointsPerSide(); row++) {
				for (int col = 0; col < chunkMap[currentPair].pointsPerSide(); col++) {
					chunkMap[currentPair].heightMap[row * chunkMap[currentPair].pointsPerSide() + col].z = offset.z + chunkMap[currentPair].resolution() * col;
					chunkMap[currentPair].heightMap[row * chunkMap[currentPair].pointsPerSide() + col].x = offset.x + chunkMap[currentPair].resolution() * row;
					//chunkMap[currentPair].heightMap[row * chunkMap[currentPair].pointsPerSide() + col].y = 1;
					gradientNoise.fractalPerlin2D(chunkMap[currentPair].heightMap[row * chunkMap[currentPair].pointsPerSide() + col], 5, 0);

					//std::cout << row * chunkMap[currentPair].pointsPerSide() + col << ": " << chunkMap[currentPair].heightMap[row * chunkMap[currentPair].pointsPerSide() + col].x << ", " << chunkMap[currentPair].heightMap[row * chunkMap[currentPair].pointsPerSide() + col].y
					//	<< ", " << chunkMap[currentPair].heightMap[row * chunkMap[currentPair].pointsPerSide() + col].z << std::endl;
				}
				//std::cout << "Row " << row << " Done" << std::endl;
			}
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


/////////////////////////////////////////////////////////////////////
/**
 * @author Thomas Etheve
 * @brief Initialize the buffers for the chunk
 * @param cmapPointer : Pointer to the color map
 */

void ChunkManager::prepareToRender(ColorMap* cmapPointer)
{
	// Loop through the chunk map
	for (auto elementIt = this->chunkMap.begin(); elementIt != this->chunkMap.end(); elementIt++)
	{
		// Initialize the buffers for the current chunk
		elementIt->second.prepareToRender(cmapPointer);
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
		chunkIt->second.renderChunk(shaderProgramPointer);
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
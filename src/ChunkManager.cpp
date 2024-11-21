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

	// Prepare 2d map
	this->make2DMap();
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
 * @brief Generate the 2D map view
 * @param windowSize : The size of the window
 */
void ChunkManager::make2DMap()
{
	// Create as many sprites as there are chunks
	this->chunkSprites.clear();

	// Loop through the chunk map
	for(auto elementIt = this->chunkMap.begin(); elementIt != this->chunkMap.end(); ++elementIt)
	{
		// Create the sprite
		sf::Sprite sprite;
		sprite.setTexture(*(elementIt->second.getTexture()));

		// Add the pair (coords, sprite) to the map
		this->chunkSprites.emplace(elementIt->first, sprite);
	}
}

/////////////////////////////////////////////////////////////////////
/**
 * @brief Draw the 2D map view
 * @param window : The window to draw the 2D map view
 */
void ChunkManager::drawChunks(sf::RenderWindow* window)
{
	// Loop through the sprites
	for(auto elementIt = this->chunkSprites.begin(); elementIt != this->chunkSprites.end(); ++elementIt)
	{
		// Get some data from the current element
		std::pair<int,int> chunkCoords = elementIt->first;		// Chunk Coord in the Chunk map cell grid
		float chunkSize = this->chunkMap[chunkCoords].size();	// Chunk size (distance)
		
		// Set the sprite position
		elementIt->second.setPosition(window->getSize().x/2 + (-0.5f + chunkCoords.first)*chunkSize,  window->getSize().y/2  + (-0.5f + chunkCoords.second)*chunkSize);
		// Draw the sprite
		
		window->draw(elementIt->second);
	}
}
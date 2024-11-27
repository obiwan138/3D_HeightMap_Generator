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
#include <queue>

#include <glm/glm.hpp>
#include "Chunk.hpp"
#include "ChunkManager.hpp"

// Include boost
#include <boost/program_options.hpp>

#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @author Lydia Jameson
 * @brief Constructor
 */
ChunkManager::ChunkManager(uint16_t viewDist, ColorMap* cmapPointer, po::variables_map args) : gradientNoise(args["seed"].as<uint32_t>()) {
	m_viewDist = viewDist;
	m_seed = args["seed"].as<uint32_t>();
	m_chunkSize = args["size"].as<size_t>()*args["resolution"].as<double>();
	m_resolution = static_cast<float>(args["resolution"].as<double>());
	m_cmapPointer = cmapPointer;

	m_pos = glm::vec3(0, 0, 0);
	m_prevPos = m_pos;
	m_center = m_pos;
	m_args = args;

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
	glm::vec3 distanceFromCenter = m_pos - m_center;

	//need new chunks in the +x direction
	if (m_pos.x > m_center.x + m_chunkSize / 2) {
		for (int i = m_center.z / m_chunkSize - m_viewDist; i <= m_center.z / m_chunkSize + m_viewDist; i++) {

			std::pair<int, int> currentPair(m_center.x / m_chunkSize + m_viewDist + 1, i);
			threadVector.emplace_back(&ChunkManager::populateChunk, this, currentPair);
		}
		m_center.x += m_chunkSize;
	}

	//need new chunks in the -x direction
	if (m_pos.x < m_center.x - m_chunkSize / 2) {
		for (int i = m_center.z / m_chunkSize - m_viewDist; i <= m_center.z / m_chunkSize + m_viewDist; i++) {

			std::pair<int, int> currentPair(m_center.x / m_chunkSize - m_viewDist - 1, i);
			threadVector.emplace_back(&ChunkManager::populateChunk, this, currentPair);
		}
		m_center.x -= m_chunkSize;
	}

	//need new chunks in the +z direction
	if (m_pos.z > m_center.z + m_chunkSize / 2) {
		for (int i = m_center.x / m_chunkSize - m_viewDist; i <= m_center.x / m_chunkSize + m_viewDist; i++) {

			std::pair<int, int> currentPair(i, m_center.z / m_chunkSize + m_viewDist + 1);
			threadVector.emplace_back(&ChunkManager::populateChunk, this, currentPair);
		}
		m_center.z += m_chunkSize;
	}

	//need new chunks in the -z direction
	if (m_pos.z < m_center.z - m_chunkSize / 2) {
		for (int i = m_center.x / m_chunkSize - m_viewDist; i <= m_center.x / m_chunkSize + m_viewDist; i++) {

			std::pair<int, int> currentPair(i, m_center.z / m_chunkSize - m_viewDist - 1);
			threadVector.emplace_back(&ChunkManager::populateChunk, this, currentPair);
		}
		m_center.z -= m_chunkSize;
	}

	std::unique_lock<std::mutex> lck(m_mut);
	//check for chunks that are more than viewDist away
	for (auto it = chunkMap.begin(); it != chunkMap.end(); it++) {
		if (abs(it->first.first * m_chunkSize - m_center.x) > m_viewDist * m_chunkSize || abs(it->first.second * m_chunkSize - m_center.z) > m_viewDist * m_chunkSize) {
			deletionQueue.push(it->first);
		}
	}

	while (!deletionQueue.empty()) {
		if (chunkMap.erase(deletionQueue.front()) && chunkSprites.erase(deletionQueue.front())) {
			deletionQueue.pop();
		}
	}

	lck.unlock();
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
			gradientNoise.fractalPerlin2D(tempChunk.heightMap[row * tempChunk.pointsPerSide() + col], m_args["max"].as<double>(), m_args["mode"].as<int>(), 
										  m_args["octaves"].as<int>(), m_args["freq-start"].as<double>(), m_args["freq-rate"].as<double>(), m_args["amp-rate"].as<double>());
		}
	}

	std::unique_lock<std::mutex> lck(m_mut);

	// Add the 3D chunk to the chunk map
	chunkMap.emplace(currentPair, tempChunk);
	std::cout << "Chunk added at " << currentPair.first << ", " << currentPair.second << std::endl;

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
		// If the chunk is not prepared to render yet (buffers and 2d texture not generated), prepare it
		if (!chunkIt->second.preparedToRender()) {
			std::unique_lock<std::mutex> lck(m_mut);

			// Prepare the chunk
			chunkIt->second.prepareToRender(m_cmapPointer);

			// Add the chunk sprite to the sprite map
			sf::Sprite sprite;										// Create the sprite
			sprite.setTexture(*(chunkIt->second.getTexture()));		// Set the texture
			this->chunkSprites.emplace(chunkIt->first, sprite);		// Add the pair (coords, sprite) to the map

			lck.unlock();
		}

		// Render the chunk
		chunkIt->second.renderChunk(shaderProgramPointer);
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
		std::pair<int,int> chunkCoords = elementIt->first;					// Chunk Coord in the Chunk map cell grid
		sf::FloatRect spriteBounds = elementIt->second.getGlobalBounds();	// Get the sprite bounds
		
		// Set the sprite position
		elementIt->second.setPosition(window->getSize().x/2 + (-0.5f + chunkCoords.first)*spriteBounds.width,  window->getSize().y/2  + (-0.5f + chunkCoords.second)*spriteBounds.height);
		
		// Draw the sprite
		window->draw(elementIt->second);
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

/*
Author: Lydia Jameson
Class: ECE6122
Last Date Modified: 11/28/2024

Description:
Chunk Manager class implementation file. This contains the declaration for the ChunkManager class. The Chunk Manager handles how chunks are created,
deleted, and rendered. It also handles the 2D map view.
*/

// Standard libraries
#include <map>
#include <vector>
#include <omp.h>
#include <thread>
#include <atomic>
#include <stdexcept>
#include <queue>
#include <iostream>

// OpenGL Mathematics
#include <glm/glm.hpp>

// Boost lib for command line arguments
#include <boost/program_options.hpp>

// Project headers
#include "Chunk.hpp"
#include "ChunkManager.hpp"

///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @author Lydia Jameson
 * @brief Constructor
 * @param cmapPointer : pointer to the color map object
 * @param args : command line arguments
 */
ChunkManager::ChunkManager(ColorMap* cmapPointer, po::variables_map args) : gradientNoise(args["seed"].as<uint32_t>()) {
	
	// Initialize member variables using the command line arguments
	m_viewDist = args["visibility"].as<unsigned int>();
	m_seed = args["seed"].as<uint32_t>();
	m_chunkSize = args["size"].as<size_t>()*args["resolution"].as<double>();
	m_resolution = static_cast<float>(args["resolution"].as<double>());
	m_cmapPointer = cmapPointer;
	m_pos = glm::vec3(0, 0, 0);
	m_prevPos = m_pos;
	m_center = m_pos;
	m_args = args;

	// Populate the initial chunk map in the view distance (in chunks)
	for (int i = -m_viewDist; i <= m_viewDist; i++) {

		for (int j = -m_viewDist; j <= m_viewDist; j++) {

			// Get the coordinates of the current chunk
			std::pair<int, int> currentPair(i, j);

			// If the current chunk is at the center, populate it (initial chunk)
			if (currentPair == std::pair<int, int>(0, 0)) {
				populateChunk(currentPair);
			} else {
				// If the current chunk is not at the center, add it to the thread vector
				threadVector.emplace_back(&ChunkManager::populateChunk, this, currentPair);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @author Lydia Jameson
 * @brief create and destroy chunks based on camera position
 * @param pos : user's position (camera)
 */
void ChunkManager::update(glm::vec3 pos){

	// Update the previous and current position
	m_prevPos = m_pos;
	m_pos = pos;

	// Compute the movement direction and distance from the center
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

	//delete chunks that are more than viewDist away
	while (!deletionQueue.empty()) {

		// Try to erase the chunk from the 3D chunk map and the sprite map
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
 * @param currentPair : pair of integers representing the chunk's coordinates
 */
void ChunkManager::populateChunk(std::pair<int, int> currentPair) {

	// Calculate the offset for the current chunk
	glm::vec3 offset = glm::vec3((m_chunkSize - m_resolution) * (currentPair.first - 0.5f), 0, (m_chunkSize - m_resolution) * (currentPair.second - 0.5f));

	// Create the temporary chunk
	Chunk tempChunk(m_seed, m_chunkSize, m_resolution, glm::vec2(currentPair.first, currentPair.second));

	// Populate the height map of the chunk
	for (int row = 0; row < tempChunk.pointsPerSide(); row++) {
		for (int col = 0; col < tempChunk.pointsPerSide(); col++) {

			// Set the x and z coordinates of the height map point
			tempChunk.heightMap[row * tempChunk.pointsPerSide() + col].z = offset.z + tempChunk.resolution() * col;
			tempChunk.heightMap[row * tempChunk.pointsPerSide() + col].x = offset.x + tempChunk.resolution() * row;

			// Set the y coordinate of the height map point using the noise generator
			gradientNoise.fractalPerlin2D(tempChunk.heightMap[row * tempChunk.pointsPerSide() + col], m_args["max"].as<double>(), m_args["mode"].as<int>(), 
										  m_args["octaves"].as<int>(), m_args["freq-start"].as<double>(), m_args["freq-rate"].as<double>(), m_args["amp-rate"].as<double>());
		}
	}

	// Lock the mutex
	std::unique_lock<std::mutex> lck(m_mut);

	// Add the 3D chunk to the chunk map
	chunkMap.emplace(currentPair, tempChunk);

	// Print the coordinates of the added chunk
	std::cout << "Chunk added at " << currentPair.first << ", " << currentPair.second << std::endl;
	lck.unlock();
}

///////////////////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////////////////
/**
 * @author Thomas Etheve
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

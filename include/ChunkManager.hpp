/*
Author: Lydia Jameson
Class: ECE6122
Last Date Modified: 11/28/2024

Description:
Chunk Manager class header file. This contains the declaration for the ChunkManager class. The Chunk Manager handles how chunks are created,
deleted, and rendered. It also handles the 2D map view.
*/

#pragma once

// Standard libraries
#include <map>                    
#include <vector>                 
#include <thread>
#include <mutex>
#include <queue>

// OpenGL
#include <GL/glew.h>              // OpenGL Library
#include <glm/glm.hpp>            // OpenGL Mathematics

// SFML
#include <SFML/Graphics.hpp>      // Simple and Fast Multimedia Library

// Include boost
#include <boost/program_options.hpp>

// Project headers
#include "Perlin.hpp"
#include "Chunk.hpp"
#include "ColorMap.hpp"           // Init the color buffer

namespace po = boost::program_options;

/**
 * @class ChunkManager
 */
class ChunkManager {

    private:

        ///////////////////////////// MEMBER VARIABLES /////////////////////////////       
        glm::vec3 m_pos;            // Player's position
        glm::vec3 m_prevPos;        // Player's previous position
        glm::vec3 m_center;         // Center of the chunk map
        float m_chunkSize;          // Size of a chunk (in meters)
        float m_resolution;         // Resolution of the chunks (in meters)
        int16_t m_viewDist;         // View distance (in chunks) from the user's position
        int64_t m_seed;             // Seed for the Perlin noise

        // External objects
        GradientNoise gradientNoise;    // Perlin noise generator
        ColorMap* m_cmapPointer;        // Pointer to the color map object
        po::variables_map m_args;       // Command line arguments used by the noise generator

        // Multithreading
        std::vector<std::thread> threadVector;          // Vector of threads that will be used to populate chunks
        std::queue<std::pair<int, int>> deletionQueue;  // Queue of chunks that need to be deleted
        std::mutex m_mut;                               // Mutex for the deletion queue

        // 2D map view : map of chunk sprites with 2D grid locations (x, z) counted in chunks
        std::map<std::pair<int, int>, sf::Sprite> chunkSprites;

        
    public:

        // Map of chunks : each loaded chunk is stored and mapped to a 2D grid location (x, z) counted in chunks
        std::map<std::pair<int, int>, Chunk> chunkMap;

        ///////////////////////////// MEMBER FUNCTIONS /////////////////////////////

        // Constructor
        ChunkManager(ColorMap* cmapPointer, po::variables_map args);

        // Update the chunk map based on the player's position (creation and deletion of chunks)
        void update(glm::vec3 pos);

        // Fill in a chunk's height values
        void populateChunk(std::pair<int, int> currentPair);

        // Render chunks in 3D
        void renderChunks(GLuint* shaderProgramPointer);

        // Draw the 2D map view
        void drawChunks(sf::RenderWindow* window);

        // Destructor
        ~ChunkManager();
};


/*
Author: Lydia Jameson
Class: ECE6122
Last Date Modified: 10/31/2024

Description:
header for chunkManager
*/

#pragma once

// Standard libraries
#include <map>                    // Used for chunk map
#include <vector>                 // Used to store vertices

// OpenGL
#include <GL/glew.h>              // OpenGL Library
#include <glm/glm.hpp>            // OpenGL Mathematics

// SFML
#include <SFML/Graphics.hpp>      // Simple and Fast Multimedia Library

// Project headers
#include "Perlin.hpp"
#include "Chunk.hpp"
#include "ColorMap.hpp"           // Init the color buffer

struct Edge2D {
    float left;
    float right;
    float top;
    float bottom;
};

class ChunkManager {
private:
    glm::vec3 m_pos;
    glm::vec3 m_prevPos;
    glm::vec3 m_center;
    float m_chunkSize;
    float m_resolution;
    int16_t m_viewDist;
    int64_t m_seed;

    // 2D map view
    std::map<std::pair<int, int>, sf::Sprite> chunkSprites;


    GradientNoise gradientNoise;
public:
    ChunkManager(uint16_t viewDist, int64_t seed, float chunkSize, float resolution);

    std::map<std::pair<int, int>, Chunk> chunkMap;

    void update(glm::vec3 pos);

    // Init the buffers
    void prepareToRender(ColorMap* cmapPointer);

    // Render chunks
    void renderChunks(GLuint* shaderProgramPointer);

    // Construct the initial 2D map view
    void make2DMap();

    // Draw the 2D map view
    void drawChunks(sf::RenderWindow* window);
};


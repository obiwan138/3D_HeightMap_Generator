/*
Author: Lydia Jameson
Class: ECE6122
Last Date Modified: 10/31/2024

Description:
header for chunkManager
*/

#include <map>
#include <vector>

#include <glm/glm.hpp>
#include "Chunk.hpp"
#include "Perlin.hpp"

class ChunkManager {
private:
    glm::vec3 m_pos;
    glm::vec3 m_prevPos;
    glm::vec3 m_center;
    float m_chunkSize;
    int16_t m_viewDist;
    int64_t m_seed;

    GradientNoise gradientNoise;
public:
    ChunkManager(uint16_t viewDist, int64_t seed, float chunkSize);

    std::map<std::pair<int, int>, Chunk> chunkMap;

    void update(glm::vec3 pos);
};
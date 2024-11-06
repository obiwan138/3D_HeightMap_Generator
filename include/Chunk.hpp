/*
Author: Matthew Luyten
Class: ECE4122
Last Date Modified: 10/26/2024

Description:
This is the header for the Chunk class. This class manages an NxN chunk of heightmap.
*/

#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Chunk {
private:
    double m_chunkSize;
    int m_resolution; //num points per side
    double m_spacing; //distance between points
    glm::vec2 m_chunkCoords;
public:
    Chunk() {}
    Chunk(int64_t seed, double chunkSize, glm::vec2 chunkCoords);
    //Chunk(size_t sz, glm::vec3 pos) : sz(sz), pos(pos) {}
    //~Chunk() {}

    //make private?
    std::vector<glm::vec3> heightMap;

    double size() { return m_chunkSize; }
    int resolution() { return m_resolution; }
    double spacing() { return m_spacing; }

    //void setHeightMap(T hm) { this->hm = hm; }
    //void setSize(size_t sz) { this->sz = sz; }
    //void setPosition(glm::vec3 pos) { this->pos = pos; }


};
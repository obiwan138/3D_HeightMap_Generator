/*
Author: Matthew Luyten
Class: ECE4122
Last Date Modified: 10/26/2024

Description:
This is the header for the Chunk class. This class manages an NxN chunk of heightmap.
*/

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

template <typename T>
class Chunk {
public:
    Chunk() {}
    Chunk(size_t sz, glm::vec3 pos) : size(sz), position(pos) {}
    ~Chunk() {}

    T* heightMap() { return &heightMap }
    size_t size() { return &size }
    glm::vec3 position() { return position; }

    void setHeightMap(T hm) { heightMap = hm; }
    void setSize(size_t sz) { size = sz; }
    void setPosition(glm::vec3 pos) { position = pos; }

private:
    T heightMap;
    glm::vec3 position;
    size_t size;
};
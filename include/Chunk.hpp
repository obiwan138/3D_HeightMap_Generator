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
private:
    T hm;
    glm::vec3 pos;
    size_t sz;
public:
    Chunk() {}
    Chunk(size_t sz, glm::vec3 pos) : sz(sz), pos(pos) {}
    ~Chunk() {}

    T* heightMap() { return &hm; }
    size_t size() { return sz; }
    glm::vec3 position() { return pos; }

    void setHeightMap(T hm) { this->hm = hm; }
    void setSize(size_t sz) { this->sz = sz; }
    void setPosition(glm::vec3 pos) { this->pos = pos; }


};
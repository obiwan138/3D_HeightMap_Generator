/*
Author: Thomas Etheve
Class: ECE6122
Last Date Modified: 10/26/2024

Description:
This is the main loop for this program. It loads in all of the objects, runs the draw loop, and updates the view based on user inputs.
*/

#include "ColorMap.hpp"

/**
 * @brief Default constructor
 */
ColorMap::ColorMap()
{
    this->type = ColorMapType::MONOCHROME_LEVELS;
    this->MonochromeColor = glm::vec3(1, 1, 1);
    this->minAlt = -1.f;
    this->maxAlt = 1.f;
}

/**
 * @brief Constructor
 * @param type : colormap type
 * @param minAlt : minimum altitude
 * @param maxAlt : maximum altitude
 */
ColorMap::ColorMap(ColorMapType type, float minAlt, float maxAlt)
{
    this->type = type;
    this->MonochromeColor = glm::vec3(1, 1, 1);
    this->minAlt = minAlt;
    this->maxAlt = maxAlt;
}

/**
 * @brief Get the color vector corresponding to the vertices vector
 * @param vertices : vector of vertices
 */

std::vector<glm::vec3> ColorMap::getColorVector(const std::vector<glm::vec3>& vertices)
{
    // Create the color vector
    std::vector<glm::vec3> colors;

    switch (this->type)
    {
        case ColorMapType::GIST_EARTH:
            getEarthLevels(vertices, colors);
            break;

        case ColorMapType::MONOCHROME_LEVELS:
            getMonochromeLevels(vertices, colors);
            break;
    }

    // Return the color vector
    return colors;
}

/**
 * @brief Get the color vector corresponding to monorchrome levels
 * @param vertices : vector of vertices
 * @param colors : vector of colors
 */

void ColorMap::getMonochromeLevels(const std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors)
{
    // Associate a color to each vertex
    for (const auto& vertex : vertices)
    {
        // Compute the color
        float value = (vertex.y - this->minAlt) / (this->maxAlt - this->minAlt);
        colors.push_back(glm::vec3(value, value, value));
    }
}

/**
 * @brief Get the color vector corresponding to Earth Gist levels
 * @param vertices : vector of vertices
 * @param colors : vector of colors
 */
void ColorMap::getEarthLevels(const std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& colors)
{
    // Set the color steps 
    glm::vec3 darkBlue = glm::vec3(0.f, 0.f, 1.f);
    glm::vec3 lightBlue = glm::vec3(0.06f, 0.89f, 1.f);
    glm::vec3 ligthGreen = glm::vec3(0.3f, 1.f, 0.3f);
    glm::vec3 brown = glm::vec3(0.5f,0.27f,0.11f);
    glm::vec3 gray = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 white = glm::vec3(1.f, 1.f, 1.f);
    glm::vec3 red = glm::vec3(1.f, 0.f, 0.f); // Debug color

    // Associate a color to each vertex
    for (const auto& vertex : vertices)
    {
        if (vertex.y < this->minAlt)
        {
            colors.push_back(darkBlue);
        }
        else if (vertex.y < SEA_LEVEL*this->minAlt)
        {
            //colors.push_back(interpolateColors(this->minAlt, SEA_LEVEL, darkBlue, lightBlue, vertex.y));
            colors.push_back(darkBlue);
        }
        else if (vertex.y < MAX_LAND*this->maxAlt)
        {
            colors.push_back(interpolateColors(SEA_LEVEL*this->minAlt, MAX_LAND*this->maxAlt, ligthGreen, brown, vertex.y));
        }
        else if (vertex.y < MAX_MOUNTAIN*this->maxAlt)
        {
            colors.push_back(interpolateColors(MAX_LAND*this->maxAlt, MAX_MOUNTAIN*this->maxAlt, brown, gray, vertex.y));
        }
        else if (vertex.y < MAX_ALT*this->maxAlt)
        {
            colors.push_back(interpolateColors(MAX_MOUNTAIN*this->maxAlt, MAX_ALT*this->maxAlt, gray, white, vertex.y));
        }
        else if (vertex.y > this->maxAlt)
        {
            colors.push_back(white);
        }
        else // Debug color
        {
            colors.push_back(white);
        }
    }
}

/**
 * @brief Interpolate between two colors
 * @param y1 : first y value
 * @param y2 : second y value
 * @param c1 : first color
 * @param c2 : second color
 * @param y : y value
 */
glm::vec3 ColorMap::interpolateColors(const float& y1, const float& y2, const glm::vec3& c1, const glm::vec3& c2, const float& y)
{
    return (c1 + ((y - y1)/(y2 - y1)) * (c2 - c1));
}

/**
 * @brief Destructor
 */
ColorMap::~ColorMap(){} 
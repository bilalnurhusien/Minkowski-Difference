#pragma once

#include "Types.hpp"
#include <sstream>
#include <string>
#include <fstream>

/**
  * Print help messages
  */
void PrintHelp();

/**
  * Process arguments
  */
bool ProcessArguments(int argc,
                      char* argvp[],
                      std::vector<sf::CircleShape>& polygonShapes,
                      std::vector<sf::ConvexShape>& manualPolygonShapes);
                      
/**
  * Get opposite angles (180 deg rotation of an angle)
  */
void GetOppositeAngles(const std::vector<float>& angles,
                       std::vector<float>& oppositeAngles);

/**
  * Get normal vectors of all edges in a polygon
  */
void GetNormalVectors(const sf::ConvexShape& shape,
                      const float offsetX,
                      const float offsetY,
                      const sf::Color color,
                      std::vector<std::vector<sf::Vertex> >& normalVectors,
                      bool outwardNormal = true);
void GetNormalVectors(const sf::CircleShape& shape,
                      const float offsetX,
                      const float offsetY,
                      const sf::Color color,
                      std::vector<std::vector<sf::Vertex> >& normalVectors,
                      bool outwardNormal = true);

/*
 * Get angle of normal vectors
 */
bool GetAngleOfNormalVectors(const std::vector<std::vector<sf::Vertex> >& normalVectors,
                             std::vector<float>& angleOfNormalVectors);

/**
 * Combine list of normal vectors
 */
void MergeAngleOfNormalVectors(const std::vector<sf::ConvexShape>& vecShapes,
                               const std::vector<float>& angleOfNormalVectors1,
                               const std::vector<float>& angleOfNormalVectors2,
                               std::vector<PolygonVertex>& polygonVertices);
void MergeAngleOfNormalVectors(const std::vector<sf::CircleShape>& vecShapes,
                               const std::vector<float>& angleOfNormalVectors1,
                               const std::vector<float>& angleOfNormalVectors2,
                               std::vector<PolygonVertex>& polygonVertices);

/**
 * Return a string with a limit number of decimal places of a float or double
 *
 */
template <typename T>
std::string ToStringSetPrecision(const T a_value, const int n = 4)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
}
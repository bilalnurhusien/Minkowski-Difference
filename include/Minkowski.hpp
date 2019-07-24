#pragma once

#include "Types.hpp"

/**
 * Calculate the Minkowksi difference using a polygon robot and obstacle
 *
 */
bool MinkowskiDifference(const sf::CircleShape& robot,
                         const sf::CircleShape& polygonObstacles,
                         std::vector<std::vector<sf::Vertex> >& normalVectorsRobot,
                         std::vector<std::vector<sf::Vertex> >& normalVectorsObstacle,
                         std::vector<float>& angleOfNormalVectorsRobot,
                         std::vector<float>& angleOfNormalVectorsObstacle,
                         std::vector<PolygonVertex>& minkowskiVertices);

bool MinkowskiDifference(const sf::ConvexShape& robot,
                         const sf::ConvexShape& polygonObstacles,
                         std::vector<std::vector<sf::Vertex> >& normalVectorsRobot,
                         std::vector<std::vector<sf::Vertex> >& normalVectorsObstacle,
                         std::vector<float>& angleOfNormalVectorsRobot,
                         std::vector<float>& angleOfNormalVectorsObstacle,
                         std::vector<PolygonVertex>& minkowskiVertices);
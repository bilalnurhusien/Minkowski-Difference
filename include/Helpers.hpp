#pragma once

#include "Types.hpp"

#include <sstream>
#include <string>
#include <fstream>

#include <SFML/Graphics.hpp>

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
  * Get normal vectors of all edges in a polygon
  */
template <class T>
void GetNormalVectors(const T& shape,
                      const float offsetX,
                      const float offsetY,
                      const sf::Color color,
                      std::vector<std::vector<sf::Vertex> >& normalVectors,
                      bool outwardNormal = true)
{
    std::vector<Point> vectors;
    std::vector<Point> midPoints;

    size_t vertexCount = shape.getPointCount();

    for (uint32_t i = 0; i < vertexCount; ++i)
    {
        Point vector;
        Point midPoint;

        sf::Vector2f vertex = shape.getPoint(i);
        sf::Vector2f nextVertex;

        nextVertex = shape.getPoint((i + 1) % vertexCount);
        vector.x = nextVertex.x - vertex.x;
        vector.y = nextVertex.y - vertex.y;
        midPoint.x = (vertex.x + nextVertex.x) / 2  + offsetX;
        midPoint.y = (vertex.y + nextVertex.y) / 2  + offsetY;

        vectors.push_back(vector);
        midPoints.push_back(midPoint);
    }

    /*
     * Outward normal vector is calculated by using the 2D rotation matrix (270 deg rotation):
     *
     * Rotation Matrix = [cos(θ) -sin(θ)] = [0  1]
     *                   [sin(θ)  cos(θ)]   [-1 0]
     *
     * If we define dx=x2-x1 and dy=y2-y1, then the normals are <-dy, dx> and <dy, -dx>.
     *
     * For example, rotating a vector <dx, dy> by 270 deg:
     *
     * [cos(θ) -sin(θ)][dx] = [0  1][dx] = [dy ]
     * [sin(θ)  cos(θ)][dy]   [-1 0][dy]   [-dx]
     */
    for (uint32_t i = 0; i < vectors.size(); ++i)
    {
        sf::Vertex v1;
        sf::Vertex v2;

        v1 = sf::Vertex(sf::Vector2f(midPoints[i].x, midPoints[i].y), color);

        if (outwardNormal)
        {
            v2 = sf::Vertex(sf::Vector2f(midPoints[i].x + vectors[i].y, midPoints[i].y - vectors[i].x), color);
        }
        else
        {
            v2 = sf::Vertex(sf::Vector2f(midPoints[i].x - vectors[i].y, midPoints[i].y + vectors[i].x), color);
        }

        normalVectors.push_back(std::vector<sf::Vertex>());
        normalVectors[i].push_back(v1);
        normalVectors[i].push_back(v2);
    }
}

/**
 * Combine list of normal vectors
 */
template <class T>
void MergeAngleOfNormalVectors(const T& robot,
                               const T& obstacle,
                               const std::vector<float>& angleOfNormalVectorsRobot,
                               const std::vector<float>& angleOfNormalVectorsObstacle,
                               std::vector<PolygonVertex>& sortedPolygonVertices)
{
    size_t vertexCount1 = robot.getPointCount();
    size_t vertexCount2 = obstacle.getPointCount();

    int32_t i = 0;
    int32_t j = 0;
    
    while (i < angleOfNormalVectorsRobot.size())
    {
        PolygonVertex polygonVertex;
        polygonVertex.normalAngle = angleOfNormalVectorsRobot[i];
        polygonVertex.vertex = robot.getPoint(i);
        polygonVertex.polygonType = PolygonType::Robot;
        sortedPolygonVertices.push_back(polygonVertex);
        ++i;
    }

    while (j < angleOfNormalVectorsObstacle.size())
    {       
        PolygonVertex polygonVertex;
        polygonVertex.normalAngle = angleOfNormalVectorsObstacle[j];
        polygonVertex.vertex = obstacle.getPoint(j);
        polygonVertex.polygonType = PolygonType::Obstacle;
        sortedPolygonVertices.push_back(polygonVertex);
        ++j;
    }

    sort(sortedPolygonVertices.begin(), sortedPolygonVertices.end(), PolygonVertex::CompAngles);
}

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
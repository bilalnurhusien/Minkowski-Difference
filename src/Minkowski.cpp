#include <iostream>
#include <vector>
#include <fstream>
#include <math.h>
#include <float.h>
#include <algorithm>

#include "../include/Minkowski.hpp"
#include "../include/Types.hpp"
#include "../include/Helpers.hpp"

using namespace std;

/**
 * Shortest distance between two angles.
 */
float Distance(float alpha, float beta) {
    float phi = fmod(fabs(beta - alpha),  360.f);
    float distance = phi > 180.0f ? 360.0f - phi : phi;
    return distance;
}

/**
 * Determines if an angle is between two angles
 */
bool IsAngleBetween(float a, float b, float targetAngle)
{
    float distanceAtoB, distanceAtoTarget, distanceBtoTarget;

    distanceAtoB = Distance(b, a);
    distanceAtoTarget = Distance(targetAngle, a);
    distanceBtoTarget = Distance(targetAngle, b);

    return ((distanceAtoB < 180.0f) && ((distanceAtoTarget + distanceBtoTarget) <= distanceAtoB));
}

/**
 * Get next polygon point that is before or after a point
 * on the unit circle. This is used to calculate the Minkowski
 * difference of two polygons.
 */
bool GetNextPolygonPoint(const uint32_t index,
                         const bool before,
                         const PolygonType nextType,
                         const vector<PolygonVertex>& polygonVertices,
                         PolygonVertex& nextPoint)
{
    const int32_t VertexCount = polygonVertices.size();
    int32_t i = index;
    
    do
    {
        if (before)
        {
            i = (i - 1 + VertexCount) % VertexCount;
        }
        else
        {
            i = (i + 1) % VertexCount;
        }

        if (polygonVertices[i].polygonType == nextType)
        {
            nextPoint = polygonVertices[i];

            return true;
        }
    } while ((i != index) &&
             (Distance(polygonVertices[i].normalAngle, polygonVertices[index].normalAngle) < 180.0f));

    return false;
}

/**
 * Calculate the Minkowksi difference using a list of polygon vertices
 */
bool MinkowskiDifference(const vector<PolygonVertex>& polygonVertices,
                         vector<PolygonVertex>& minkowskiVertices)
{       
    uint32_t vertexCount = polygonVertices.size();

    for (int32_t i = 0; i < vertexCount; ++i)
    {
        PolygonVertex vertexBefore;
        PolygonVertex vertexAfter;
        PolygonVertex polygonVertex = polygonVertices[i];
        PolygonType nextPolygonType;
        
        if (polygonVertex.polygonType == PolygonType::Robot)
        {
            nextPolygonType = PolygonType::Obstacle;
        }
        else if (polygonVertex.polygonType == PolygonType::Obstacle)
        {
            nextPolygonType = PolygonType::Robot;
        }

        if (!GetNextPolygonPoint(i, true, nextPolygonType, polygonVertices, vertexBefore))
        {
            continue;
        }
       
        if (!GetNextPolygonPoint(i, false, nextPolygonType, polygonVertices, vertexAfter))
        {
            continue;
        }

        if (IsAngleBetween(vertexBefore.normalAngle, vertexAfter.normalAngle, polygonVertex.normalAngle))
        {
            if (!GetNextPolygonPoint(i, false, polygonVertex.polygonType, polygonVertices, polygonVertex))
            {
                continue;
            }

            PolygonVertex minkowskiVertex;
            float x, y;

            if (vertexAfter.polygonType == PolygonType::Obstacle)
            {
                x = vertexAfter.vertex.x - polygonVertex.vertex.x;
                y = vertexAfter.vertex.y - polygonVertex.vertex.y;
            }
            else
            {
                x = polygonVertex.vertex.x - vertexAfter.vertex.x;
                y = polygonVertex.vertex.y - vertexAfter.vertex.y;
            }

            /*
             * 57.2957795f = 180 / PI
             */
            minkowskiVertex.vertex = sf::Vector2f(x, y);
            minkowskiVertex.normalAngle = fmod(atan2(y, x) * 57.2957795f + 360.0f, 360.0f);
            minkowskiVertices.push_back(minkowskiVertex);            
        }
    }
    
    sort(minkowskiVertices.begin(), minkowskiVertices.end(), PolygonVertex::CompAngles);
}

/**
  * Get opposite angles (180 deg rotation of an angle)
  */
void GetOppositeAngles(const vector<float>& angles,
                       vector<float>& oppositeAngles)
{
    for (uint32_t i = 0; i < angles.size(); ++i)
    {
        oppositeAngles.push_back(fmod(angles[i] + 180, 360.0f));
    }
}

/*
 * Get angle of normal vectors
 */
bool GetAngleOfNormalVectors(const vector<vector<sf::Vertex> >& normalVectors,
                             vector<float>& angleOfNormalVectors)
{
    angleOfNormalVectors.clear();
    for (uint32_t i  = 0; i < normalVectors.size(); ++i)
    {
        if (normalVectors[i].size() != 2)
        {
            cout << "Invalid number of vertices for normal vector";
            return false;
        }

        float yDiff = (float)normalVectors[i][1].position.y - (float)normalVectors[i][0].position.y;
        float xDiff = (float)normalVectors[i][1].position.x - (float)normalVectors[i][0].position.x;

        /*
         * 57.2957795f = 180 / PI
         */
        float angle = fmod(atan2(yDiff, xDiff) * 57.2957795f + 360.0f, 360.0f);

        angleOfNormalVectors.push_back(angle);

#ifdef DEBUG            
        cout << "Angle of Normal: " << i << ": "<< angle << endl;
#endif
    }
    
    return true;
}

bool MinkowskiDifference(const sf::ConvexShape& robot,
                         const sf::ConvexShape& obstacle,
                         vector<vector<sf::Vertex> >& normalVectorsRobot,
                         vector<vector<sf::Vertex> >& normalVectorsObstacle,
                         vector<float>& angleOfNormalVectorsRobot,
                         vector<float>& angleOfNormalVectorsObstacle,
                         std::vector<PolygonVertex>& minkowskiVertices)
{
    vector<float> angleOfInwardNormalVectorsRobot;
    vector<PolygonVertex> sortedPolygonVertices;
    std::vector<vector<sf::Vertex> > inwardNormalVectorsRobot;
    
    /* Get normal vectors */
    GetNormalVectors<sf::ConvexShape>(robot, OffSetX1, OffSetY1, sf::Color::Red, normalVectorsRobot);
    GetNormalVectors<sf::ConvexShape>(robot, OffSetX1, OffSetY1, sf::Color::Blue, inwardNormalVectorsRobot, false);
    GetNormalVectors<sf::ConvexShape>(obstacle, OffSetX2, OffSetY2, sf::Color::Green, normalVectorsObstacle);
    
    /* Get angle of normal vectors */
    GetAngleOfNormalVectors(normalVectorsRobot, angleOfNormalVectorsRobot);
    GetAngleOfNormalVectors(normalVectorsObstacle, angleOfNormalVectorsObstacle);
    GetOppositeAngles(angleOfNormalVectorsRobot, angleOfInwardNormalVectorsRobot);
    
    MergeAngleOfNormalVectors(robot, obstacle, angleOfInwardNormalVectorsRobot, angleOfNormalVectorsObstacle, sortedPolygonVertices);

    return MinkowskiDifference(sortedPolygonVertices, minkowskiVertices);
}


bool MinkowskiDifference(const sf::CircleShape& robot,
                         const sf::CircleShape& obstacle,
                         vector<vector<sf::Vertex> >& normalVectorsRobot,
                         vector<vector<sf::Vertex> >& normalVectorsObstacle,
                         vector<float>& angleOfNormalVectorsRobot,
                         vector<float>& angleOfNormalVectorsObstacle,
                         std::vector<PolygonVertex>& minkowskiVertices)
{
    vector<float> angleOfInwardNormalVectorsRobot;
    vector<PolygonVertex> sortedPolygonVertices;
    std::vector<vector<sf::Vertex> > inwardNormalVectorsRobot;
    
    /* Get normal vectors */
    GetNormalVectors<sf::CircleShape>(robot, OffSetX1, OffSetY1, sf::Color::Red, normalVectorsRobot);
    GetNormalVectors<sf::CircleShape>(robot, OffSetX1, OffSetY1, sf::Color::Blue, inwardNormalVectorsRobot, false);
    GetNormalVectors<sf::CircleShape>(obstacle, OffSetX2, OffSetY2, sf::Color::Green, normalVectorsObstacle);
    
    /* Get angle of normal vectors */
    GetAngleOfNormalVectors(normalVectorsRobot, angleOfNormalVectorsRobot);
    GetAngleOfNormalVectors(normalVectorsObstacle, angleOfNormalVectorsObstacle);
    GetOppositeAngles(angleOfNormalVectorsRobot, angleOfInwardNormalVectorsRobot);
    
    MergeAngleOfNormalVectors(robot, obstacle, angleOfInwardNormalVectorsRobot, angleOfNormalVectorsObstacle, sortedPolygonVertices);

    return MinkowskiDifference(sortedPolygonVertices, minkowskiVertices);
}
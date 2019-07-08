#include <iostream>
#include <vector>
#include <fstream>
#include <math.h>
#include <float.h>
#include <algorithm>

#include "../include/Minkowski.hpp"
#include "../include/Types.hpp"

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

#ifdef DEBUG
cout << "a : " << a  << " b: " << b << " targetAngle: " << targetAngle << endl;
cout << "Distance AtoB " << distanceAtoB << endl;
cout << "Distance AtoTarget " << distanceAtoTarget << endl;
cout << "Distance BtoTarget " << distanceBtoTarget << endl;
#endif

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
#ifdef DEBUG
            cout << "Is next polygon angle: " << polygonVertices[i].normalAngle << " type: " << polygonVertices[i].polygonType << " before " << polygonVertices[index].normalAngle <<  endl;
#endif
        }
        else
        {
            i = (i + 1) % VertexCount;
#ifdef DEBUG
            cout << "Is next polygon angle: " << polygonVertices[i].normalAngle << " type: " << polygonVertices[i].polygonType << " after " << polygonVertices[index].normalAngle <<  endl;
#endif
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

#ifdef DEBUG
        cout << endl << "============== Test: =================== " << i <<  endl;
        cout << "polygonVertexType: " << polygonVertex.polygonType << " polygonVertexAngle: " << polygonVertex.normalAngle << endl;
        cout << "polygonVertexX: " << polygonVertex.vertex.x << " polygonVertexY: " << polygonVertex.vertex.y << endl;  
#endif

        if (!GetNextPolygonPoint(i, true, nextPolygonType, polygonVertices, vertexBefore))
        {
            continue;
        }
 
#ifdef DEBUG
        cout << "vertexBeforeType: " << vertexBefore.polygonType << " vertexBeforeAngle: " << vertexBefore.normalAngle << endl;
        cout << "vertexBeforeX: " << vertexBefore.vertex.x << " vertexBeforeY: " << vertexBefore.vertex.y << endl;
#endif
      
        if (!GetNextPolygonPoint(i, false, nextPolygonType, polygonVertices, vertexAfter))
        {
            continue;
        }

#ifdef DEBUG
        cout << "vertexAfterType: " << vertexAfter.polygonType << " vertexAfterAngle: " << vertexAfter.normalAngle << endl;
        cout << "vertexAfterx: " << vertexAfter.vertex.x << " vertexAfterY: " << vertexAfter.vertex.y << endl;
#endif

        if (IsAngleBetween(vertexBefore.normalAngle, vertexAfter.normalAngle, polygonVertex.normalAngle))
        {
            if (!GetNextPolygonPoint(i, false, polygonVertex.polygonType, polygonVertices, polygonVertex))
            {
                continue;
            }

#ifdef DEBUG
        cout << endl << "============== Found: =================== " << i <<  endl;
        cout << "polygonVertexType: " << polygonVertex.polygonType << " polygonVertexAngle: " << polygonVertex.normalAngle << endl;
        cout << "polygonVertexX: " << polygonVertex.vertex.x << " polygonVertexY: " << polygonVertex.vertex.y << endl;  
#endif

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

            minkowskiVertex.vertex = sf::Vector2f(x, y);
            minkowskiVertex.normalAngle = fmod(atan2(y, x) * 57.2957795f + 360.0f, 360.0f);
            minkowskiVertices.push_back(minkowskiVertex);            

#ifdef DEBUG
            cout << "New Polygon Point: " << x << " y: " << y << endl;
            cout << "Angle: " << minkowskiVertex.normalAngle << endl;
#endif

        }
    }
    
    sort(minkowskiVertices.begin(), minkowskiVertices.end(), PolygonVertex::CompAngles);
}

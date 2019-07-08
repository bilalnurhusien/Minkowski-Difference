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
 * Sum of two polygons.
 */
bool GetNextPolygonPoint(const uint32_t index,
                         const bool before,
                         const PolygonType nextType,
                         const vector<PolygonEdge>& polygonEdges,
                         PolygonEdge& nextPoint)
{
    const int32_t VertexCount = polygonEdges.size();
    int32_t i = index;
    
    do
    {
        if (before)
        {
            i = (i - 1 + VertexCount) % VertexCount;
    #ifdef DEBUG
            cout << "Is next polygon angle: " << polygonEdges[i].normalAngle << " type: " << polygonEdges[i].polygonType << " before " << polygonEdges[index].normalAngle <<  endl;
    #endif
        }
        else
        {
            i = (i + 1) % VertexCount;
    #ifdef DEBUG
            cout << "Is next polygon angle: " << polygonEdges[i].normalAngle << " type: " << polygonEdges[i].polygonType << " after " << polygonEdges[index].normalAngle <<  endl;
    #endif
        }

        if (polygonEdges[i].polygonType == nextType)
        {
            nextPoint = polygonEdges[i];

            return true;
        }
    } while ((i != index) &&
             (Distance(polygonEdges[i].normalAngle, polygonEdges[index].normalAngle) < 180.0f));
    return false;
}

/**
 * Calculate the Minkowksi sum using a list of polygon edges
 */
bool MinkowskiSum(const vector<PolygonEdge>& polygonEdges,
                  vector<PolygonEdge>& minkowskiVertices)
{       
    uint32_t vertexCount = polygonEdges.size();

    for (int32_t i = 0; i < vertexCount; ++i)
    {
        PolygonEdge edgeBefore;
        PolygonEdge edgeAfter;
        PolygonEdge polygonEdge = polygonEdges[i];
        PolygonType nextPolygonType;
        
        if (polygonEdge.polygonType == PolygonType::Robot)
        {
            nextPolygonType = PolygonType::Obstacle;
        }
        else if (polygonEdge.polygonType == PolygonType::Obstacle)
        {
            nextPolygonType = PolygonType::Robot;
        }

#ifdef DEBUG
        cout << endl << "============== Test: =================== " << i <<  endl;
        cout << "polygontype: " << polygonEdge.polygonType << " polygonangle: " << polygonEdge.normalAngle << endl;
        cout << "polygonx: " << polygonEdge.vertex.x << " polygony: " << polygonEdge.vertex.y << endl;  
#endif

        if (!GetNextPolygonPoint(i, true, nextPolygonType, polygonEdges, edgeBefore))
        {
            continue;
        }
 
#ifdef DEBUG
        cout << "edgeBeforetype: " << edgeBefore.polygonType << " edgeBeforeangle: " << edgeBefore.normalAngle << endl;
        cout << "edgeBeforex: " << edgeBefore.vertex.x << " edgeBeforey: " << edgeBefore.vertex.y << endl;
#endif
      
        if (!GetNextPolygonPoint(i, false, nextPolygonType, polygonEdges, edgeAfter))
        {
            continue;
        }

#ifdef DEBUG
        cout << "aftertype: " << edgeAfter.polygonType << " afterangle: " << edgeAfter.normalAngle << endl;
        cout << "afterx: " << edgeAfter.vertex.x << " aftery: " << edgeAfter.vertex.y << endl;
#endif

        if (IsAngleBetween(edgeBefore.normalAngle, edgeAfter.normalAngle, polygonEdge.normalAngle))
        {
            if (!GetNextPolygonPoint(i, false, polygonEdge.polygonType, polygonEdges, polygonEdge))
            {
                continue;
            }

#ifdef DEBUG
        cout << endl << "============== Found: =================== " << i <<  endl;
        cout << "polygontype: " << polygonEdge.polygonType << " polygonangle: " << polygonEdge.normalAngle << endl;
        cout << "polygonx: " << polygonEdge.vertex.x << " polygony: " << polygonEdge.vertex.y << endl;
#endif
            PolygonEdge minkowskiVertex;
            float x, y;

            if (edgeAfter.polygonType == PolygonType::Obstacle)
            {
                x = edgeAfter.vertex.x - polygonEdge.vertex.x;
                y = edgeAfter.vertex.y - polygonEdge.vertex.y;
            }
            else
            {
                x = polygonEdge.vertex.x - edgeAfter.vertex.x;
                y = polygonEdge.vertex.y - edgeAfter.vertex.y;
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
    
    sort(minkowskiVertices.begin(), minkowskiVertices.end(), PolygonEdge::CompAngles);
}

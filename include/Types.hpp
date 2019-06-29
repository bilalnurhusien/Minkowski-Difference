#pragma once

#include <SFML/Graphics.hpp>

#include <iostream>
#include <vector>
#include <math.h>
#include <stdint.h>
#include <utility>
#include <limits.h>
#include <float.h>
#include <algorithm>

/**
 * Const variables
 */
const float WindowSizeX = 1000.0f;
const float WindowSizeY = 1000.0f;
const float OffSetX1 = 150.0f;
const float OffSetY1 = 100.0f;
const float OffSetX2 = 700.0f;
const float OffSetY2 = 100.0f;
const float ShapeSize = 80.0f;
const float OriginX = 500.0f;
const float OriginY = 500.0f;

/**
 * Struct that represents
 * a point on a polygon.
 */
struct Point
{
    Point(float valX, float valY)
    {
        x = valX;
        y = valY;        
    }
    
    Point():
        x(0),
        y(0)
    {
    }
    
    float x = 0;
    float y = 0;
};

/**
 * Enum used for distinguishing
 * between types of polygons 
 */
enum PolygonType
{
    None = 0,
    Robot = 1,
    Obstacle = 2
};

/**
 * Struct that represents a polygon edge
 */
struct PolygonEdge
{
    /* Polygon type */
    PolygonType polygonType;

    /* Vertex associated with edge */
    sf::Vector2f vertex;
    
    /* Angle normal to edge */
    float normalAngle;
    
    /* Used to sort normal angles of edges */
    static bool CompAngles(const PolygonEdge& p1, const PolygonEdge& p2)
    {
        return p1.normalAngle < p2.normalAngle;
    }
};
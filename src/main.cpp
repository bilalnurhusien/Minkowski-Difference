#include <vector>

#include "../include/Helpers.hpp"
#include "../include/Types.hpp"
#include "../include/Minkowski.hpp"

using namespace std;

int32_t main(int argc, char *argv[])
{
    sf::RenderWindow window(sf::VideoMode(WindowSizeX, WindowSizeY), "Assignment 1 : Minkowski Difference");

    sf::ConvexShape minkowskiShape;
    vector<sf::CircleShape> vecShapes;
    vector<vector<sf::Vertex> > normalVectors1;
    vector<vector<sf::Vertex> > inwardNormalVectors1;  
    vector<vector<sf::Vertex> > normalVectors2;
    vector<float> angleOfNormalVectors1;
    vector<float> angleOfInwardNormalVectors1;
    vector<float> angleOfNormalVectors2;
    vector<PolygonEdge> minkowskiVertices;
    vector<PolygonEdge> polygonEdges;

    /* Process arguments */
    if (!ProcessArguments(argc, argv, vecShapes))
    {
        cout << "Failed to process arguments" << endl;
        return -1;
    }

    /* Get normal vectors */
    GetNormalVectors(vecShapes[0], OffSetX1, OffSetY1, sf::Color::Red, normalVectors1);
    GetNormalVectors(vecShapes[0], OffSetX1, OffSetY1, sf::Color::Blue, inwardNormalVectors1, false);
    GetNormalVectors(vecShapes[1], OffSetX2, OffSetY2, sf::Color::Green, normalVectors2);
    
    /* Get angle of normal vectors */
    GetAngleOfNormalVectors(normalVectors1, angleOfNormalVectors1);
    GetAngleOfNormalVectors(normalVectors2, angleOfNormalVectors2);
    GetOppositeAngles(angleOfNormalVectors1, angleOfInwardNormalVectors1);  
    MergeAngleOfNormalVectors(vecShapes, angleOfInwardNormalVectors1, angleOfNormalVectors2, polygonEdges);

#ifdef DEBUG
    for (uint32_t i = 0; i < polygonEdges.size(); ++i)
    {
        cout << "Point: " << polygonEdges[i].vertex.x << ", " << polygonEdges[i].vertex.y << endl;
        cout << "Angle: " << polygonEdges[i].normalAngle << endl;
        cout << "Type: " << polygonEdges[i].polygonType << endl;
    }
#endif

    /* Calculate Minkowski Sum of edges */
    MinkowskiSum(polygonEdges, minkowskiVertices);
    minkowskiShape.setPointCount(minkowskiVertices.size());
    minkowskiShape.setPosition(OriginX, OriginY);

    for (uint32_t i = 0; i < minkowskiVertices.size(); ++i)
    {
        cout << "Minkowski Vertex: " << minkowskiVertices[i].vertex.x << " " << minkowskiVertices[i].vertex.y << endl;
        cout << "Minkowski Angle: " << minkowskiVertices[i].normalAngle << endl;
        minkowskiShape.setPoint(i, minkowskiVertices[i].vertex);
    }

    /* Draw y and x-axis */
    sf::Vertex yAxis[] =
    {
        sf::Vertex(sf::Vector2f(OriginX, 0)),
        sf::Vertex(sf::Vector2f(OriginX, WindowSizeY))
    };
    
    sf::Vertex xAxis[] =
    {
        sf::Vertex(sf::Vector2f(0, OriginY)),
        sf::Vertex(sf::Vector2f(WindowSizeX, OriginY))
    };

    /* Select font */
    sf::Text text;
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeMono.ttf"))
    {
        cout << "Warning: failed to load font" << endl;
    }

    text.setFont(font);

    /* In pixels, not points! */
    text.setCharacterSize(18); 

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if ((event.type == sf::Event::Closed) ||
                (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)))
                window.close();
        }

        window.draw(yAxis, 2, sf::Lines);
        window.draw(xAxis, 2, sf::Lines);
        window.draw(vecShapes[0]);
        window.draw(vecShapes[1]);
        window.draw(minkowskiShape);

        for (uint32_t i = 0; i < normalVectors1.size(); ++i)
        {
            sf::Vertex* normalVector = &(normalVectors1[i][0]);
            window.draw(normalVector, 2, sf::Lines);
            
            text.setString(ToStringSetPrecision(angleOfNormalVectors1[i], 2));
            text.setPosition(normalVectors1[i][1].position.x, normalVectors1[i][1].position.y);
            window.draw(text);
        }

        for (uint32_t i = 0; i < normalVectors2.size(); ++i)
        {
            sf::Vertex* normalVector = &(normalVectors2[i][0]);
            window.draw(normalVector, 2, sf::Lines);
            
            text.setString(ToStringSetPrecision(angleOfNormalVectors2[i], 2));
            text.setPosition(normalVectors2[i][1].position.x, normalVectors2[i][1].position.y);
            window.draw(text);
        }
        
        window.display();
    }

    return 0;
}
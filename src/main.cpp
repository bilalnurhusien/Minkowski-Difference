#include <vector>

#include "../include/Helpers.hpp"
#include "../include/Types.hpp"
#include "../include/Minkowski.hpp"

using namespace std;

int32_t main(int argc, char *argv[])
{
    sf::RenderWindow window(sf::VideoMode(WindowSizeX, WindowSizeY), "Assignment 1 : Minkowski Difference");

    sf::ConvexShape minkowskiShape;
    vector<sf::CircleShape> polygonShapesVec;
    vector<sf::ConvexShape> manualPolygonShapesVec;
    vector<vector<sf::Vertex> > normalVectors1;
    vector<vector<sf::Vertex> > inwardNormalVectors1;  
    vector<vector<sf::Vertex> > normalVectors2;
    vector<float> angleOfNormalVectors1;
    vector<float> angleOfInwardNormalVectors1;
    vector<float> angleOfNormalVectors2;
    vector<PolygonVertex> minkowskiVertices;
    vector<PolygonVertex> sortedPolygonVertices;

    /* Process arguments */
    if (!ProcessArguments(argc, argv, polygonShapesVec, manualPolygonShapesVec))
    {
        return -1;
    }

    if (manualPolygonShapesVec.empty() &&
        polygonShapesVec.empty())
    {
        cout << "Failed to create polygons" << endl;

        return -1;
    }

    if ((!manualPolygonShapesVec.empty()) && 
        (manualPolygonShapesVec.size() < 2))
    {
        cout << "Invalid input file" << endl;
        return -1;
    }
    
#ifdef DEBUG
    cout << "Manual " << manualPolygonShapesVec.size() << " Automatic " << polygonShapesVec.size() <<endl;
    if (!manualPolygonShapesVec.empty())
    {
        for (uint32_t i = 0; i < manualPolygonShapesVec[0].getPointCount(); ++i)
        {
            sf::Vector2f vertex = manualPolygonShapesVec[0].getPoint(i);

            cout << vertex.x << ", " << vertex.y << endl;
        }

        for (uint32_t i = 0; i < manualPolygonShapesVec[1].getPointCount(); ++i)
        {
            sf::Vector2f vertex = manualPolygonShapesVec[1].getPoint(i);

            cout << vertex.x << ", " << vertex.y << endl;
        }
    }
#endif

    if (!polygonShapesVec.empty())
    {
        /* Get normal vectors */
        GetNormalVectors(polygonShapesVec[0], OffSetX1, OffSetY1, sf::Color::Red, normalVectors1);
        GetNormalVectors(polygonShapesVec[0], OffSetX1, OffSetY1, sf::Color::Blue, inwardNormalVectors1, false);
        GetNormalVectors(polygonShapesVec[1], OffSetX2, OffSetY2, sf::Color::Green, normalVectors2);
    }
    else
    {
        /* Get normal vectors */
        GetNormalVectors(manualPolygonShapesVec[0], OffSetX1, OffSetY1, sf::Color::Red, normalVectors1);
        GetNormalVectors(manualPolygonShapesVec[0], OffSetX1, OffSetY1, sf::Color::Blue, inwardNormalVectors1, false);
        GetNormalVectors(manualPolygonShapesVec[1], OffSetX2, OffSetY2, sf::Color::Green, normalVectors2);
    }

    /* Get angle of normal vectors */
    GetAngleOfNormalVectors(normalVectors1, angleOfNormalVectors1);
    GetAngleOfNormalVectors(normalVectors2, angleOfNormalVectors2);
    GetOppositeAngles(angleOfNormalVectors1, angleOfInwardNormalVectors1);

    if (!polygonShapesVec.empty())
    {
        MergeAngleOfNormalVectors(polygonShapesVec, angleOfInwardNormalVectors1, angleOfNormalVectors2, sortedPolygonVertices);
    }
    else
    {
        MergeAngleOfNormalVectors(manualPolygonShapesVec, angleOfInwardNormalVectors1, angleOfNormalVectors2, sortedPolygonVertices);
    }

#ifdef DEBUG
    for (uint32_t i = 0; i < sortedPolygonVertices.size(); ++i)
    {
        cout << "Point: " << sortedPolygonVertices[i].vertex.x << ", " << sortedPolygonVertices[i].vertex.y << endl;
        cout << "Angle: " << sortedPolygonVertices[i].normalAngle << endl;
        cout << "Type: " << sortedPolygonVertices[i].polygonType << endl;
    }
#endif

    /* Calculate Minkowski difference of polygons */
    MinkowskiDifference(sortedPolygonVertices, minkowskiVertices);
    minkowskiShape.setPointCount(minkowskiVertices.size());
    minkowskiShape.setPosition(OriginX, OriginY);
    minkowskiShape.setFillColor(sf::Color::Cyan);

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
        if (polygonShapesVec.size() == 2)
        {
            text.setString("Robot");
            text.setPosition(polygonShapesVec[0].getPosition().x, polygonShapesVec[0].getPosition().y);
            window.draw(polygonShapesVec[0]);
            window.draw(text);

            text.setString("Obstacle");
            text.setPosition(polygonShapesVec[1].getPosition().x, polygonShapesVec[1].getPosition().y);
            window.draw(polygonShapesVec[1]);
            window.draw(text);
        }
        else
        {

            text.setString("Robot");
            text.setPosition(manualPolygonShapesVec[0].getPosition().x, manualPolygonShapesVec[0].getPosition().y);
            window.draw(manualPolygonShapesVec[0]);
            window.draw(text);

            text.setString("Obstacle");
            text.setPosition(manualPolygonShapesVec[1].getPosition().x, manualPolygonShapesVec[1].getPosition().y);
            window.draw(manualPolygonShapesVec[1]);
            window.draw(text);
        }

        window.draw(minkowskiShape);
        text.setString("C-Obstacle");
        text.setPosition(minkowskiShape.getPosition().x, minkowskiShape.getPosition().y);
        window.draw(text);

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
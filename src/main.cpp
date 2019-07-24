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
    vector<float> angleOfNormalVectorsRobot;
    vector<float> angleOfNormalVectorsObstacle;
    vector<vector<sf::Vertex> > normalVectorsRobot;
    vector<vector<sf::Vertex> > normalVectorsObstacle;
    vector<PolygonVertex> minkowskiVertices;

    /* Process arguments */
    if (!ProcessArguments(argc, argv, polygonShapesVec, manualPolygonShapesVec))
    {
        return -1;
    }

    /* Calculate Minkowski difference of robot and obstacle */

    if (!polygonShapesVec.empty())
    {
        MinkowskiDifference(
            polygonShapesVec[0],
            polygonShapesVec[1],
            normalVectorsRobot,
            normalVectorsObstacle,
            angleOfNormalVectorsRobot,
            angleOfNormalVectorsObstacle,
            minkowskiVertices);
    }
    else
    {
        MinkowskiDifference(
            manualPolygonShapesVec[0],
            manualPolygonShapesVec[1],
            normalVectorsRobot,
            normalVectorsObstacle,
            angleOfNormalVectorsRobot,
            angleOfNormalVectorsObstacle,
            minkowskiVertices);
    }

    minkowskiShape.setPointCount(minkowskiVertices.size());
    minkowskiShape.setPosition(OriginX, OriginY);
    minkowskiShape.setFillColor(sf::Color::Red);

    for (uint32_t i = 0; i < minkowskiVertices.size(); ++i)
    {
        cout << "Minkowski Vertex " << i << ":  " << minkowskiVertices[i].vertex.x << " " << minkowskiVertices[i].vertex.y << endl;
        cout << "Minkowski Angle " << i << ":  " << minkowskiVertices[i].normalAngle << endl << endl;
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
    text.setCharacterSize(18); 
    text.setColor(sf::Color::White);

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if ((event.type == sf::Event::Closed) ||
                (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)))
            {
                window.close();
            }
        }

        /* Draw X and Y axes */
        window.draw(yAxis, 2, sf::Lines);
        window.draw(xAxis, 2, sf::Lines);

        if (polygonShapesVec.size() == 2)
        {
            text.setString("Robot");
            text.setPosition(polygonShapesVec[0].getPosition().x + ShapeSize, polygonShapesVec[0].getPosition().y + ShapeSize);
            window.draw(polygonShapesVec[0]);
            window.draw(text);

            text.setString("Obstacle");
            text.setPosition(polygonShapesVec[1].getPosition().x + ShapeSize, polygonShapesVec[1].getPosition().y + ShapeSize);
            window.draw(polygonShapesVec[1]);
            window.draw(text);
        }
        else
        {
            text.setString("Robot");
            text.setPosition(manualPolygonShapesVec[0].getPosition().x + ShapeSize, manualPolygonShapesVec[0].getPosition().y + ShapeSize);
            window.draw(manualPolygonShapesVec[0]);
            window.draw(text);

            text.setString("Obstacle");
            text.setPosition(manualPolygonShapesVec[1].getPosition().x + ShapeSize, manualPolygonShapesVec[1].getPosition().y + ShapeSize);
            window.draw(manualPolygonShapesVec[1]);
            window.draw(text);
        }

        /* Draw C-Obstacle */
        window.draw(minkowskiShape);
        text.setString("C-Obstacle");
        text.setPosition(minkowskiShape.getPosition().x, minkowskiShape.getPosition().y);
        window.draw(text);

        /* Draw normal vectors */
        for (uint32_t i = 0; i < normalVectorsRobot.size(); ++i)
        {
            sf::Vertex* normalVector = &(normalVectorsRobot[i][0]);
            window.draw(normalVector, 2, sf::Lines);
            
            text.setString(ToStringSetPrecision(angleOfNormalVectorsRobot[i], 2));
            text.setPosition(normalVectorsRobot[i][1].position.x, normalVectorsRobot[i][1].position.y);
            window.draw(text);
        }

        for (uint32_t i = 0; i < normalVectorsObstacle.size(); ++i)
        {
            sf::Vertex* normalVector = &(normalVectorsObstacle[i][0]);
            window.draw(normalVector, 2, sf::Lines);
            
            text.setString(ToStringSetPrecision(angleOfNormalVectorsObstacle[i], 2));
            text.setPosition(normalVectorsObstacle[i][1].position.x, normalVectorsObstacle[i][1].position.y);
            window.draw(text);
        }
        
        window.display();
    }

    return 0;
}
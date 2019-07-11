#include <string.h>
#include <fstream>

#include "../include/Helpers.hpp"

using namespace std;

/**
  * Print help messages
  */
void PrintHelp()
{
    cout << "Usage:" << endl;
    cout << "\t./minkowski <num-of-polygon1-verticies> <num-of-polygon2-verticies>" << endl;
    cout << "Example:\n\t./minkowski 6 5 # For a hexagon and a pentagon" << endl;
    cout << "\n\t./minkowski -f <input-file> # CSV input file" << endl;
}

/* Check if a string is a numerical value */
bool IsNumeric(const std::string& str) {
    try
    {
        size_t sz;
        std::stol(str, &sz);
        return sz == str.size();
    }
    catch (const std::exception& ex)
    {
        // if no conversion could be performed.
        return false;   
    }
}

inline bool FileExists (const std::string& name) {
    ifstream f(name.c_str());
    return f.good();
}

bool ReadFile(string fileName,
              vector<sf::ConvexShape>& manualPolygonShapes)
{
    string line;
	ifstream infile;
	infile.open(fileName);
	if(infile.fail())
	{
		cout << "Problem opening file" << endl;
		return false;
	}
    
    uint32_t index = 0;

    manualPolygonShapes = vector<sf::ConvexShape>(2);

    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        int x, y, point = 0;
        char comma;
        sf::Color color;
        size_t n = std::count(line.begin(), line.end(), ',');
        manualPolygonShapes[index].setPointCount(n);

        while (!iss.eof())
        {
            if (!(iss >> x >> comma >> y))
            {
                cout << "Failed to read line " << index << endl;
                return false;
            }

            manualPolygonShapes[index].setPoint(point, sf::Vector2f(x, y));
            point++;
        }

        index++;
    }

    manualPolygonShapes[0].setPosition(OffSetX1, OffSetY1);
    manualPolygonShapes[0].setFillColor(sf::Color::Green);

    manualPolygonShapes[1].setPosition(OffSetX2, OffSetY2);
    manualPolygonShapes[1].setFillColor(sf::Color::Blue);
    return true;
}
/**
  * Process arguments
  */
bool ProcessArguments(int argc,
                      char* argvp[],
                      vector<sf::CircleShape>& vecShapes,
                      vector<sf::ConvexShape>& manualPolygonShapes)
{
    if (argc < 3)
    {
        PrintHelp();
        return false;
    }

    if (strcmp(argvp[1], "-f") == 0)
    {
        string fileName = argvp[2];

        if (!ReadFile(fileName, manualPolygonShapes))
        {
            return false;
        }

        return true;
    }

    if ((!IsNumeric(argvp[1])) ||
        (!IsNumeric(argvp[2])))
    {
        cout << "Invalid numerical argument" << endl;

        PrintHelp();

        return false;
    }

    int32_t vertexNum1 = strtol(argvp[1], nullptr, 10);

    if (vertexNum1 < 3)
    {
        cout << "Please use a convex polygon\n";
        PrintHelp();
        return false;
    }

    int32_t vertexNum2 = strtol(argvp[2], nullptr, 10);

    if (vertexNum2 < 3)
    {
        cout << "Please use a convex polygon\n";
        PrintHelp();
        return false;
    }

    sf::CircleShape polygon1(ShapeSize, vertexNum1);
    polygon1.setFillColor(sf::Color::Green);
    polygon1.setPosition(OffSetX1, OffSetY1);

    sf::CircleShape polygon2(ShapeSize, vertexNum2);
    polygon2.setFillColor(sf::Color::Blue);
    polygon2.setPosition(OffSetX2, OffSetY2);

    vecShapes.push_back(polygon1);
    vecShapes.push_back(polygon2);

    return true;        
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

/**
  * Get normal vectors of all edges in a polygon
  */
void GetNormalVectors(const sf::ConvexShape& shape,
                      const float offsetX,
                      const float offsetY,
                      const sf::Color color,
                      vector<vector<sf::Vertex> >& normalVectors,
                      bool outwardNormal)
{
    vector<Point> vectors;
    vector<Point> midPoints;

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

#ifdef DEBUG

        cout << "Vertex " << i << " : " << vertex.x << ", " << vertex.y << endl;

        cout << "Vector " << i << " : " << vector.x << ", " << vector.y << endl;

        cout << "MidPoint vertex " << i << " : " << midPoint.x << ", " << midPoint.y << endl;

        cout << "Next Vertex " << (i + 1) % vertexCount  << " : " << nextVertex.x << ", " << nextVertex.y << endl;
#endif

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

        normalVectors.push_back(vector<sf::Vertex>());
        normalVectors[i].push_back(v1);
        normalVectors[i].push_back(v2);

#ifdef EXTRA_DEBUG
        cout << "Normal vertex (x1, y1): " << normalVectors[i][0].position.x << " " << normalVectors[i][0].position.y  << endl;
        cout << "Normal vertex (x2, y2): " << normalVectors[i][1].position.x << " " << normalVectors[i][1].position.y  << endl;
        cout << normalVectors.size() << endl;
#endif
    }
}


/**
  * Get normal vectors of all edges in a polygon
  */
void GetNormalVectors(const sf::CircleShape& shape,
                      const float offsetX,
                      const float offsetY,
                      const sf::Color color,
                      vector<vector<sf::Vertex> >& normalVectors,
                      bool outwardNormal)
{
    vector<Point> vectors;
    vector<Point> midPoints;

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

#ifdef DEBUG
        cout << "Vertex " << i << " : " << vertex.x << ", " << vertex.y << endl;

        cout << "Vector " << i << " : " << vector.x << ", " << vector.y << endl;
            
        cout << "MidPoint vertex " << i << " : " << midPoint.x << ", " << midPoint.y << endl;
        
        cout << "Next Vertex " << (i + 1) % vertexCount  << " : " << nextVertex.x << ", " << nextVertex.y << endl;
#endif

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

        normalVectors.push_back(vector<sf::Vertex>());
        normalVectors[i].push_back(v1);
        normalVectors[i].push_back(v2);

#ifdef EXTRA_DEBUG
        cout << "Normal vertex (x1, y1): " << normalVectors[i][0].position.x << " " << normalVectors[i][0].position.y  << endl;
        cout << "Normal vertex (x2, y2): " << normalVectors[i][1].position.x << " " << normalVectors[i][1].position.y  << endl;
        cout << normalVectors.size() << endl;
#endif
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

/**
 * Combine list of normal vectors
 */
void MergeAngleOfNormalVectors(const vector<sf::ConvexShape>& vecShapes,
                               const vector<float>& angleOfNormalVectors1,
                               const vector<float>& angleOfNormalVectors2,
                               vector<PolygonVertex>& sortedPolygonVertices)
{
    size_t vertexCount1 = vecShapes[0].getPointCount();
    size_t vertexCount2 = vecShapes[1].getPointCount();

    int32_t i = 0;
    int32_t j = 0;
    
    while (i < angleOfNormalVectors1.size())
    {
        PolygonVertex polygonVertex;
        polygonVertex.normalAngle = angleOfNormalVectors1[i];
        polygonVertex.vertex = vecShapes[0].getPoint(i);
        polygonVertex.polygonType = PolygonType::Robot;
        sortedPolygonVertices.push_back(polygonVertex);
        ++i;
    }

    while (j < angleOfNormalVectors2.size())
    {       
        PolygonVertex polygonVertex;
        polygonVertex.normalAngle = angleOfNormalVectors2[j];
        polygonVertex.vertex = vecShapes[1].getPoint(j);
        polygonVertex.polygonType = PolygonType::Obstacle;
        sortedPolygonVertices.push_back(polygonVertex);
        ++j;
    }

    sort(sortedPolygonVertices.begin(), sortedPolygonVertices.end(), PolygonVertex::CompAngles);
}

/**
 * Combine list of normal vectors
 */
void MergeAngleOfNormalVectors(const vector<sf::CircleShape>& vecShapes,
                               const vector<float>& angleOfNormalVectors1,
                               const vector<float>& angleOfNormalVectors2,
                               vector<PolygonVertex>& sortedPolygonVertices)
{
    size_t vertexCount1 = vecShapes[0].getPointCount();
    size_t vertexCount2 = vecShapes[1].getPointCount();

    int32_t i = 0;
    int32_t j = 0;

    while (i < angleOfNormalVectors1.size())
    {
        PolygonVertex polygonVertex;
        polygonVertex.normalAngle = angleOfNormalVectors1[i];
        polygonVertex.vertex = vecShapes[0].getPoint(i);
        polygonVertex.polygonType = PolygonType::Robot;
        sortedPolygonVertices.push_back(polygonVertex);
        ++i;
    }

    while (j < angleOfNormalVectors2.size())
    {       
        PolygonVertex polygonVertex;
        polygonVertex.normalAngle = angleOfNormalVectors2[j];
        polygonVertex.vertex = vecShapes[1].getPoint(j);
        polygonVertex.polygonType = PolygonType::Obstacle;
        sortedPolygonVertices.push_back(polygonVertex);
        ++j;
    }
    
    sort(sortedPolygonVertices.begin(), sortedPolygonVertices.end(), PolygonVertex::CompAngles);
}
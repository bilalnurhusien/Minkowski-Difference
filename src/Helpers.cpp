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
    cout << "\t./minkowski <num-of-robot-vertices> <num-of-obstacle-vertices>" << endl;
    cout << "Example:\n\t./minkowski 6 5 # For a hexagon robot and a pentagon obstacle" << endl;
    cout << "\n\t./minkowski -f <csv-input-file>" << endl;
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

        if ((!manualPolygonShapesVec.empty()) && 
            (manualPolygonShapesVec.size() < 2))
        {
            cout << "Invalid input file" << endl;
            return -1;
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


#pragma once

#include "Types.hpp"

/**
 * Calculate the Minkowksi sum using a list of polygon edges
 */
bool MinkowskiSum(const std::vector<PolygonEdge>& polygonEdges,
                  std::vector<PolygonEdge>& minkowskiVertices);
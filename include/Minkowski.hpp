#pragma once

#include "Types.hpp"

/**
 * Calculate the Minkowksi difference using a list of polygon vertices
 */
bool MinkowskiDifference(const std::vector<PolygonVertex>& polygonVertices,
                         std::vector<PolygonVertex>& minkowskiVertices);
#pragma once
#include "Vectors.h"

/// <summary>
/// Checks intersection between ray casted from Ro point to Rd direction and sphere at Sp position and Sr radius
/// </summary>
/// <param name="ro">Casted ray emmit point</param>
/// <param name="rd">Direction of casted ray</param>
/// <param name="sp">Sphere postion</param>
/// <param name="sr">Sphere radius</param>
/// <returns>Distance to the intersection point. If no intersection returns -1</returns>
float RISphere(Vector3 ro, Vector3 rd, Vector3 sp, float r);
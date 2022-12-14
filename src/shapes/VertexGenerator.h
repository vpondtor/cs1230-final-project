#pragma once

#include "shapes/Cone.h"
#include "shapes/Cube.h"
#include "shapes/Cylinder.h"
#include "shapes/MarchingCube.h"
#include "shapes/Sphere.h"
#include "utils/scenedata.h"

class VertexGenerator
{
public:
    VertexGenerator();
    void updateParameters(float param1, float param2);
    std::vector<float> generateShape(PrimitiveType primitive);

    // FIXME: THIS DOESN'T REALLY BELONG HERE
    MarchingCube m_marchingCube;
private:
    Cone m_cone;
    Cube m_cube;
    Cylinder m_cylinder;
    Sphere m_sphere;
};


#include "VertexGenerator.h"
#include "settings.h"

VertexGenerator::VertexGenerator()
{
    m_cone = Cone();
    m_cube = Cube();
    m_cylinder = Cylinder();
    m_sphere = Sphere();
    m_marchingCube = MarchingCube();

    updateParameters(settings.shapeParameter1, settings.shapeParameter2);
}

void VertexGenerator::updateParameters(float param1, float param2) {
    m_cone.updateParams(param1, param2);
    m_cube.updateParams(param1);
    m_cylinder.updateParams(param1, param2);
    m_sphere.updateParams(param1, param2);
    m_marchingCube.updateParams(param1, param2);
}

std::vector<float> VertexGenerator::generateShape(PrimitiveType primitive) {
    switch (primitive) {
    case PrimitiveType::PRIMITIVE_CONE:
        return m_cone.generateShape();
    case PrimitiveType::PRIMITIVE_CUBE:
        return m_cube.generateShape();
    case PrimitiveType::PRIMITIVE_CYLINDER:
        return m_cylinder.generateShape();
    case PrimitiveType::PRIMITIVE_SPHERE:
        return m_sphere.generateShape();
    case PrimitiveType::PRIMITIVE_MARCHINGCUBE:
        return m_marchingCube.generateShape();
    default:
        return m_cube.generateShape();
    }
}

#include "MarchingCube.h"
#include "settings.h"
#include <ostream>
#include <iostream>
#include <map>
#include <glm/gtc/noise.hpp>

void MarchingCube::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_numCubes = param2;
    m_layers = param1;

    updateVertexValues();
    setVertexData();
}

void MarchingCube::updateThreshold(float threshold){
    m_vertexData = std::vector<float>();
    m_threshold = threshold;
    setVertexData();
}

float MarchingCube::generateValue(glm::vec3 pos) {
    float radius = 0.5;

    float val = abs(glm::perlin(pos + glm::vec3(r, r1, r2)));
    if (settings.extraCredit2) {
//        val = pow(pos[0], 2) + pow(pos[1], 2) + pow(pos[2], 2) - pow(0.5f, 2);
//        val = abs(glm::simplex(pos));
    }
//
//    float val = radius - glm::length(pos) + glm::perlin(pos);
    return val;
}

void MarchingCube::updateVertexValues() {
    float step = 0.1f;
    glm::vec3 base((step * m_numCubes)/-2.f);
    if (settings.extraCredit1) {
        step = 1.f / m_numCubes;
        base = glm::vec3(-0.5f);
    }

    if (settings.extraCredit2) {
        float X = 100.f;
        r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/X));
        r1 = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/X));
        r2 = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/X));
    }

    std::vector<std::vector<std::vector<MarchingCubeVertex>>> vertices{};

    for (int z = 0; z <= m_numCubes; z++) {
        std::vector<std::vector<MarchingCubeVertex>> plane{};
        for (int y = 0; y <= m_layers; y++) {
            std::vector<MarchingCubeVertex> row{};
            for (int x = 0; x <= m_numCubes; x++) {
                glm::vec3 offset(step * x, step * y, step * z);

                //FIXME
//                float val = 0.5f;
//                float val = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
//                float val = perlin.fractal(x, y, z);
//                float val = m_perlin.noise(x, y, z);
//                val = glm::noise3(glm::vec3(x, y, z));

                // This one could work with interpolation!
                // THE PROBLEM IS I WAS USING x, y, z
//                val = abs(glm::simplex(glm::vec3(x, y, z)));

                // FIXME: FIGURE OUT HOW TO GET GLM::PERLIN NOISE!
                // TODO: Take the terrain generator from lab 7 and generalize it to 3D
                float val;
                if (x == 0 || y == 0 || z == 0 || x == m_numCubes || y == m_layers || z == m_numCubes) {
                    val = -INFINITY;
                } else {
                    val = generateValue(base + offset);
                }

                row.push_back(MarchingCubeVertex{base + offset, val});
            }
            plane.push_back(row);
        }
        vertices.push_back(plane);
    }
    m_marchingCubeVertices = vertices;
}

void MarchingCube::makeTriangle(glm::vec3 first,
                                glm::vec3 second,
                                glm::vec3 third) {

    glm::vec3 normal = glm::normalize(glm::cross(third - second, first - second));

    insertVec3(m_vertexData, first);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, second);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, third);
    insertVec3(m_vertexData, normal);
}

void MarchingCube::setVertexData() {
    for (int z = 0; z < m_numCubes; z++) {
        for (int y = 0; y < m_layers; y++) {
            for (int x = 0; x < m_numCubes; x++) {
                std::vector<MarchingCubeVertex> edgeVertices = findSingleCube(x, y, z);
                int configuration = findConfiguration(edgeVertices, m_threshold);
                std::vector<glm::vec3> intersectPoints = findIntersectPoints(edgeVertices, configuration);
                std::vector<int> triangleBlueprint = m_triTable[configuration];
                for (int i = 0; i < triangleBlueprint.size(); i += 3) {
                    if (triangleBlueprint[i] == -1) {
                        continue;
                    }

                    makeTriangle(intersectPoints[triangleBlueprint[i]], intersectPoints[triangleBlueprint[i + 1]], intersectPoints[triangleBlueprint[i+2]]);
                }
            }
        }
    }
}

glm::vec3 interpolate(MarchingCubeVertex p1, MarchingCubeVertex p2) {
    //FIXME: DOESNT WORK
//    float alpha = p2.val / (p1.val + p2.val);
    float alpha = 0.5f;
    return p1.position + alpha * (p2.position - p1.position);
}

std::vector<glm::vec3> MarchingCube::findIntersectPoints(std::vector<MarchingCubeVertex> &edgeVertices, int configuration) {
    std::vector<glm::vec3> intersectPoints(12);
    int edges = m_edgeTable[configuration];

    for (int i = 0; i < 12; i++) {
        if (edges & int(pow(2, i))) {
            intersectPoints[i] = interpolate(
                edgeVertices[m_edgeEndPoints[i][0]],
                edgeVertices[m_edgeEndPoints[i][1]]);
        } else {
            intersectPoints[i] = glm::vec3(0.f);
        }
    }

    return intersectPoints;
}

std::vector<MarchingCubeVertex> MarchingCube::findSingleCube(int x, int y, int z) {
    std::vector<MarchingCubeVertex> vertices{};
    vertices.push_back(m_marchingCubeVertices[z][y][x]);
    vertices.push_back(m_marchingCubeVertices[z][y][x+1]);
    vertices.push_back(m_marchingCubeVertices[z+1][y][x+1]);
    vertices.push_back(m_marchingCubeVertices[z+1][y][x]);
    vertices.push_back(m_marchingCubeVertices[z][y+1][x]);
    vertices.push_back(m_marchingCubeVertices[z][y+1][x+1]);
    vertices.push_back(m_marchingCubeVertices[z+1][y+1][x+1]);
    vertices.push_back(m_marchingCubeVertices[z+1][y+1][x]);
    return vertices;
}

int MarchingCube::findConfiguration(std::vector<MarchingCubeVertex> &edgeVertices, float threshold) {
    int configuration = 0;
    for (int i = 0; i < 8; i++) {
        if (edgeVertices[i].val > threshold) {
            configuration += pow(2, i);
        }
    }
    return configuration;
}

void MarchingCube::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

// Helpful for testing
void MarchingCube::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {

    glm::vec3 normal = glm::normalize(glm::cross(bottomRight - bottomLeft, topLeft - bottomLeft));

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal);
}

void MarchingCube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {

    glm::vec3 incrementRow(0);
    glm::vec3 incrementCol(0);

    glm::vec3 normal = glm::normalize(glm::cross(bottomRight - bottomLeft, topLeft - bottomLeft));
    if (normal[0] != 0) {
        // on x plane
        incrementRow[2] = bottomRight[2] - topLeft[2];
        incrementCol[1] = bottomRight[1] - topLeft[1];
    } else if (normal[1] != 0) {
        // on y plane
        incrementRow[0] = bottomRight[0] - topLeft[0];
        incrementCol[2] = bottomRight[2] - topLeft[2];
    } else {
        // on z plane
        incrementRow[1] = bottomRight[1] - topLeft[1];
        incrementCol[0] = bottomRight[0] - topLeft[0];
    }

    incrementRow /= m_param1;
    incrementCol /= m_param1;

    for (int row = 0; row < m_param1; row++) {
        for (int col = 0; col < m_param1; col++) {
            glm::vec3 current = topLeft + float(col) * incrementCol + float(row) * incrementRow;
            makeTile(current, current + incrementCol, current + incrementRow, current + incrementCol + incrementRow);
        }
    }
}

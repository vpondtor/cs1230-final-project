#include "Cube.h"
#include <ostream>
#include <iostream>

void Cube::updateParams(int param1) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    setVertexData();
}

void Cube::makeTile(glm::vec3 topLeft,
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

void Cube::makeFace(glm::vec3 topLeft,
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

void Cube::setVertexData() {
    // Front on z plane
     makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
              glm::vec3( 0.5f,  0.5f, 0.5f),
              glm::vec3(-0.5f, -0.5f, 0.5f),
              glm::vec3( 0.5f, -0.5f, 0.5f));

    // Back on z plane
    makeFace(glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f));

//     // Top on y plane
     makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
              glm::vec3( 0.5f,  0.5f, 0.5f),
              glm::vec3(-0.5f, 0.5f, -0.5f),
              glm::vec3( 0.5f, 0.5f, -0.5f));

//     // Back on y plane
     makeFace(glm::vec3( 0.5f,  -0.5f, 0.5f),
              glm::vec3(-0.5f,  -0.5f, 0.5f),
              glm::vec3( 0.5f, -0.5f, -0.5f),
              glm::vec3(-0.5f, -0.5f, -0.5f));

     // Front on x plane
     makeFace(glm::vec3(0.5f,  0.5f, -0.5f),
              glm::vec3(0.5f,  0.5f, 0.5f),
              glm::vec3(0.5f, -0.5f, -0.5f),
              glm::vec3(0.5f, -0.5f, 0.5f));

     // Back on x plane
     makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
              glm::vec3(-0.5f,  0.5f, -0.5f),
              glm::vec3(-0.5f, -0.5f, 0.5f),
              glm::vec3(-0.5f, -0.5f, -0.5f));

}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cube::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

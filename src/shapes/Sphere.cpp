#include "Sphere.h"
#include "glm/ext/scalar_constants.hpp"

void Sphere::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}

void Sphere::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(topLeft));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, glm::normalize(bottomLeft));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(bottomRight));

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(bottomRight));
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, glm::normalize(topRight));
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(topLeft));
}

glm::vec3 sphericalToCartesian(glm::vec3 polar) {
    float r = polar[0];
    float theta = polar[1];
    float phi = polar[2];

    float x = r * sin(phi) * sin(theta);
    float y = r * cos(phi);
    float z = r * sin(phi) * cos(theta);
    return glm::vec3(x, y, z);
}

void Sphere::makeWedge(float currentTheta, float nextTheta) {
    float step = M_PI / m_param1;

    for (float phi = 0; phi < M_PI; phi += step) {
        glm::vec3 topLeft = sphericalToCartesian(glm::vec3(0.5, currentTheta, phi));
        glm::vec3 topRight = sphericalToCartesian(glm::vec3(0.5, nextTheta, phi));
        glm::vec3 botLeft = sphericalToCartesian(glm::vec3(0.5, currentTheta, phi + step));
        glm::vec3 botRight = sphericalToCartesian(glm::vec3(0.5, nextTheta, phi + step));
        makeTile(topLeft, topRight, botLeft, botRight);
    }
}

void Sphere::makeSphere() {
    float step = 2 * M_PI / m_param2;
    for (float theta = 0; theta < 2 * M_PI; theta += step) {
        makeWedge(theta, theta + step);
    }
}

void Sphere::setVertexData() {
     makeSphere();
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Sphere::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

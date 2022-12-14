#include "Cone.h"

void Cone::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}

float Cone::getRadius(float height) {
    return (0.5 - height) / 2.f;
}

glm::vec3 Cone::getBodyNormal(glm::vec3 point) {
    return glm::normalize(glm::vec3(2 * point[0], (-point[1] / 2) + (0.25), 2 * point[2]));
}

void Cone::makeTile(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight) {

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, getBodyNormal(topLeft));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, getBodyNormal(bottomLeft));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, getBodyNormal(bottomRight));

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, getBodyNormal(bottomRight));
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, getBodyNormal(topRight));
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, getBodyNormal(topLeft));
}

void Cone::makeTip(glm::vec3 top, glm::vec3 bottomLeft, glm::vec3 bottomRight) {
    glm::vec3 tipNormal = (bottomLeft + bottomRight / 2.f);
    tipNormal[1] = 0.f;
    tipNormal = normalize(tipNormal);
    tipNormal[1] = 0.5f;
    tipNormal = glm::normalize(tipNormal);

    insertVec3(m_vertexData, top);
    insertVec3(m_vertexData, tipNormal);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, getBodyNormal(bottomLeft));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, getBodyNormal(bottomRight));
}

void Cone::makeWedge(float currentTheta, float nextTheta) {
    float step = 1.f / m_param1;
    for (int i = 0; i < m_param1; i++) {
        float height = -0.5 + i * step;
        float r1 = getRadius(height);
        float r2 = getRadius(height + step);

        glm::vec3 topLeft(r2 * sin(currentTheta), height + step, r2 * cos(currentTheta));
        glm::vec3 topRight(r2 * sin(nextTheta), height + step, r2 * cos(nextTheta));
        glm::vec3 botLeft(r1 * sin(currentTheta), height, r1 * cos(currentTheta));
        glm::vec3 botRight(r1 * sin(nextTheta), height, r1 * cos(nextTheta));

        if (i == m_param1 - 1) {
            makeTip(topLeft, botLeft, botRight);
        } else {
            makeTile(topLeft, topRight, botLeft, botRight);
        }

        // Base
        if (i == 0) {
            insertVec3(m_vertexData, glm::vec3(0, height, 0));
            insertVec3(m_vertexData, glm::vec3(0.f, -1.f, 0.f));
            insertVec3(m_vertexData, botRight);
            insertVec3(m_vertexData, glm::vec3(0.f, -1.f, 0.f));
            insertVec3(m_vertexData, botLeft);
            insertVec3(m_vertexData, glm::vec3(0.f, -1.f, 0.f));
        }
    }
}

void Cone::makeBody() {
    float step = 2 * M_PI / m_param2;
    for (float theta = 0; theta < 2 * M_PI; theta += step) {
        makeWedge(theta, theta + step);
    }
}


void Cone::setVertexData() {
    makeBody();
}


// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cone::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

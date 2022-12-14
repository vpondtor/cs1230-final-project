#pragma once

#include <vector>
#include <glm/glm.hpp>

class Cone
{
public:
    void updateParams(int param1, int param2);
    std::vector<float> generateShape() { return m_vertexData; }

private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVertexData();

    float getRadius(float height);
    glm::vec3 getBodyNormal(glm::vec3 point);
    void makeTile(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight);
    void makeTip(glm::vec3 top, glm::vec3 bottomLeft, glm::vec3 bottomRight);
    void makeWedge(float currentTheta, float nextTheta);
    void makeBody();

    std::vector<float> m_vertexData;
    int m_param1;
    int m_param2;
    float m_radius = 0.5;
};

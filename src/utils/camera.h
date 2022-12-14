#pragma once
#include "utils/scenedata.h"
#include <glm/glm.hpp>

class Camera {
public:
    Camera();
    void initializeCamera(SceneCameraData data, float width, float height);

    glm::mat4 view;
    glm::mat4 proj;
    glm::vec4 pos;
    glm::vec4 look;
    glm::vec4 up;

    glm::mat4 getViewMatrix();
    void updateViewMatrix();
    glm::mat4 getProjMatrix();
    glm::vec4 getPosition();
    void rotateCamera(float theta, glm::vec3 axis);

    void setProjMatrix(float near, float far);
    void setAspectRatio(float aspectRatio);
    void setNearAndFar(float near, float far);

    float getAspectRatio();
    float getHeightAngle();
    float getWidthAngle();

private:
    float m_aspectRatio;
    float m_heightAngle;
    float m_widthAngle;

    float m_near;
    float m_far;
};


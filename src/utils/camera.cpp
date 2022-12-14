#include "camera.h"
#include "glm/gtx/transform.hpp"
#include "settings.h"

glm::mat4 createViewMatrix(glm::vec4 pos, glm::vec4 look, glm::vec4 up) {
    glm::mat4 T = glm::translate(-1.f * glm::vec3(pos));

    glm::vec3 w = -glm::vec3(look);
    w = glm::normalize(w);
    glm::vec3 v = glm::vec3(up) - glm::dot(glm::vec3(up), w)*w;
    v = glm::normalize(v);
    glm::vec3 u = glm::cross(v, w);

    glm::vec4 col1 = glm::vec4(u[0], v[0], w[0], 0.0);
    glm::vec4 col2 = glm::vec4(u[1], v[1], w[1], 0.0);
    glm::vec4 col3 = glm::vec4(u[2], v[2], w[2], 0.0);
    glm::vec4 col4 = glm::vec4(0.0, 0.0, 0.0, 1.0);
    glm::mat4 R = glm::mat4(col1, col2, col3, col4);
    return R * T;
}

void Camera::setProjMatrix(float near, float far) {
    float c = -near / far;

    // Scaling
    glm::vec4 m1_col1 = glm::vec4(1.f / (far * glm::tan(m_widthAngle / 2.f)), 0.f, 0.f, 0.f);
    glm::vec4 m1_col2 = glm::vec4(0.f, 1.f / (far * glm::tan(m_heightAngle / 2.f)), 0.f, 0.f);
    glm::vec4 m1_col3 = glm::vec4(0.f, 0.f, 1.f / far, 0.f);
    glm::vec4 m1_col4 = glm::vec4(0.f, 0.f, 0.f, 1.f);
    glm::mat4 m1 = glm::mat4(m1_col1, m1_col2, m1_col3, m1_col4);

    // Unhinging
    glm::vec4 m2_col1 = glm::vec4(1.f, 0.f, 0.f, 0.f);
    glm::vec4 m2_col2 = glm::vec4(0.f, 1.f, 0.f, 0.f);
    glm::vec4 m2_col3 = glm::vec4(0.f, 0.f, 1.f / (1.f + c), -1.f);
    glm::vec4 m2_col4 = glm::vec4(0.f, 0.f, -c / (1.f + c), 0.f);
    glm::mat4 m2 = glm::mat4(m2_col1, m2_col2, m2_col3, m2_col4);

    // OpenGL Remapping
    glm::vec4 m3_col1 = glm::vec4(1.f, 0.f, 0.f, 0.f);
    glm::vec4 m3_col2 = glm::vec4(0.f, 1.f, 0.f, 0.f);
    glm::vec4 m3_col3 = glm::vec4(0.f, 0.f, -2.f, 0.f);
    glm::vec4 m3_col4 = glm::vec4(0.f, 0.f, -1.f, 1.f);
    glm::mat4 m3 = glm::mat4(m3_col1, m3_col2, m3_col3, m3_col4);

    proj = m3 * m2 * m1;
}

void Camera::setAspectRatio(float aspectRatio) {
    m_aspectRatio = aspectRatio;
}

void Camera::setNearAndFar(float near, float far) {
    m_near = near;
    m_far = far;

    setProjMatrix(m_near, m_far);
}

Camera::Camera() {
    view = glm::mat4(0.f);
    proj = glm::mat4(0.f);
    pos = glm::vec4(0.f);

    m_aspectRatio = 1.f;
    m_heightAngle = 1.f;
    m_widthAngle = 1.f;
}

void Camera::initializeCamera(SceneCameraData data, float width, float height) {
    pos = data.pos;
    look = data.look;
    up = data.up;

    updateViewMatrix();
    proj = glm::mat4(0.f);

    m_aspectRatio = width / height;
    m_heightAngle = data.heightAngle;
    m_widthAngle = m_aspectRatio * m_heightAngle;

    setNearAndFar(settings.nearPlane, settings.farPlane);
}

void Camera::updateViewMatrix() {
    glm::mat4 T = glm::translate(-1.f * glm::vec3(pos));

    glm::vec3 w = -glm::vec3(look);
    w = glm::normalize(w);
    glm::vec3 v = glm::vec3(up) - glm::dot(glm::vec3(up), w)*w;
    v = glm::normalize(v);
    glm::vec3 u = glm::cross(v, w);

    glm::vec4 col1 = glm::vec4(u[0], v[0], w[0], 0.0);
    glm::vec4 col2 = glm::vec4(u[1], v[1], w[1], 0.0);
    glm::vec4 col3 = glm::vec4(u[2], v[2], w[2], 0.0);
    glm::vec4 col4 = glm::vec4(0.0, 0.0, 0.0, 1.0);
    glm::mat4 R = glm::mat4(col1, col2, col3, col4);
    view = R * T;
}

void Camera::rotateCamera(float theta, glm::vec3 axis) {
    glm::vec4 col1(
        glm::cos(theta) + pow(axis[0], 2) * (1 - glm::cos(theta)),
        axis[0] * axis[1] * (1 - glm::cos(theta)) + axis[2] * glm::sin(theta),
        axis[0] * axis[2] * (1 - glm::cos(theta)) - axis[1] * glm::sin(theta),
        0.f);

    glm::vec4 col2(
        axis[0] * axis[1] * (1 - glm::cos(theta)) - axis[2] * glm::sin(theta),
        glm::cos(theta) + pow(axis[1], 2) * (1 - glm::cos(theta)),
        axis[1] * axis[2] * (1 - glm::cos(theta)) + axis[0] * glm::sin(theta),
        0.f);

    glm::vec4 col3(
        axis[0] * axis[2] * (1 - glm::cos(theta)) + axis[1] * glm::sin(theta),
        axis[1] * axis[2] * (1 - glm::cos(theta)) - axis[0] * glm::sin(theta),
        glm::cos(theta) + pow(axis[2], 2) * (1 - glm::cos(theta)),
        0.f);

    glm::vec4 col4(0.f, 0.f, 0.f, 1.f);

    glm::mat4 rotation(col1, col2, col3, col4);

    pos = rotation * pos;
    look = rotation * look;

    updateViewMatrix();
}

glm::mat4 Camera::getViewMatrix() {
    return view;
}

glm::mat4 Camera::getProjMatrix() {
    return proj;
}

glm::vec4 Camera::getPosition() {
    return pos;
}

float Camera::getAspectRatio() {
    return m_aspectRatio;
}

float Camera::getHeightAngle() {
    return m_heightAngle;
}

float Camera::getWidthAngle() {
    return m_widthAngle;
}

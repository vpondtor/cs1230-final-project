#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#include "shapes/VertexGenerator.h"
#include "utils/camera.h"
#include "utils/sceneparser.h"
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    bool m_initialized = false;

    GLuint m_shader;
    GLuint m_textureShader;
    VertexGenerator m_generator;
    RenderData m_metaData;
    bool m_sceneLoaded = false;
    Camera m_camera;

    // VBOs and VAOs
    void initializeVboMap();
    void updateVboMap();
    void initializeVaoMap();

    std::map<PrimitiveType, GLuint> m_vbos;
    std::map<PrimitiveType, GLuint> m_vaos;

    std::vector<PrimitiveType> m_primitives = {
        PrimitiveType::PRIMITIVE_CONE,
        PrimitiveType::PRIMITIVE_CUBE,
        PrimitiveType::PRIMITIVE_CYLINDER,
        PrimitiveType::PRIMITIVE_SPHERE,
        PrimitiveType::PRIMITIVE_MARCHINGCUBE
    };

    // FBO related
    void makeFBO();
    void initializeFullscreenQuad();
    void paintTexture(GLuint texture, bool perPixel, bool kernel);

    GLuint m_defaultFBO;
    GLuint m_fbo;
    GLuint m_fbo_texture;
    GLuint m_fbo_renderbuffer;

    GLuint m_fullscreen_vbo;
    GLuint m_fullscreen_vao;

    int m_fbo_width;
    int m_fbo_height;

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    int m_devicePixelRatio;
};

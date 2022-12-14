#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "utils/shaderloader.h"

// ================== Project 5: Lights, Camera

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here

    if (m_initialized) {
        for (PrimitiveType primitive : m_primitives) {
            glDeleteBuffers(1, &m_vbos[primitive]);
            glDeleteVertexArrays(1, &m_vaos[primitive]);
        }

        glDeleteProgram(m_shader);
    }

    // TODO: Clean up any FBO related objects
    // the fbo, texture, renderbuffer
    // the fullscreen quadrant

    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);

    glDeleteBuffers(1, &m_fullscreen_vbo);
    glDeleteVertexArrays(1, &m_fullscreen_vao);

    this->doneCurrent();
}

// Maps all primitives to an ID for its VBO
void Realtime::initializeVboMap() {
    for (PrimitiveType primitive : m_primitives) {
        m_vbos[primitive] = 0;
        glGenBuffers(1, &m_vbos[primitive]);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[primitive]);

        std::vector<float> vertices = m_generator.generateShape(primitive);

        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

// Assumes initializeVboMap has been called, updates the data in the VBOs
void Realtime::updateVboMap() {
    for (PrimitiveType primitive : m_primitives) {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[primitive]);

        std::vector<float> vertices = m_generator.generateShape(primitive);

        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    };
}

// Maps each primitive to an ID for its VAO
void Realtime::initializeVaoMap() {
    for (PrimitiveType primitive : m_primitives) {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[primitive]);

        m_vaos[primitive] = 0;
        glGenVertexArrays(1, &m_vaos[primitive]);
        glBindVertexArray(m_vaos[primitive]);

        // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(GLfloat) * 6, reinterpret_cast<void*>(0));

        // Surface Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(GLfloat) * 6, reinterpret_cast<void*>(3 * sizeof(GLfloat)));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void Realtime::initializeFullscreenQuad() {
    std::vector<GLfloat> fullscreen_quad_data =
        {
         -1.f,  1.f, 0.0f,
         0.f, 1.f,
         -1.f, -1.f, 0.0f,
         0.f, 0.f,
         1.f, -1.f, 0.0f,
         1.f, 0.f,
         1.f,  1.f, 0.0f,
         1.f, 1.f,
         -1.f,  1.f, 0.0f,
         0.f, 1.f,
         1.f, -1.f, 0.0f,
         1.f, 0.f,
         };

    // Create VBO
    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);

    // Create VAO
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);

    // Enable attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Realtime::makeFBO() {
    // Texture Attachment
    glGenTextures(1, &m_fbo_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_fbo_width, m_fbo_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Renderbuffer Attachment
    glGenRenderbuffers(1, &m_fbo_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_fbo_width, m_fbo_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Creating framebuffer
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Adding Attachments
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);

    // Unbinding
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");
    m_textureShader = ShaderLoader::createShaderProgram(":/resources/shaders/texture.vert", ":/resources/shaders/texture.frag");

    m_generator = VertexGenerator();
    m_generator.updateParameters(settings.shapeParameter1, settings.shapeParameter2);

    initializeVboMap();
    initializeVaoMap();
    initializeFullscreenQuad();

    m_defaultFBO = 2;

    m_fbo_width = size().width() * m_devicePixelRatio;
    m_fbo_height = size().height() * m_devicePixelRatio;

    glUseProgram(m_textureShader);
    GLuint location = glGetUniformLocation(m_textureShader, "sampler");
    glUniform1i(location, 0);

    location = glGetUniformLocation(m_textureShader, "step_u");
    glUniform1f(location, 1.f / m_fbo_width);

    location = glGetUniformLocation(m_textureShader, "step_v");
    glUniform1f(location, 1.f / m_fbo_height);

    glm::vec3 col1(-1.f / 9.f, -1.f / 9.f, -1.f / 9.f);
    glm::vec3 col2(-1.f / 9.f, 17.f / 9.f, -1.f / 9.f);
    glm::vec3 col3(-1.f / 9.f, -1.f / 9.f, -1.f / 9.f);
    glm::mat3x3 sharpenKernel = glm::mat3x3(col1, col2, col3);

    location = glGetUniformLocation(m_textureShader, "sharpenKernel");
    glUniformMatrix3fv(location, 1, false, &sharpenKernel[0][0]);

    glUseProgram(0);

    makeFBO();
    m_initialized = true;
}

void Realtime::paintGL() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_shader);

    // Students: anything requiring OpenGL calls every frame should be done here
    SceneGlobalData &globalData = m_metaData.globalData;

    for (RenderShapeData &shape : m_metaData.shapes) {
        SceneMaterial &material = shape.primitive.material;

        std::vector<float> shapeData = m_generator.generateShape(shape.primitive.type);

        GLuint shape_vbo = m_vbos[shape.primitive.type];
        GLuint shape_vao = m_vaos[shape.primitive.type];

        glBindBuffer(GL_ARRAY_BUFFER, shape_vbo);
        glBindVertexArray(shape_vao);

        // Model
        GLuint location = glGetUniformLocation(m_shader, "model");
        glUniformMatrix4fv(location, 1, false, &shape.ctm[0][0]);

        // View
        location = glGetUniformLocation(m_shader, "view");
        glUniformMatrix4fv(location, 1, false, &m_camera.view[0][0]);

        location = glGetUniformLocation(m_shader, "terrain");
        glUniform1i(location, settings.extraCredit4);

        // Projection
        location = glGetUniformLocation(m_shader, "projection");
        glUniformMatrix4fv(location, 1, false, &m_camera.proj[0][0]);

        location = glGetUniformLocation(m_shader, "normalTransform");
        glUniformMatrix3fv(location, 1, false, &shape.normalTransform[0][0]);

        // Global Data
        location = glGetUniformLocation(m_shader, "globalData.ka");
        glUniform1f(location, globalData.ka);

        location = glGetUniformLocation(m_shader, "globalData.kd");
        glUniform1f(location, globalData.kd);

        location = glGetUniformLocation(m_shader, "globalData.ks");
        glUniform1f(location, globalData.ks);

        // Material
        location = glGetUniformLocation(m_shader, "material.cAmbient");
        glUniform4fv(location, 1, &material.cAmbient[0]);

        location = glGetUniformLocation(m_shader, "material.cDiffuse");
        glUniform4fv(location, 1, &material.cDiffuse[0]);

        location = glGetUniformLocation(m_shader, "material.cSpecular");
        glUniform4fv(location, 1, &material.cSpecular[0]);

        location = glGetUniformLocation(m_shader, "material.shininess");
        glUniform1f(location, material.shininess);

        // Lights
        location = glGetUniformLocation(m_shader, "numLights");
        glUniform1i(location, GLuint(fmin(m_metaData.lights.size(), 8)));

        for (int i = 0; i < m_metaData.lights.size() && i < 8; i++) {
            location = glGetUniformLocation(m_shader, ("lights[" + std::to_string(i) + "].type").c_str());
            glUniform1i(location, int(m_metaData.lights[i].type));

            location = glGetUniformLocation(m_shader, ("lights[" + std::to_string(i) + "].color").c_str());
            glUniform4fv(location, 1, &m_metaData.lights[i].color[0]);

            location = glGetUniformLocation(m_shader, ("lights[" + std::to_string(i) + "].function").c_str());
            glUniform3fv(location, 1, &m_metaData.lights[i].function[0]);

            if (m_metaData.lights[i].type == LightType::LIGHT_POINT) {
                location = glGetUniformLocation(m_shader, ("lights[" + std::to_string(i) + "].pos").c_str());
                glUniform4fv(location, 1, &m_metaData.lights[i].pos[0]);
            } else if (m_metaData.lights[i].type == LightType::LIGHT_DIRECTIONAL) {
                location = glGetUniformLocation(m_shader, ("lights[" + std::to_string(i) + "].direction").c_str());
                glUniform4fv(location, 1, &m_metaData.lights[i].dir[0]);
            } else if (m_metaData.lights[i].type == LightType::LIGHT_SPOT) {
                location = glGetUniformLocation(m_shader, ("lights[" + std::to_string(i) + "].pos").c_str());
                glUniform4fv(location, 1, &m_metaData.lights[i].pos[0]);

                location = glGetUniformLocation(m_shader, ("lights[" + std::to_string(i) + "].direction").c_str());
                glUniform4fv(location, 1, &m_metaData.lights[i].dir[0]);

                location = glGetUniformLocation(m_shader, ("lights[" + std::to_string(i) + "].penumbra").c_str());
                glUniform1f(location, m_metaData.lights[i].penumbra);

                location = glGetUniformLocation(m_shader, ("lights[" + std::to_string(i) + "].angle").c_str());
                glUniform1f(location, m_metaData.lights[i].angle);
            }
        }

        // Camera
        location = glGetUniformLocation(m_shader, "camera.position");
        glUniform4fv(location, 1, &m_camera.pos[0]);

        // Draw
        glDrawArrays(GL_TRIANGLES, 0, shapeData.size() / 6);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintTexture(m_fbo_texture, settings.perPixelFilter, settings.kernelBasedFilter);

    glUseProgram(0);
}

void Realtime::paintTexture(GLuint texture, bool perPixel, bool kernel) {
    glUseProgram(m_textureShader);

    GLuint location = glGetUniformLocation(m_textureShader, "perPixel");
    glUniform1i(location, perPixel);

    location = glGetUniformLocation(m_textureShader, "kernel");
    glUniform1i(location, kernel);

    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBindVertexArray(m_fullscreen_vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

void Realtime::resizeGL(int w, int h) {
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);

    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    m_fbo_width = size().width() * m_devicePixelRatio;
    m_fbo_height = size().height() * m_devicePixelRatio;

    makeFBO();

    GLuint location = glGetUniformLocation(m_textureShader, "step_u");
    glUniform1f(location, 1.f / m_fbo_width);

    location = glGetUniformLocation(m_textureShader, "step_v");
    glUniform1f(location, 1.f / m_fbo_height);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    if (m_sceneLoaded) {
        m_camera.initializeCamera(m_metaData.cameraData, w, h);
    }
}

void Realtime::sceneChanged() {
    m_metaData = RenderData();
    m_sceneLoaded = SceneParser::parse(settings.sceneFilePath, m_metaData);

    if (m_sceneLoaded) {
        m_camera = Camera();
        m_camera.initializeCamera(m_metaData.cameraData, size().width(), size().height());
    }

    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
    m_generator.updateParameters(settings.shapeParameter1, settings.shapeParameter2);

    if (m_initialized) {
        updateVboMap();
    }

    if (m_sceneLoaded) {
       m_camera.setNearAndFar(settings.nearPlane, settings.farPlane);
    }

    update(); // asks for a PaintGL() call to occur
}

// ================== Project 6: Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        float angleX = M_PI * float(deltaX) / float(m_fbo_width);
        float angleY = M_PI * float(deltaY) / float(m_fbo_height);

        m_camera.rotateCamera(angleX, glm::vec3(0.f, 1.f, 0.f));


        glm::vec3 axis = glm::normalize(glm::cross(glm::vec3(m_camera.look), glm::vec3(m_camera.up)));
        m_camera.rotateCamera(angleY, axis);

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    float rotateSpeed = 0.004;

    m_elapsedTimer.restart();

    bool changeFlag = false;
    glm::vec4 direction(0.f);
    float SCALE = 5;

    if (m_keyMap[Qt::Key::Key_W]) {
        direction = m_camera.look;
        changeFlag = true;
    } else if (m_keyMap[Qt::Key::Key_A]) {
        direction = glm::vec4(-glm::cross(glm::vec3(m_camera.look), glm::vec3(m_camera.up)), 0.f);
        changeFlag = true;
    } else if (m_keyMap[Qt::Key::Key_S]) {
        direction = -m_camera.look;
        changeFlag = true;
    } else if (m_keyMap[Qt::Key::Key_D]) {
        direction = glm::vec4(glm::cross(glm::vec3(m_camera.look), glm::vec3(m_camera.up)), 0.f);
        changeFlag = true;
    } else if (m_keyMap[Qt::Key::Key_Space]) {
        direction = glm::vec4(0.f, 1.f, 0.f, 0.f);
        changeFlag = true;
    } else if (m_keyMap[Qt::Key::Key_Control]) {
        direction = glm::vec4(0.f, -1.f, 0.f, 0.f);
        changeFlag = true;
    } else if (settings.extraCredit3) {
        float angleX = M_PI * rotateSpeed;
        m_camera.rotateCamera(angleX, glm::vec3(0.f, 1.f, 0.f));
    }

    if (m_keyMap[Qt::Key::Key_Q]) {
        //FIXME: This is scuffed
        MarchingCube &marchingCube = m_generator.m_marchingCube;
        marchingCube.updateThreshold(marchingCube.m_threshold - marchingCube.m_thresholdStep);
    } else if (m_keyMap[Qt::Key::Key_E]) {
        //FIXME: This is scuffed
        MarchingCube &marchingCube = m_generator.m_marchingCube;
        marchingCube.updateThreshold(marchingCube.m_threshold + marchingCube.m_thresholdStep);
    }

    if (m_initialized) {
        updateVboMap();
    }

    if (changeFlag) {
        m_camera.pos += SCALE * glm::normalize(direction) * deltaTime;
        m_camera.updateViewMatrix();
    }

    update(); // asks for a PaintGL() call to occur
}

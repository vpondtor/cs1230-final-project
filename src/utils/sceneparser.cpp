#include "sceneparser.h"
#include "scenefilereader.h"
#include "glm/gtx/transform.hpp"

#include <chrono>
#include <memory>
#include <iostream>

glm::mat3 createNormalTransform(glm::mat4 ctm) {
    glm::mat3 m(ctm);
    m = glm::transpose(m);
    m = glm::inverse(m);
    return m;
}

void populateShapes(std::vector<RenderShapeData> &shapes, SceneNode *node, glm::mat4 ctm) {
    glm::mat4 current = ctm;
    for (SceneTransformation *transformation : node->transformations) {
        glm::mat4 m;
        switch (transformation->type) {
        case TransformationType::TRANSFORMATION_TRANSLATE:
            m = glm::translate(transformation->translate);
            break;
        case TransformationType::TRANSFORMATION_ROTATE:
            m = glm::rotate(transformation->angle, transformation->rotate);
            break;
        case TransformationType::TRANSFORMATION_SCALE:
            m = glm::scale(transformation->scale);
            break;
        default:
            m = transformation->matrix;
        }
        current = current * m;
    }

    for (ScenePrimitive *primitive : node->primitives) {
        shapes.push_back(RenderShapeData{*primitive, current, createNormalTransform(current)});
    }

    for (SceneNode *child : node->children) {
        populateShapes(shapes, child, current);
    }
}

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readXML();
    if (!success) {
        return false;
    }

    renderData.globalData = fileReader.getGlobalData();
    renderData.lights = fileReader.getLights();
    renderData.cameraData = fileReader.getCameraData();
    renderData.shapes.clear();

    SceneNode *root = fileReader.getRootNode();
    populateShapes(renderData.shapes, root, glm::mat4{1.0f});

    return true;
}

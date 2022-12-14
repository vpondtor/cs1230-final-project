#pragma once
#include "utils/scenedata.h"
#include <vector>
class uniforms
{
public:
    void passUniforms();

private:
    std::vector<SceneLightData> m_lights;
    SceneGlobalData m_globalData;
};

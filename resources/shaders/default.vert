#version 330 core

layout(location = 0) in vec3 objectSpacePosition;
layout(location = 1) in vec3 objectSpaceNormal;

out vec3 worldSpacePosition;
out vec3 worldSpaceNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat3 normalTransform;

void main() {
    worldSpacePosition = vec3(model * vec4(objectSpacePosition, 1));
    worldSpaceNormal = normalTransform * normalize(objectSpaceNormal);

    mat4 mvp = projection * view * model;
    gl_Position = mvp * vec4(objectSpacePosition, 1.0);
}

#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uvVert;

out vec2 uvFrag;

void main()
{
    uvFrag = uvVert;
    gl_Position = vec4(position, 1.f);
}

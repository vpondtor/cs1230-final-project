#version 330 core

in vec2 uvFrag;
out vec4 color;

uniform sampler2D sampler;
uniform bool perPixel;
uniform bool kernel;

uniform float step_u;
uniform float step_v;

uniform mat3 sharpenKernel;

void main(void)
{
    color = vec4(1.f);
    color = texture(sampler, uvFrag);

    if (kernel) {
        vec4 acc = vec4(0.f, 0.f, 0.f, 1.f);
        for (int offset_v = -1; offset_v < 2; offset_v++) {
            for (int offset_u = -1; offset_u < 2; offset_u++) {
                vec4 currentColor = texture(sampler, vec2(uvFrag[0] + offset_u * step_u, uvFrag[1] + offset_v * step_v));
                acc += sharpenKernel[offset_u + 1][offset_v + 1] * currentColor;
            }
        }
        color = acc;
    }

    if (perPixel) {
        color = 1 - color;
    }
}

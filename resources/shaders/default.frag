#version 330 core

in vec3 worldSpacePosition;
in vec3 worldSpaceNormal;

out vec4 color;

// For the future, might need support for point and spot lights
struct GlobalData {
    float ka;
    float kd;
    float ks;
};

struct Material {
    vec4 cAmbient;
    vec4 cDiffuse;
    vec4 cSpecular;
    float shininess;
};

struct Light {
    int type;
    vec4 color;
    vec4 pos;
    vec4 direction;
    float penumbra;
    float angle;
    vec3 function;
};

struct Camera {
    vec4 position;
};

uniform GlobalData globalData;
uniform Material material;

uniform int numLights;
uniform Light lights[8];

uniform Camera camera;

uniform bool terrain;

float falloff(float x, float inner, float outer) {
    float base = (x - inner) / (outer - inner);
    return -2 * pow(base, 3) + 3 * pow(base, 2);
}

float angle(vec3 v1, vec3 v2) {
    return acos(dot(normalize(v1), normalize(v2)));
}

void main() {
    vec4 blue = vec4(0.17, 0.39, 0.93, 1);
    vec4 red = vec4(0.96, 0.41, 0.02, 1);
    vec4 brown = vec4(0.61, 0.46, 0.32, 1);
    vec4 green = vec4(0.28, 0.44, 0.22, 1);

    vec4 terrainColor;

    if (worldSpacePosition[1] < -0.25) {
        terrainColor = red;
    } else if (worldSpacePosition[1] < 0.5) {
        terrainColor = brown;
    } else {
        terrainColor = green;
    }

    color = vec4(0, 0, 0, 1);

    vec3 N = normalize(worldSpaceNormal);
    vec3 E = normalize(vec3(camera.position) - worldSpacePosition);

    // Ambient
    if (terrain) {
        color += globalData.ka * terrainColor;
    } else {
        color += globalData.ka * material.cAmbient;
    }

    for (int i = 0; i < numLights; i++) {
        float attenuation = 1.0;
        Light light = lights[i];
        vec4 lightIntensity = light.color;

        // Diffuse
        vec3 L = -normalize(vec3(light.direction));
        if (light.type == 0) {
            // Point
            float distance = distance(worldSpacePosition, vec3(light.pos));
            L = normalize(vec3(light.pos) - worldSpacePosition);
            attenuation = 1.0 / (light.function[0] + light.function[1] * distance + light.function[2] * pow(distance, 2));
        } else if (light.type == 1) {
            // Directional
            L = -normalize(vec3(light.direction));
        } else if (light.type == 2) {
            // Spot
            float distance = distance(worldSpacePosition, vec3(light.pos));
            L = normalize(vec3(light.pos) - worldSpacePosition);
            attenuation = 1.0 / (light.function[0] + light.function[1] * distance + light.function[2] * pow(distance, 2));

            float theta = angle(normalize(vec3(light.direction)), normalize(worldSpacePosition - vec3(light.pos)));
            if (theta > light.angle) {
                lightIntensity = vec4(0.0);
            } else {
                if (light.angle - light.penumbra < theta && theta <= light.angle) {
                    lightIntensity *= 1 - falloff(theta, light.angle - light.penumbra, light.angle);
                }
            }
        }

        vec3 R = normalize(reflect(-L, N));


        float diffuse = globalData.kd * clamp(dot(N, L), 0, 1);

        if (terrain) {
            color += attenuation * lightIntensity * diffuse * terrainColor;
        } else {
            color += attenuation * lightIntensity * diffuse * material.cDiffuse;
        }


        if (worldSpacePosition[1] < 0) {
        }

        // Specular
        float specular = clamp(dot(R, E), 0, 1);
        if (specular < 0 || (specular == 0 && material.shininess <= 0)) {
            specular = 0;
        } else {
            specular = pow(specular, material.shininess);
        }

        specular = globalData.ks * specular;
        color += attenuation * lightIntensity * specular * material.cSpecular;
    }
}

#version 330 core
layout (location = 0) in uvec2 vertex;

uniform mat4 view;
uniform mat4 projection;

uniform uvec3 chunkPos;
uniform uint daylight;

out vec2 texCoord;
out vec4 light;

void main(void)
{
    float lightValue = float((vertex.y >> 18u) & 15u) / 15.0;    

    if ((lightValue == 0) || ((daylight / 15.0) > lightValue)) {
        lightValue = daylight / 15.0;
    }

    light = vec4(lightValue, lightValue, lightValue, 1.0);
    float x = float((vertex.x  & 31u)             + chunkPos.x) - 0.5;
    float y = float(((vertex.x & 8160u)    >> 5u) + chunkPos.y) - 0.5;
    float z = float(((vertex.x & 253952u) >> 13u) + chunkPos.z) - 0.5;
    uint occlusion = (vertex.x & 786432u) >> 18u;
    uint highlight = (vertex.x & 1048576u) >> 20u;

    switch (occlusion) {
        case 0u:
            light *= vec4(1.0, 1.0, 1.0, 1.0);
            break;
        case 1u:
            light *= vec4(0.68, 0.68, 0.68, 1.0);
            break;
        case 2u:
            light *= vec4(0.55, 0.55, 0.55, 1.0);
            break;
        case 3u:
            light *= vec4(0.45, 0.45, 0.45, 1.0);
            break;
    }

    if (highlight == 1u) {
        light = vec4(0.5, 0.5, 0.5, 1.0);
    }

    gl_Position = projection * view * vec4(x, y, z, 1.0);

    texCoord.x = float(vertex.y & 255u) / 16;
    texCoord.y = float((vertex.y >> 8u) & 255u) / 16;

    float trans = (float((vertex.y >> 16u) & 3u) * 0.3);
    if (trans != 0) {
        light.a = trans;
    }
}

#version 330 core
layout (location = 0) in uvec2 vertex;

uniform mat4 view;
uniform mat4 projection;

uniform uvec3 chunkPos;

out vec2 texCoord;
out vec3 color;

void main(void)
{
    float x = float((vertex.x  & 31u)             + chunkPos.x) - 0.5;
    float y = float(((vertex.x & 8160u)    >> 5u)  + chunkPos.y) - 0.5;
    float z = float(((vertex.x & 253952u) >> 13u) + chunkPos.z) - 0.5;
    uint occlusion = (vertex.x & 786432u) >> 18u;
    switch (occlusion) {
        case 0u:
            color = vec3(1.0, 1.0, 1.0);
            break;
        case 1u:
            color = vec3(0.68, 0.68, 0.68);
            break;
        case 2u:
            color = vec3(0.55, 0.55, 0.55);
            break;
        case 3u:
            color = vec3(0.45, 0.45, 0.45);
            break;
    }
    gl_Position = projection * view * vec4(x, y, z, 1.0);

    texCoord.x = float(vertex.y & 255u) / 16;
    texCoord.y = float((vertex.y >> 8u) & 255u) / 16;
}

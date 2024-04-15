#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in float occlusion;

uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord;
out vec3 color;
out vec3 normal;
out vec3 fragPos;

void main(void)
{
    int occlusionInt = int(occlusion);
    switch (occlusionInt) {
        case 0: 
            color = vec3(1.0, 1.0, 1.0);
            break;
        case 1: 
            color = vec3(0.68, 0.68, 0.68);
            break;
        case 2: 
            color = vec3(0.55, 0.55, 0.55);
            break;
        case 3: 
            color = vec3(0.45, 0.45, 0.45);
            break;
    }
    normal = normalize(aNormal);
    gl_Position = projection * view * vec4(aPos, 1.0); 
    texCoord = aTexCoord / 16;
}

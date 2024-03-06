#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 scaling;
uniform mat4 view;
uniform mat4 projection;

out vec3 texCoord;

void main(void)
{
    gl_Position = projection * view * scaling * vec4(aPos, 1.0);
    texCoord = aPos;
}

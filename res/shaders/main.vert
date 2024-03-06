#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord;
out vec3 color;

void main(void)
{
    const vec3 light = vec3(0.7, 0.7, 0.7);
    float shade;
    shade = dot(normalize( vec3(0.5, 0.5, 0.0) ), light);
    shade = max(shade, 0.0);
    color = vec3(shade);

	gl_Position = projection * view  * model * vec4(aPos, 1.0); 
    texCoord = aTexCoord / 5;
}

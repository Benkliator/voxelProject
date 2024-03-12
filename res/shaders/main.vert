#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord;
out vec3 color;

void main(void)
{
    const vec3 light = normalize(vec3(0.7, -0.7, 0.7));
    float shade;
    shade = dot(normalize( aNormal ), light);
    shade = max(shade, 0.0);
    color = vec3(shade);

    gl_Position = projection * view  * model * vec4(aPos, 1.0); 
    texCoord = aTexCoord / 5;
}

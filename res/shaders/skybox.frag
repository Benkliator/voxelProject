#version 330 core

out vec4 fragColor;

in vec3 texCoord;

uniform samplerCube ourTexture;

void main(void)
{
    fragColor = texture(ourTexture, texCoord);
}

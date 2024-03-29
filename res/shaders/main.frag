#version 330 core

out vec4 fragColor;

in vec2 texCoord;
in vec3 color;
in vec3 normal;

uniform sampler2D ourTexture;

void main(void)
{
    fragColor = texture(ourTexture, texCoord) * vec4(color, 1.0);
}

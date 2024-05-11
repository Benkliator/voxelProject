#version 330 core

out vec4 fragColor;

in vec2 texCoord;
in vec3 color;

uniform sampler2D ourTexture;

void main(void)
{
    fragColor = texture(ourTexture, texCoord) * vec4(color, 1.0);
}

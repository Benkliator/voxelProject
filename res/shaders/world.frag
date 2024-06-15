#version 330 core

out vec4 fragColor;

in vec2 texCoord;
in vec4 color;

uniform sampler2D ourTexture;

void main(void)
{
    vec4 tex = texture(ourTexture, texCoord) * color;
    if (tex.a < 0.02) {
        discard;
    }
    fragColor = tex;
}

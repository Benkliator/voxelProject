#version 330 core

out vec4 fragColor;

in vec2 texCoord;
in vec4 light;

uniform sampler2D frag_tex;

void main(void)
{
    vec4 tex = texture(frag_tex, texCoord) * light;
    if (tex.a < 0.02) {
        discard;
    } else if (tex.a > 0.9) {
        tex.a = 1;
    }
    fragColor = tex;
}

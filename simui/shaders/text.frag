#version 330 core

uniform vec4      uColor;
uniform sampler2D uTexture;

in vec2 aTexCoord;

out vec4 fragColor;

void main()
{
    fragColor = vec4(1.0, 1.0, 1.0, texture(uTexture, aTexCoord).a) * uColor;
};



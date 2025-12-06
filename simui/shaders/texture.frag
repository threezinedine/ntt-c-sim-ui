#version 330 core

uniform vec4      uColor;
uniform sampler2D uTexture;

in vec2 aTexCoord;

out vec4 fragColor;

void main()
{
    fragColor = texture(uTexture, aTexCoord) * uColor;

    if (aTexCoord.x == 0)
    {
        fragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
};


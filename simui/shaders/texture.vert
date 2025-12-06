#version 330 core

layout (location = 0) in vec2 vPos;
layout (location = 1) in vec2 vTexCoord;

uniform vec2 windowSize;

out vec2 aTexCoord;

void main()
{
    vec2 pos = vPos / windowSize - 1;
    gl_Position = vec4(pos, 0.0, 1.0);
    aTexCoord = vTexCoord;
}
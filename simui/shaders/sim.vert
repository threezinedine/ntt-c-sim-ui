#version 330 core
								  
layout (location = 0) in vec2 vPos;
								  
uniform vec2 uWindowSize;
								  
void main()
{
    vec2 pos = vPos / uWindowSize - 1;
    gl_Position = vec4(pos, 0.0, 1.0);
};

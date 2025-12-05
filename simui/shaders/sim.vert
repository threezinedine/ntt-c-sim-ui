#version 330 core
								  
layout (location = 0) in vec2 vPos;
								  
uniform vec2 vWindowSize;
								  
void main()
{
    vec2 pos = vPos / vWindowSize - 1;
    gl_Position = vec4(pos, 0.0, 1.0);
};

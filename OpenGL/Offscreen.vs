#version 400 core

layout (location = 0) in vec3 inputPosition;
layout (location = 2) in vec2 inputTexcoord;

out vec2 TexCoords;


void main()
{
    gl_Position = vec4(inputPosition.x, inputPosition.y, 0.0, 1.0); 
    TexCoords = inputTexcoord;
}  
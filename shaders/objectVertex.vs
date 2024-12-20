#version 330 core

layout (location = 0) in vec4 aPos;

uniform mat4 mvpMatrix;

void main()
{
	gl_Position = mvpMatrix * aPos;
}
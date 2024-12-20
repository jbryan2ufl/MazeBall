#version 330 core
layout (location = 0) in vec4 vertex;
out vec2 TexCoords;
uniform mat4 mvpMatrix;

void main() {
	gl_Position = mvpMatrix * vec4(vertex.xy, 1.0, 1.0);
	TexCoords = vertex.zw;
}
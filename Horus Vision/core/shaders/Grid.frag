#version 460 core

out vec4 FragColor;
uniform vec4 GridColor;

void main()
{
	FragColor = GridColor;
}
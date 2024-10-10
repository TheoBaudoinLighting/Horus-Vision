#version 460 core

out vec4 FragColor;
uniform vec4 GridColor = vec4(0.2, 0.2, 0.2, 1.0);

void main()
{
	FragColor = GridColor;
}
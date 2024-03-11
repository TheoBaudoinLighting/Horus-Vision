#version 460 core

layout (location = 0) in vec3 Pos;

uniform mat4 Model;
uniform mat4 CameraMatrix;

void main()
{
	gl_Position = CameraMatrix * Model * vec4(Pos, 1.0);
}
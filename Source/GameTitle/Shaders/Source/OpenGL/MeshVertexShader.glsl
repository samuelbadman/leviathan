#version 460 core

layout (location = 0) in vec3 Attrib_Position;

void main()
{
	gl_Position = vec4(Attrib_Position, 1.0f);
}
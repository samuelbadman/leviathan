#version 460 core

layout (location = 0) in vec3 Attrib_Position;
layout (location = 1) in vec4 Attrib_Color;

out vec4 VertexColor;

void main()
{
	VertexColor = Attrib_Color;
	gl_Position = vec4(Attrib_Position, 1.0f);
}
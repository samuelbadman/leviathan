#version 460 core

layout (location = 0) in vec3 Attrib_Position;
layout (location = 1) in vec4 Attrib_Color;

out VS_OUT
{
	vec4 VertexColor;
} VS_Out;

void main()
{
	VS_Out.VertexColor = Attrib_Color;
	gl_Position = vec4(Attrib_Position, 1.0f);
}
#version 460 core

layout (location = 0) in vec3 Attrib_Position;
layout (location = 1) in vec4 Attrib_Color;

layout (std140, binding = 0) uniform ShaderConstants
{
	vec3 VertexPositionOffset;
};

out VS_OUT
{
	vec4 VertexColor;
} VS_Out;

void main()
{
	VS_Out.VertexColor = Attrib_Color;
	gl_Position = vec4(
		Attrib_Position.x + VertexPositionOffset.x, 
		Attrib_Position.y + VertexPositionOffset.y, 
		Attrib_Position.z + VertexPositionOffset.z,
		1.0f
	);
}
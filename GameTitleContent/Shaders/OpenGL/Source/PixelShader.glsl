#version 460 core

in VS_OUT
{
	vec4 VertexColor;
	vec2 TexCoord0;
} FS_In;

out vec4 FragmentColor;

void main()
{
	FragmentColor = FS_In.VertexColor;
}
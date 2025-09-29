#version 460 core

in vec4 VertexColor;

out vec4 FragmentColor;

void main()
{
	FragmentColor = VertexColor;
}
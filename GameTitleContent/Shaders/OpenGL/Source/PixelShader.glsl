#version 460 core

#extension GL_ARB_bindless_texture : require

layout (std140, binding = 1) uniform PixelShaderConstants
{
	vec4 Color;
};

in VS_OUT
{
	vec4 VertexColor0;
	vec2 TexCoord0;
} FS_In;

out vec4 FragmentColor;

void main()
{
	//FragmentColor = FS_In.VertexColor0;
	FragmentColor = Color;
}
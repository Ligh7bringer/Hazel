#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Colour;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;
layout(location = 5) in int a_EntityID;

uniform mat4 u_ViewProjection;

out vec2 v_TexCoord;
out vec4 v_Colour;
out float v_TexIndex;
out float v_TilingFactor;
out flat int v_EntityID;

void main()
{
	v_Colour = a_Colour;
	v_TexCoord = a_TexCoord;
	v_TexIndex = a_TexIndex;
	v_TilingFactor = a_TilingFactor;
	v_EntityID = a_EntityID;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450

#define MAX_TEXTURES 32

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

in vec4 v_Colour;
in vec2 v_TexCoord;
in float v_TexIndex;
in float v_TilingFactor;
in flat int v_EntityID;

uniform sampler2D u_Textures[MAX_TEXTURES];

void main()
{

#define TEXTURE(IDX)                                                                               \
	if(v_TexIndex == IDX)                                                                          \
	{                                                                                              \
		color = texture(u_Textures[int(IDX)], v_TexCoord * v_TilingFactor) * v_Colour;             \
	}

	// TODO: Add tiling factor
	TEXTURE(0)
	TEXTURE(1)
	TEXTURE(2)
	TEXTURE(3)
	TEXTURE(4)
	TEXTURE(5)
	TEXTURE(6)
	TEXTURE(7)
	TEXTURE(8)
	TEXTURE(9)
	TEXTURE(10)
	TEXTURE(11)
	TEXTURE(12)
	TEXTURE(13)
	TEXTURE(14)
	TEXTURE(15)
	TEXTURE(16)
	TEXTURE(17)
	TEXTURE(18)
	TEXTURE(19)
	TEXTURE(20)
	TEXTURE(21)
	TEXTURE(22)
	TEXTURE(23)
	TEXTURE(24)
	TEXTURE(25)
	TEXTURE(26)
	TEXTURE(27)
	TEXTURE(28)
	TEXTURE(29)
	TEXTURE(30)
	TEXTURE(31)
#undef TEXTURE

	color2 = v_EntityID;
}
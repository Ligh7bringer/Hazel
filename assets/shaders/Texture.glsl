#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Colour;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;

out vec2 v_TexCoord;
out vec4 v_Colour;

void main()
{
	v_Colour = a_Colour;
	v_TexCoord = a_TexCoord;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec4 v_Colour;
in vec2 v_TexCoord;

uniform sampler2D u_Texture;
uniform vec4 u_Colour;
uniform float u_TilingFactor;

void main()
{
	// color = texture(u_Texture, v_TexCoord * u_TilingFactor) * u_Colour;
	color = v_Colour;
}
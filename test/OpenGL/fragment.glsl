#version 330 core

out vec4 color;

in vec2 ftexcoord;

uniform sampler2D tex1;
uniform sampler2D tex2;

void main()
{
	color = mix(texture(tex1, ftexcoord), texture(tex2, ftexcoord), 0.2);
}


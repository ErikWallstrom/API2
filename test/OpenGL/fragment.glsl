#version 330 core

in vec3 outcolor;
out vec4 color;

void main()
{
	color = vec4(outcolor, 1.0);
}


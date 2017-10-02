#version 330 core

layout(location = 0) in vec2 vposition;
layout(location = 1) in vec3 vcolor;
layout(location = 2) in vec2 vtexcoord;

out vec3 fcolor;
out vec2 ftexcoord;

void main()
{
	gl_Position = vec4(vposition.x, vposition.y, 0.0, 1.0);
	fcolor = vcolor;
	ftexcoord = vtexcoord;
}


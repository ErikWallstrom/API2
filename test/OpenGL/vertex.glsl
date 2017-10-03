#version 330 core

layout(location = 0) in vec3 vposition;
layout(location = 1) in vec2 vtexcoord;

out vec2 ftexcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(vposition, 1.0);
	ftexcoord = vtexcoord;
}


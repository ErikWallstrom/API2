#version 330 core

layout(location = 0) in vec3 vposition;
layout(location = 1) in vec2 vtexcoord;

out vec2 ftexcoord;

uniform mat4 transform;

void main()
{
	gl_Position = transform * vec4(vposition, 1.0);
	ftexcoord = vtexcoord;
}


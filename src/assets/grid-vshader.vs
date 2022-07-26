#version 330 core

layout (location = 0) in vec3 posA;
layout (location = 1) in vec3 tempColor;

out vec3 newColor;
uniform float alpha;

void main()
{
	gl_Position = vec4(posA, 1.0);
	newColor = tempColor;
}

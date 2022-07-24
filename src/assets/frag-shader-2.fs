#version 330 core

out vec4 fragColor;

in vec3 newColor;

uniform float alpha;

void main()
{
	fragColor = vec4(newColor, alpha);
}

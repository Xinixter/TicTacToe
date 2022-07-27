#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 outColor;
out vec2 texCoord;

uniform float deltaX;
uniform float deltaY;

void main()
{
	gl_Position = vec4(aPos.x + 2 * (deltaX - 1) / 3, aPos.y + 2 * (1 - deltaY) / 3, 0.0, 1.0);
	outColor = aColor;
	texCoord = aTexCoord;
}

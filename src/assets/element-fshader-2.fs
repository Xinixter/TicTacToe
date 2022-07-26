#version 330 core

out vec4 fragColor;

in vec3 outColor;
in vec2 texCoord;

uniform sampler2D outTexture;

void main()
{
	fragColor = texture(outTexture, texCoord);
}

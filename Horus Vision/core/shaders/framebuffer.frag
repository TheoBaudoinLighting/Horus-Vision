#version 460 core

out vec4 FragColor;
in vec2 TexCoord0;

in vec2 ScreenTextureSize;

uniform sampler2D ScreenTexture;

void main()
{
	const float OffsetX = 1.0f / ScreenTextureSize.x;
	const float OffsetY = 1.0f / ScreenTextureSize.y;

	vec2 Offsets[9] = vec2[]
	(
		vec2(-OffsetX, OffsetY), 
		vec2(0.0f, OffsetY), 
		vec2(OffsetX, OffsetY), 
		vec2(-OffsetX, 0.0f),
		vec2(0.0f, 0.0f), 
		vec2(OffsetX, 0.0f),
		vec2(-OffsetX, -OffsetY), 
		vec2(0.0f, -OffsetY), 
		vec2(OffsetX, -OffsetY) 
	);

	float Kernel[9] = float[]
	(
		1.0f, 1.0f, 1.0f,
		1.0f, -8.0f, 1.0f,
		1.0f, 1.0f, 1.0f
	);

	vec3 color = vec3(0.0f);
	for(int i = 0; i < 9; i++)
	{
		color += vec3(texture(ScreenTexture, TexCoord0.st + Offsets[i])) * Kernel[i];
	}

	FragColor = vec4(color, 1.0f);
}


#version 330 core

uniform sampler2D uTexture;
uniform sampler2D uTextureB;
uniform sampler2D uTextureObstacle;

uniform float uDissipation;

layout(location = 0) out vec4 out_color;

void main()
{	
	vec2 texelSize = 1.0 / textureSize(uTexture, 0);
	const float dt = 0.01;
	vec2 fragCoord = gl_FragCoord.xy * texelSize;

	ivec2 t = ivec2(gl_FragCoord.xy);

	float solid = texelFetch(uTextureObstacle, t, 0).x;

	if (solid > 0.0)
	{
		out_color = vec4(0.0);
		return;
	}
	
	vec2 u = texture(uTexture, fragCoord).xy;
	vec2 coord = fragCoord - dt * u;
	out_color = vec4(uDissipation * texture(uTextureB, coord).xy, 0.0, 1.0);
}

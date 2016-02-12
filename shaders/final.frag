#version 330 core

uniform sampler2D uTexture;
layout(location = 0) out vec4 out_color;

void main()
{
	vec2 texelSize = 1.0 / textureSize(uTexture, 0);
	vec2 fragCoord = gl_FragCoord.xy * texelSize;
	ivec2 t = ivec2(gl_FragCoord.xy);

	out_color = vec4(1.0, 0.0, 1.0, texture(uTexture, fragCoord).x);
	//out_color = vec4(texture(uTexture, fragCoord).xyz, texture(uTexture, fragCoord).w);
	//out_color = vec4(0.56 * (texture(uTexture, fragCoord).xyz + vec3(1.0)), 1.0);

	vec2 wR = texelFetchOffset(uTexture, t, 0, ivec2(1, 0)).xy;
	vec2 wL = texelFetchOffset(uTexture, t, 0, ivec2(-1, 0)).xy;
	vec2 wT = texelFetchOffset(uTexture, t, 0, ivec2(0, 1)).xy;
	vec2 wB = texelFetchOffset(uTexture, t, 0, ivec2(0, -1)).xy;

	float r = 0.5 * (((wR.y - wL.y) / texelSize.x) - ((wT.x - wB.x) / texelSize.y));


//	out_color = vec4(0.5 * abs(r), 1.0 - exp(-0.02 * abs(r)), 1.0 / abs(r), 1.0);
//	out_color = vec4(0.0, 0.5 * (r + 1.0f), 0.0, 1.0);
	
	if (r > 0.0)
	{
//		out_color = vec4(0.0, 0.2 * r, 0.1 * r, 1.0);
	}
	else
	{
//		out_color = vec4(-0.2 * r, - 0.1 * r, 0.0, 1.0);
	}
}
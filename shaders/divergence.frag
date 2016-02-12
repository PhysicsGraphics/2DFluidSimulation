#version 330 core

uniform sampler2D uTexture;
uniform sampler2D uTextureObstacle;

layout(location = 0) out vec4 out_color;

void main()
{
	vec2 texelSize = 1.0 / textureSize(uTexture, 0);
	vec2 fragCoord = gl_FragCoord.xy * texelSize;
	ivec2 t = ivec2(gl_FragCoord.xy);

	vec2 wR = texelFetchOffset(uTexture, t, 0, ivec2(1, 0)).xy;
	vec2 wL = texelFetchOffset(uTexture, t, 0, ivec2(-1, 0)).xy;
	vec2 wT = texelFetchOffset(uTexture, t, 0, ivec2(0, 1)).xy;
	vec2 wB = texelFetchOffset(uTexture, t, 0, ivec2(0, -1)).xy;

	vec3 oL = texelFetchOffset(uTextureObstacle, t, 0, ivec2(-1, 0)).xyz;
	vec3 oR = texelFetchOffset(uTextureObstacle, t, 0, ivec2(1, 0)).xyz;
	vec3 oB = texelFetchOffset(uTextureObstacle, t, 0, ivec2(0, -1)).xyz;
	vec3 oT = texelFetchOffset(uTextureObstacle, t, 0, ivec2(0, 1)).xyz;

	if (oL.x > 0.0) wL = oL.yz;
	if (oR.x > 0.0) wR = oR.yz;
	if (oB.x > 0.0) wB = oB.yz;
	if (oT.x > 0.0) wT = oT.yz;

	float div = 0.5 * (((wR.x - wL.x) / texelSize.x) + ((wT.y - wB.y) / texelSize.y));
	out_color = vec4(div, 0.0, 0.0, 1.0);
}
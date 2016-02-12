#version 330 core

uniform sampler2D uTexture;
uniform sampler2D uTextureB;
uniform sampler2D uTextureObstacle;

layout(location = 0) out vec4 out_color;

void main()
{	
	vec2 texelSize = 1.0 / textureSize(uTexture, 0);
	float alpha = -texelSize.x * texelSize.x;
	float beta = 4.0;

	vec2 fragCoord = gl_FragCoord.xy * texelSize;
	ivec2 t = ivec2(gl_FragCoord.xy);

	float pR = texelFetchOffset(uTexture, t, 0, ivec2(1, 0)).x;
	float pL = texelFetchOffset(uTexture, t, 0, ivec2(-1, 0)).x;
	float pT = texelFetchOffset(uTexture, t, 0, ivec2(0, 1)).x;
	float pB = texelFetchOffset(uTexture, t, 0, ivec2(0, -1)).x;
	float pC = texelFetch(uTexture, t, 0).x;

	vec3 oL = texelFetchOffset(uTextureObstacle, t, 0, ivec2(-1, 0)).xyz;
	vec3 oR = texelFetchOffset(uTextureObstacle, t, 0, ivec2(1, 0)).xyz;
	vec3 oB = texelFetchOffset(uTextureObstacle, t, 0, ivec2(0, -1)).xyz;
	vec3 oT = texelFetchOffset(uTextureObstacle, t, 0, ivec2(0, 1)).xyz;

	if (oL.x > 0.0) pL = pC;
	if (oR.x > 0.0) pR = pC;
	if (oB.x > 0.0) pB = pC;
	if (oT.x > 0.0) pT = pC;

	float bC = texture(uTextureB, fragCoord).x;
	
	float res = (pT + pR + pB + pL + alpha * bC) / beta;
	out_color = vec4(res, 0.0, 0.0, 1.0);
}

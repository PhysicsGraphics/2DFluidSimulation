#version 330 core

uniform sampler2D uTexture;
uniform sampler2D uTextureB;
uniform sampler2D uTextureObstacle;

layout(location = 0) out vec4 out_color;

void main()
{	
	vec2 texelSize = 1.0 / textureSize(uTexture, 0);
	vec2 fragCoord = gl_FragCoord.xy * texelSize;

	vec3 oC = texture(uTextureObstacle, fragCoord).xyz;
	if (oC.x > 0.0)
	{
		out_color = vec4(oC.yz, 0.0, 1.0);
		return;
    }

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

	vec2 vObst = vec2(0.0);
	vec2 vMask = vec2(1.0);

	if (oL.x > 0.0)
	{
		pL = pC;
		vObst.x = oL.y;
		vMask.x = 0.0;
	}
	if (oR.x > 0.0) 
	{
		pR = pC;
		vObst.x = oR.y;
		vMask.x = 0.0;
	}
	if (oB.x > 0.0)
	{
		pB = pC;
		vObst.y = oB.z;
		vMask.y = 0.0;
	}
	if (oT.x > 0.0)
	{
		pT = pC;
		vObst.y = oT.z;
		vMask.y = 0.0;
	}

	vec2 uOld = texture(uTextureB, fragCoord).xy;	
	vec2 grad = 0.5 * vec2((pR - pL) / texelSize.x, (pT - pB) / texelSize.y);
	vec2 uNew = uOld - grad;
	out_color = vec4((vMask * uNew) + vObst, 0.0, 1.0);

//	vec2 uNew = texture(uTextureB, fragCoord).xy;	
//	vec2 res = uNew - 0.5 * vec2((pR - pL) / texelSize.x, (pT - pB) / texelSize.y);
//	out_color = vec4(res, 0.0, 1.0);
}

#version 330 core

uniform sampler2D uTexture;
uniform sampler2D uTextureB;
uniform sampler2D uTextureObstacle;

layout(location = 0) out vec4 out_color;

void main()
{	
	vec2 texelSize = 1.0 / textureSize(uTexture, 0);
	const float dt = 0.01;
	const float n = 0.00001;
	float alpha = texelSize.x * texelSize.x / (n * dt);
	float beta = 4.0 + alpha;

	vec2 fragCoord = gl_FragCoord.xy * texelSize;

	vec3 oC = texture(uTextureObstacle, fragCoord).xyz;
    if (oC.x > 0.0)
	{
        out_color = vec4(oC.yz, 0.0, 1.0);
        return;
    }

	ivec2 t = ivec2(gl_FragCoord.xy);

	vec2 uR = texelFetchOffset(uTexture, t, 0, ivec2(1, 0)).xy;
	vec2 uL = texelFetchOffset(uTexture, t, 0, ivec2(-1, 0)).xy;
	vec2 uT = texelFetchOffset(uTexture, t, 0, ivec2(0, 1)).xy;
	vec2 uB = texelFetchOffset(uTexture, t, 0, ivec2(0, -1)).xy;

	vec3 oL = texelFetchOffset(uTextureObstacle, t, 0, ivec2(-1, 0)).xyz;
	vec3 oR = texelFetchOffset(uTextureObstacle, t, 0, ivec2(1, 0)).xyz;
	vec3 oB = texelFetchOffset(uTextureObstacle, t, 0, ivec2(0, -1)).xyz;
	vec3 oT = texelFetchOffset(uTextureObstacle, t, 0, ivec2(0, 1)).xyz;


	if (oL.x > 0.0) uL = oL.yz;
	if (oR.x > 0.0) uR = oR.yz;
	if (oB.x > 0.0) uB = oB.yz;
	if (oT.x > 0.0) uT = oT.yz;

	vec2 uC = texelFetch(uTextureB, t, 0).xy;
	
	vec2 res = (uT + uR + uB + uL + alpha * uC) / beta;
	out_color = vec4(res, 0.0, 1.0);
}

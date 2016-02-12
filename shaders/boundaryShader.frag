#version 330 core

uniform sampler2D uTexture;
uniform sampler2D uTextureObstacle;

layout(location = 0) out vec4 out_color;

void main()
{
	vec2 texelSize = 1.0 / textureSize(uTexture, 0);
	vec2 fragCoord = gl_FragCoord.xy * texelSize;

	vec2 offset = texture(uTextureObstacle, fragCoord).yz * texelSize;

	vec2 u = texture(uTexture, fragCoord + offset).xy;

	out_color = vec4(-u, 0.0, 1.0);
	//out_color = vec4(0.0, 0.0, 0.0, 1.0);
}

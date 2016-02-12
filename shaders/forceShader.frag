#version 330 core

uniform sampler2D uTexture;
uniform vec2 uForcePos;
uniform vec2 uForceDir;

layout(location = 0) out vec4 out_color;

void main()
{	
	vec2 texelSize = 1.0 / textureSize(uTexture, 0);
	const float dt = 0.01;
	vec2 fragCoord = gl_FragCoord.xy * texelSize;

//	ivec2 t = ivec2(gl_FragCoord.xy);


	bool isInteraction = true;
	if (isInteraction)
	{	
		vec2 diff = fragCoord - uForcePos;
		float d = 4000.0f * dot(diff, diff);

		vec2 vel = texture(uTexture, fragCoord).xy;
		vel += uForceDir * dt * exp(-d);

		if ((fragCoord.y < 0.1) && (fragCoord.y > 0.01) && (fragCoord.x > 0.01) && (fragCoord.x < 0.99))
		{
			//vel += vec2(0.0, 5.0) * dt;
			//vel += uForceDir * dt;
		}

		if ((fragCoord.y < 0.1) && (fragCoord.y > 0.01))
		{
			//if ((fragCoord.x > 0.34) && (fragCoord.x < 0.35) || (fragCoord.x > 0.44) && (fragCoord.x < 0.45) || (fragCoord.x > 0.55) && (fragCoord.x < 0.56) || (fragCoord.x > 0.65) && (fragCoord.x < 0.66))
			//vel += vec2(0.0, 1.0) * dt;
			//vel += uForceDir * dt;
		}
		out_color = vec4(vel, 0.0, 1.0);
	}
	else
	{
		vec2 dist = 2.0 * (fragCoord - vec2(0.50084, 0.50084));
		float d = dot(dist, dist);

		vec2 vel = texture(uTexture, fragCoord).xy;

		if ((d > 0.94) && (d < 0.95))
		{
			float theta = atan(dist.x, dist.y);
			if ((abs(dist.x) < 0.01) && (dist.y > 0.0))
			{
				theta = 0.5 * 3.1415;
			}

			if ((abs(dist.x) < 0.01) && (dist.y < 0.0))
			{
				theta = -0.5 * 3.1415;
			}

//			float fx = cos(theta) * (cos(theta) * sin(theta) - sin(2.0 * theta));
//			float fy = pow(cos(theta), 3.0) + sin(2.0 * theta) * sin(theta);

			float fx = -cos(theta) * sin(2.0 * theta);
			float fy = sin(2.0 * theta) * sin(theta);

			vel += 100.0 * dt * vec2(fx, fy);
		}
		out_color = vec4(vel, 0.0, 1.0);
	}
}

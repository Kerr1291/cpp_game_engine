
uniform sampler2D glowTexture;

uniform vec4 lightColor;
uniform float lightRadius;
uniform float lightIntensity;
uniform float lightAngle;
uniform vec2 lightDirection;

uniform float screenHeight;

in vec2 light_screen_pos;

uniform float pass;

float atten(float d)
{
	// float attenuation = 1.0 / d * lightIntensity;	
	
	// float attenuation = ( 1.0-(d*d) );	
	
	float d_sq = d*d;
	float d_cb = d_sq*d;
	
	float attenuation = 1 - 27.08490804*d_cb*d_sq + 77.22348199*d_sq*d_sq - 75.40312421*d_cb + 27.87226002*d_sq - 3.607709751*d;
	
	return attenuation;
}

float inten(float i)
{ 
	float x = i;
	float intensity = -log(x+0.1);
	return intensity;
}

void main() 
{	

	vec4 color = vec4( 20.0 * texture2D( glowTexture , gl_TexCoord[0].st ).rgb * pass, 1.0 );
	vec2 lightPos = light_screen_pos;
	
	vec2 pixel = gl_FragCoord.xy;	
	pixel.y = screenHeight - pixel.y;		
	vec2 aux = lightPos - pixel;
	float d = length(aux);
	float zoom = lightAngle;
	d *= zoom;
	
	
	// vec3 dd=vec3(aux.x, aux.y, 0.0);
	
	if(d <= lightRadius)
	{			
		float n_d = d / lightRadius;
		float attenuation = atten(n_d);
		float intensity = inten( n_d ) * lightIntensity;
		float brightness = attenuation; //clamp(attenuation + intensity, 0.0, 1.0);
		vec3 final_color = vec3( lightColor.r + intensity, lightColor.g + intensity, lightColor.b + intensity );
		color = vec4(brightness, brightness, brightness, 1.0) * vec4(final_color, 1.0);
		
		
		/*
		float decrease = 1.0 / d * lightIntensity * 10.0;
		float shadows = 0.05;
		
		float maxDecrease = clamp((1.0 - d / lightRadius), 0.0, 1.0);
		
		if( lightDirection != vec2(0,0) )
		{
			if( dot( normalize(aux), lightDirection ) < lightAngle * d / lightRadius )
				shadows=0;
		}
			
		color = vec4(decrease * shadows, decrease * shadows, decrease * shadows, 1.0) * vec4(lightColor.rgb * maxDecrease, 1.0);		
		*/
	}	
	
	gl_FragData[0] = color;
}
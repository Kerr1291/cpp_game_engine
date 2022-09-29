
uniform sampler2D screenTexture;
uniform sampler2D glowTexture;

uniform vec3 ambient_light;

void main() 
{	
	vec4 dcolor = texture2D( screenTexture, gl_TexCoord[0].st );
	vec4 gcolor = texture2D( glowTexture, gl_TexCoord[0].st );
	vec4 color = vec4(0,0,0,0);
	vec4 amb = vec4(ambient_light,1.0);
	// color = gcolor*gcolor.a + color*color.a*(1.0-gcolor.a);
	// color = color*color.a + gcolor*gcolor.a*(1.0-color.a);
	
	// vec4 one = vec4(1.0);
	// color = one - (1 - dcolor) * (1 - gcolor);
	
	color = dcolor * amb;
	color += dcolor * gcolor * 4.0 + gcolor;
	
	gl_FragData[0] = color;
}
	
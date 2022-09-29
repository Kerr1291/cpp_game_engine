#version 130

#extension GL_EXT_gpu_shader4 : enable //requires opengl 3.0 support

uniform sampler2D atlasTexture;

uniform float flipped;

uniform float glowPass;

out vec4 ocolor;
// layout(location = 1) out vec4 glowColor;

in vec2 pos;
in vec2 size;

void main()
{	
	vec2 tc = gl_TexCoord[0].st;
	     tc.x = flipped * (1.0 - tc.x) + (1.0 - flipped) * tc.x;
	
	vec2 final_pos;
	final_pos.x = pos.x + tc.x * size.x;
	final_pos.y = pos.y + tc.y * size.y; 
	
	vec4 color = texelFetch(atlasTexture, ivec2(final_pos), 0);
	
	// ignore magic pink (consider it alpha 0)
	if(color.r >= 1.0 && color.g <= 0 && color.b >= 1.0)
		color.a = 0.0;
	
	// blend as needed
	ocolor = color*color.a + ocolor*ocolor.a*(1.0-color.a);
	
	if( glowPass > 0 )
	{
		if(ocolor.r > 0.95 && ocolor.g > 0.95 && ocolor.b > 0.95)
			ocolor = vec4(1.0,0.5,0.0,1.0)*color.a;
		else
			ocolor = vec4(0,0,0,0);
	}
}

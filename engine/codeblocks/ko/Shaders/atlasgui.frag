#version 130

#extension GL_EXT_gpu_shader4 : enable //requires opengl 3.0 support

uniform sampler2D atlasTexture;

out vec4 ocolor;

flat in uvec2 pos;
flat in uvec2 size;

void main()
{	
	vec2 tc = gl_TexCoord[0].st;
	
	vec2 final_pos;
	final_pos.x = pos.x + tc.x * size.x;
	final_pos.y = pos.y + tc.y * size.y; 
	
	vec4 color = texelFetch(atlasTexture, ivec2(final_pos), 0);
	
	// ignore magic pink (consider it alpha 0)
	if(color.r >= 1.0 && color.g <= 0 && color.b >= 1.0)
		color.a = 0.0;
	
	//blend as needed
	ocolor = color*color.a + ocolor*ocolor.a*(1.0-color.a);
}

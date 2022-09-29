#version 130

#extension GL_EXT_gpu_shader4 : enable //requires opengl 3.0 support

uniform usampler2D gridTexture;
uniform sampler2D atlasTexture;
uniform usampler1D atlasMapTexture;

uniform vec2 gridSize;
uniform vec2 cellSize;

uniform int pass;

in vec2 gSize;

layout(location = 0) out vec4 ocolor;
// layout(location = 1) out vec4 glowColor;

void main()
{	
	ocolor = vec4(0,0,0,0);
	// glowColor = vec4(0,0,0,0);
	
	//look up into gridTexture, get the index
	vec2 tc = gl_TexCoord[0].st;
	tc.x *= (gridSize.x);
	tc.y *= (gridSize.y);
	
	ivec2 tc_i = ivec2( int(tc.x), int(tc.y) );
	
	uvec4 lookupIndex = texelFetch(gridTexture, tc_i, 0);
	
	for( int i = 2*pass; i < 2*(pass+1); ++i )
	{
		const unsigned int short_max = 65535u;
	
		unsigned int l = lookupIndex[i]; // TODO: split this into two passes (0,1 and 2,3) for background and foreground
		
		if( l == short_max ) //-1 (which is short_max) is a non-texture, don't try to draw it or anything
			continue;
		
		// use the index, look up the pos and size of the tex in the atlas
		uvec4 atlas_info = texelFetch( atlasMapTexture, int(l), 0 );
		
		// extract the interleaved info
		uvec2 pos = atlas_info.xy; 
		uvec2 size = atlas_info.zw;
		
		// use the info to look up the texture in the atlas
		vec2 final_pos;
		vec2 transformed_tc;
		
		transformed_tc = vec2( gl_TexCoord[0].s * gSize.x, gl_TexCoord[0].t * gSize.y );
		
		vec2 t_tc_i = vec2( mod( transformed_tc.x, cellSize.x ), mod( transformed_tc.y, cellSize.y ) );
		transformed_tc = vec2( t_tc_i.x / cellSize.x, t_tc_i.y / cellSize.y );
		
		final_pos.x = pos.x + ( transformed_tc.x ) * (size.x);
		final_pos.y = pos.y + ( transformed_tc.y ) * (size.y); 

		// get the color from the appropriate spot in the atlas using the exact texture location
		vec4 color = texelFetch(atlasTexture, ivec2(final_pos), 0);
		
		if(color.r >= 1.0 && color.g <= 0 && color.b >= 1.0)
			color.a = 0.0;
		
		// blend as needed
		ocolor = color*color.a + ocolor*ocolor.a*(1.0-color.a);
	
	}
}

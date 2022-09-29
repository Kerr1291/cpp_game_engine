#version 130

#extension GL_EXT_gpu_shader4 : enable //requires opengl 3.0 support

uniform mat4 projectionMatrix;

uniform vec2 texturePosition;
uniform vec2 textureScale;

uniform int atlasTextureIndex;
uniform int drawIndex;
uniform usampler1D atlasMapTexture;

out flat uvec2 pos;
out flat uvec2 size;

void main(void) 
{ 
	vec2 vert = gl_Vertex.xy;
	vec2 v = vec2(vert.x * textureScale.x, vert.y * textureScale.y);
		 v = v + texturePosition;
		 v.x += textureScale.x * drawIndex;
	gl_Position = projectionMatrix * vec4(v.x,v.y,0,1);
	
	uvec4 atlas_info = texelFetch( atlasMapTexture, atlasTextureIndex, 0 );
	
	pos = atlas_info.xy; 
	size = atlas_info.zw;
	
	gl_TexCoord[0] = gl_MultiTexCoord0; 
}

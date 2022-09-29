#version 130

uniform mat4 projectionMatrix;

uniform vec2 spritePosition;
uniform mat2 spriteRotation;
uniform vec2 spriteScale;

uniform float animationFrame;
uniform vec2 animationSize;
uniform vec2 animationStartPos;
uniform float animationSpace;

out vec2 pos;
out vec2 size;

void main(void) 
{ 
	vec2 vert = gl_Vertex.xy;
	vec2 v = vec2(vert.x * spriteScale.x, vert.y * spriteScale.y);
		 v = v * spriteRotation;
		 v = v + spritePosition;
	gl_Position = projectionMatrix * vec4(v.x,v.y,0,1);
	
	pos = animationStartPos;
		pos.x += animationFrame * animationSize.x;
		pos.x += animationFrame * animationSpace; 
	size = animationSize;
	
	gl_TexCoord[0] = gl_MultiTexCoord0; 
}

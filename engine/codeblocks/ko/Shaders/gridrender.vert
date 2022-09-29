#version 130

//in vec2 vert;
//in vec4 gl_Vertex;

uniform vec2 gridPosition;
uniform mat4 projectionMatrix;

uniform vec2 gridSize;
uniform vec2 cellSize;

out vec2 gSize;

void main(void) 
{ 
	vec2 scale = vec2( gridSize.x * cellSize.x, gridSize.y * cellSize.y );
	
	vec2 vert = gl_Vertex.xy;
	vec2 v = vec2( vert.x * scale.x, vert.y * scale.y );
		 v = v + gridPosition;
	gl_Position = projectionMatrix * vec4(v.x,v.y,0,1);
	
	gl_TexCoord[0] = gl_MultiTexCoord0; 

	gSize = scale;
}


in vec2 vert;

uniform vec2 scale;
uniform mat2 rotation;
uniform vec2 pos;
uniform mat4 projectionMatrix;

void main(void) 
{ 
	vec2 v = vec2(vert.x * scale.x, vert.y * scale.y);
		 v = v * rotation;
		 v = v + pos;
	gl_Position = projectionMatrix * vec4(v.x,v.y,0,1);
}
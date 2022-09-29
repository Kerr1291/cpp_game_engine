
in vec2 vert;
in vec2 UV;
out vec2 UV_frag;

void main(void) 
{ 
	vec2 v = vert;
	gl_Position = vec4(v.x,v.y,0,1);
}

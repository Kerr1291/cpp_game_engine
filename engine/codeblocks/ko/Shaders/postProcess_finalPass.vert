
void main(void) 
{ 
	vec2 v = gl_Vertex.xy;
	gl_Position = vec4(gl_Vertex.xy,0,1.0);
	gl_TexCoord[0] = gl_MultiTexCoord0; 
}
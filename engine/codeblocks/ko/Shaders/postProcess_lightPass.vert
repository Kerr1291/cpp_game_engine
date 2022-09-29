
uniform mat4 projectionMatrix;
uniform vec2 lightPos;

out varying vec2 light_screen_pos;

uniform float screenHeight;
uniform float screenWidth;

void main(void) 
{ 
	vec4 light_pos = projectionMatrix * vec4( lightPos, 0, 1.0 );
	
	light_screen_pos = (light_pos.xy + vec2(1.0)) / 2.0;
	light_screen_pos.x *= screenWidth;
	light_screen_pos.y *= screenHeight;
	light_screen_pos.y = screenHeight - light_screen_pos.y;
	
	vec2 v = gl_Vertex.xy;
	gl_Position = vec4(gl_Vertex.xy,0,1.0);
	gl_TexCoord[0] = gl_MultiTexCoord0; 
}
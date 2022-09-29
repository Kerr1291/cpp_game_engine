
uniform vec4 object_color;
uniform int hasGlow;

void main() 
{	
	gl_FragData[0] = object_color;// + object_color * float(hasGlow) * 5;
	// gl_FragData[1] = object_color * float(hasGlow);
}
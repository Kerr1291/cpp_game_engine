 
out vec2 v_texCoord;
out vec2 v_blurTexCoords[14];

uniform float pass;

uniform float sample_radius;

void main(void) 
{ 
	vec2 v = gl_Vertex.xy;
	gl_Position = vec4(gl_Vertex.xy,0,1.0);
	
	float c_pass = clamp(pass,0,1.0);
	
	v_texCoord = gl_MultiTexCoord0.st;
    v_blurTexCoords[ 0] = v_texCoord + vec2( (1.0 - c_pass) * -0.028 * sample_radius, c_pass * -0.028 * sample_radius);
    v_blurTexCoords[ 1] = v_texCoord + vec2( (1.0 - c_pass) * -0.024 * sample_radius, c_pass * -0.024 * sample_radius);
    v_blurTexCoords[ 2] = v_texCoord + vec2( (1.0 - c_pass) * -0.020 * sample_radius, c_pass * -0.020 * sample_radius);
    v_blurTexCoords[ 3] = v_texCoord + vec2( (1.0 - c_pass) * -0.016 * sample_radius, c_pass * -0.016 * sample_radius);
    v_blurTexCoords[ 4] = v_texCoord + vec2( (1.0 - c_pass) * -0.012 * sample_radius, c_pass * -0.012 * sample_radius);
    v_blurTexCoords[ 5] = v_texCoord + vec2( (1.0 - c_pass) * -0.008 * sample_radius, c_pass * -0.008 * sample_radius);
    v_blurTexCoords[ 6] = v_texCoord + vec2( (1.0 - c_pass) * -0.004 * sample_radius, c_pass * -0.004 * sample_radius);
    v_blurTexCoords[ 7] = v_texCoord + vec2( (1.0 - c_pass) * 0.004 * sample_radius, c_pass * 0.004 * sample_radius);
    v_blurTexCoords[ 8] = v_texCoord + vec2( (1.0 - c_pass) * 0.008 * sample_radius, c_pass * 0.008 * sample_radius);
    v_blurTexCoords[ 9] = v_texCoord + vec2( (1.0 - c_pass) * 0.012 * sample_radius, c_pass * 0.012 * sample_radius);
    v_blurTexCoords[10] = v_texCoord + vec2( (1.0 - c_pass) * 0.016 * sample_radius, c_pass * 0.016 * sample_radius);
    v_blurTexCoords[11] = v_texCoord + vec2( (1.0 - c_pass) * 0.020 * sample_radius, c_pass * 0.020 * sample_radius);
    v_blurTexCoords[12] = v_texCoord + vec2( (1.0 - c_pass) * 0.024 * sample_radius, c_pass * 0.024 * sample_radius);
    v_blurTexCoords[13] = v_texCoord + vec2( (1.0 - c_pass) * 0.028 * sample_radius, c_pass * 0.028 * sample_radius);
}
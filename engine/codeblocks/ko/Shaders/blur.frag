
in flat int direction;
in vec2 UV;

out vec4 color;
 
uniform sampler2D glowTexture;
 
void main(){

	float uv_x = UV.x * (1 - direction);
	float uv_y = UV.y * direction;
	vec2 readPos0 = vec2( uv_x, uv_y );
	vec2 readPos1 = vec2( uv_x, uv_y ) * 0.9;
	vec2 readPos2 = vec2( uv_x, uv_y ) * 0.8;
	vec2 readPos3 = vec2( uv_x, uv_y ) * 0.7;
	vec2 readPos4 = vec2( uv_x, uv_y ) * 0.6;
	
	color =	  texture2D( glowTexture, readPos0 )
			 + texture2D( glowTexture, readPos1 )
			 + texture2D( glowTexture, readPos2 )
			 + texture2D( glowTexture, readPos3 )
			 + texture2D( glowTexture, readPos4 ) 
			 ;
    
}
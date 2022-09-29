
in vec2 UV_frag;
 
out vec4 color;
 
uniform sampler2D renderedTexture;
 
void main(){
    color = texture2D( renderedTexture, UV_frag );
}
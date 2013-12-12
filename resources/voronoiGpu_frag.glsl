
#version 120
#extension GL_ARB_texture_rectangle: enable
uniform sampler2DRect tex0;
uniform vec2 sampleScale;
void main() {
	vec2 pos[9];
	pos[0] = texture2DRect( tex0, gl_TexCoord[0].st + vec2(-1.0,-1.0) * sampleScale ).rg;
	pos[1] = texture2DRect( tex0, gl_TexCoord[0].st + vec2(0.0,-1.0) * sampleScale ).rg;
	pos[2] = texture2DRect( tex0, gl_TexCoord[0].st + vec2(1.0,-1.0) * sampleScale ).rg;
	pos[3] = texture2DRect( tex0, gl_TexCoord[0].st + vec2(-1.0,0.0) * sampleScale ).rg;
	pos[4] = texture2DRect( tex0, gl_TexCoord[0].st + vec2(0.0,0.0) * sampleScale ).rg;
	pos[5] = texture2DRect( tex0, gl_TexCoord[0].st + vec2(1.0,0.0) * sampleScale ).rg;
	pos[6] = texture2DRect( tex0, gl_TexCoord[0].st + vec2(-1.0,1.0) * sampleScale ).rg;
	pos[7] = texture2DRect( tex0, gl_TexCoord[0].st + vec2(0.0,1.0) * sampleScale ).rg;
	pos[8] = texture2DRect( tex0, gl_TexCoord[0].st + vec2(1.0,1.0) * sampleScale ).rg;
	
	vec2 smallest = vec2( -1, -1 );
	float smallestDistance = 65535.0 * 65535.0;//distance( gl_TexCoord[0].st, pos[0] );
	for( int s = 0; s < 9; ++s ) {
// This test seems to blow up the 8800 under Mac OS X 10.5.8 quite handily, but it's not necessary if we seed the field w/ large negative closest sites
//	"		if( pos[s].x < 0 ) continue;
		float distance = distance( gl_TexCoord[0].st, pos[s] );
		if( distance < smallestDistance ) {
			smallestDistance = distance;
			smallest = pos[s];
		}
	}

	gl_FragColor.rg = smallest;
};

/* Old code from mandelbrot set
#version 120

uniform sampler2D tex;
uniform vec2 center;
uniform float scale;
uniform int iter;

void main() {
    vec2 z, c;
    
    c.x = 1.3333 * (gl_TexCoord[0].x - 0.5) * scale - center.x;
    c.y = (gl_TexCoord[0].y - 0.5) * scale - center.y;
    
    int i;
    z = c;
    for( i=0; i<iter; i++ ){
        float x = (z.x * z.x - z.y * z.y) + c.x;
        float y = (z.y * z.x + z.x * z.y) + c.y;
        
        if( ( x * x + y * y ) > 4.0 ) break;
        z.x = x;
        z.y = y;
    }
    vec2 texCoord = vec2( (i == iter ? 0.0 : float(i)) / 100.0, 0.0 );
    vec3 color = texture2D( tex, texCoord ).rgb;
    
    gl_FragColor.rgb = color;
    gl_FragColor.a = 1.0;
}//*/


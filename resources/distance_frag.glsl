uniform sampler2DRect tex0;
void main() {
	gl_FragColor.rgb = vec3( distance( texture2DRect( tex0, gl_TexCoord[0].st ).rg, gl_TexCoord[0].st ) );
}
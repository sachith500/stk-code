#version 130
uniform sampler2D tex;

void main()
{
	vec2 texc = gl_TexCoord[0].xy;
	texc.y = 1.0 - texc.y;

  
	gl_FragColor = vec4(texture2D(tex, texc).rgb, 1.0);
}
uniform sampler2D tex;
uniform sampler2D dtex;
uniform mat4 invproj;
uniform vec2 screen;

in float lf;
in vec2 tc;
out vec4 color;


void main(void)
{
	vec2 xy = gl_FragCoord.xy / screen;
	float FragZ = gl_FragCoord.z;
	float EnvZ = texture(dtex, xy).x;
	vec4 FragmentPos = invproj * (2. * vec4(xy, FragZ, 1.0) - 1.);
	FragmentPos /= FragmentPos.w;
	vec4 EnvPos = invproj * (2. * vec4(xy, EnvZ, 1.0) - 1.);
	EnvPos /= EnvPos.w;
	float alpha = clamp((EnvPos.z - FragmentPos.z) * 0.3, 0., 1.);
	color = texture(tex, tc);
    color.a *= alpha * smoothstep(1., 0.8, lf);
}

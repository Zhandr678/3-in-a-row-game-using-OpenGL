#version 330 core

in vec3 vcolor;
in vec3 vnormal;
in vec3 vpos;
in vec2 vtcoord;

uniform sampler2D tex1;
uniform vec3 campos;
uniform vec3 lightpos;

out vec4 gl_FragColor;

void main() 
{
	vec3 camdir = normalize(campos - vpos);
	float lighting = max(dot(normalize(vnormal), normalize(lightpos - vpos)), 0.5);
	vec3 r = reflect(-normalize(lightpos - vpos), normalize(vnormal));
	float specularity = max(pow(dot(camdir, r), 10), 0);
	//vec3 difcolor = texture(tex1, vtcoord).xyz;
	gl_FragColor = vec4(lighting * vcolor + 2 * specularity * vec3(1), 0);
}
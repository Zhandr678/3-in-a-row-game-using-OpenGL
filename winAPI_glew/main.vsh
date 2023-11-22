#version 330 core

in vec3 pos; 
in vec3 color; 
in vec3 normal; 
in vec2 tcoord; 

out vec3 vcolor; 
out vec3 vnormal; 
out vec3 vpos; 
out vec2 vtcoord; 

uniform mat4 transform; 
uniform mat4 rtransform; 
uniform mat4 projection;

void main() 
{
	vcolor = color;
	vtcoord = tcoord;
	vnormal = (rtransform * vec4(normal, 1)).xyz;
	vpos = (transform * vec4(pos, 1)).xyz;
	gl_Position = projection * transform * vec4(pos, 1);
}
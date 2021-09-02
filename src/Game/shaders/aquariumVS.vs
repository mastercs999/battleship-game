#version 130

in vec3 position; 
in vec2 tc; 
in vec2 tcLight;
in vec2 tcNormal;
in vec3 normal;
in vec3 tangent;

uniform mat4 mvp; 
uniform mat4 mv;
uniform float time;
uniform vec2 delta;

out vec2 texcoord; 
out vec2 texLightCoord;
out vec2 texNormalCoord;
out vec3 VertexPosition;
out vec3 n;
out vec3 t;

void main() { 
	gl_Position = mvp*vec4(position,1); 
	texcoord = tc; 
	texLightCoord = tcLight + delta * time / 20;
	VertexPosition = position;
	
	// Normal mapping
	texNormalCoord = tcNormal;
        n = normal;
        t = tangent;
}





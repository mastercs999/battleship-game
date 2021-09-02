#version 130

uniform mat4 m;
uniform mat4 v;
uniform mat4 p;

in vec3 position;
in vec4 color;
in vec2 tc;

out vec2 texcoord;
out vec4 c;
out vec3 VertexPosition;
out vec3 n;
out mat4 vMat;

void main()
{
    gl_Position = p * v * m * vec4(position,1) ;
    c = color;
    texcoord = tc;
    vec4 newPos = m * vec4(position,1);
    VertexPosition = vec3(newPos.x, newPos.y, newPos.z);
    n = vec3(color);
}

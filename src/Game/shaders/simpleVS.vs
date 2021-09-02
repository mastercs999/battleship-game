#version 130

uniform mat4 mvp;

in vec3 position;
in vec4 color;

out vec4 c;

void main()
{
    gl_Position = mvp * vec4(position,1) ;
    c = color;
}

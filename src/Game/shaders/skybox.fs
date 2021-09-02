#version 130


in vec3 texcoords;
uniform samplerCube cube_texture;
out vec4 fragColor;

void main()
{
    fragColor = texture (cube_texture, texcoords);
}

#version 330 core

in vec4 vCol;		// was vec3
in vec4 vPos;		// was vec3

out vec4 vertexColor;

uniform mat4 matView;
uniform mat4 matProjection;
void main()
{
    gl_Position = matProjection * matView * vec4(vPos.xyz, 1.0);
    vertexColor = vCol;
}
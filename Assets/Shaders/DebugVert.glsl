#version 330 core

in vec4 vCol;		// was vec3
in vec4 vPos;		// was vec3

out vec4 vertexColor;

uniform mat4 cameraView;

void main()
{
    gl_Position = cameraView * vec4(vPos.xyz, 1.0);
    vertexColor = vCol;
}
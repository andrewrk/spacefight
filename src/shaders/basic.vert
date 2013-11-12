#version 330 core

in vec3 VertexPosition;
uniform mat4 MVP;

void main()
{
    vec4 v = vec4(VertexPosition, 1.0);
    gl_Position = MVP * v;
}

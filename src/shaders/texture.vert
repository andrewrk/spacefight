#version 330 core

in vec3 VertexPosition;
in vec2 TextureCoords;


out vec2 texCoord;

uniform mat4 MVP; // projection * ModelViewMatrix

void main()
{
    gl_Position = MVP * vec4(VertexPosition, 1.0);
    texCoord = TextureCoords;

}



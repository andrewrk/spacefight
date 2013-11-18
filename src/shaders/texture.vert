#version 150 core

in vec3 VertexPosition;
in vec2 TextureCoords;


//in vec3 VertexNormal;
//out vec3 EyePosition;
//out vec3 EyeNormal;
out vec2 texCoord;

uniform mat4 ModelViewMatrix;
//uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP; // projection * ModelViewMatrix

void main()
{
  /*  EyeNormal = normalize(NormalMatrix * VertexNormal);
    EyePosition = vec3(ModelViewMatrix * vec4(VertexPosition, 1.0));*/

    gl_Position = MVP * vec4(VertexPosition, 1.0);
    texCoord = TextureCoords;

}

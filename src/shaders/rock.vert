#version 330 core

in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 TexCoord;
out vec3 EyePosition;
out vec3 EyeNormal;
out vec2 FragTexCoord;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main()
{
    FragTexCoord = TexCoord;
    EyeNormal = normalize(NormalMatrix * VertexNormal);
    EyePosition = vec3(ModelViewMatrix * vec4(VertexPosition, 1.0));
    gl_Position = MVP * vec4(VertexPosition, 1.0);

}


#version 330 core

in vec3 VertexPosition;
in vec3 VertexNormal;
out vec3 LightIntensity;
uniform vec4 LightPosition; // light position in eye coords
uniform vec3 DiffuseReflectivity; // diffuse reflectivity
uniform vec3 LightSourceIntensity; // light source intensity
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP; // projection * ModelViewMatrix

void main()
{
    // convert normal and position to eye coords
    vec3 tnorm = normalize(NormalMatrix * VertexNormal);
    vec4 eyeCoords = ModelViewMatrix * vec4(VertexPosition, 1.0);
    vec3 s = normalize(vec3(LightPosition - eyeCoords));
    // the diffuse shading equation
    LightIntensity = LightSourceIntensity * DiffuseReflectivity * max(dot(s, tnorm), 0.0);
    LightIntensity = vec3(1, 0, 0);
    // convert position to clip coordinates and pass along
    gl_Position = MVP * vec4(VertexPosition, 1.0);
}


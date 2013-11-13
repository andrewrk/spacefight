#version 330 core

in vec3 VertexPosition;
in vec3 VertexNormal;
out vec3 LightIntensity;

struct LightInfo {
    vec4 Position; // light position in eye coords
    vec3 La; // ambient light intensity
    vec3 Ld; // diffuse light intensity
    vec3 Ls; // specular light intensity
};

uniform LightInfo Light;

struct MaterialInfo {
    vec3 Ka; // ambient reflectivity
    vec3 Kd; // diffuse reflectivity
    vec3 Ks; // specular reflectivity
    float Shininess; // specular shininess factor
};

uniform MaterialInfo Material;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP; // projection * ModelViewMatrix

void main()
{
    // convert normal and position to eye coords
    vec3 tnorm = normalize(NormalMatrix * VertexNormal);
    vec4 eyeCoords = ModelViewMatrix * vec4(VertexPosition, 1.0);
    vec3 s = normalize(vec3(Light.Position - eyeCoords));
    vec3 v = normalize(-eyeCoords.xyz);
    vec3 r = reflect(-s, tnorm);
    vec3 ambient = Light.La * Material.Ka;
    float sDotN = max(dot(s, tnorm), 0.0);
    vec3 diffuse = Light.Ld * Material.Kd * sDotN;
    vec3 spec = vec3(0.0);
    if (sDotN > 0.0) {
        spec = Light.Ls * Material.Ks * pow(max(dot(r,v), 0.0), Material.Shininess);
    }
    LightIntensity = ambient + diffuse + spec;
    // convert position to clip coordinates and pass along
    gl_Position = MVP * vec4(VertexPosition, 1.0);
}


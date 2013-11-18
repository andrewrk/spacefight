#version 330 core

in vec3 EyePosition;
in vec3 EyeNormal;

out vec4 FragColor;

layout (shared) uniform Light {
    vec4 Position; // light position in eye coords
    vec3 La; // ambient light intensity
    vec3 Ld; // diffuse light intensity
    vec3 Ls; // specular light intensity
};

layout (shared) uniform Material {
    vec3 Ka; // ambient reflectivity
    vec3 Kd; // diffuse reflectivity
    vec3 Ks; // specular reflectivity
    float Shininess; // specular shininess factor
};

const int levels = 3;
const float scaleFactor = 1.0 / levels;

void main()
{
    vec3 s = normalize(Position.xyz - EyePosition);
    float cosine = max(0.0, dot(s, EyeNormal));
    vec3 diffuse = Ld * Kd * ceil(cosine * levels) * scaleFactor;
    vec3 color3 = La * Ka + diffuse;
    FragColor = vec4(color3, 1.0);
}



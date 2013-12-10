#version 330 core

in vec3 EyePosition;
in vec3 EyeNormal;

out vec4 FragColor;

layout (shared) uniform Light {
    vec3 LightDirection; // light direction, should be normalized
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


void main()
{
    vec3 v = normalize(vec3(-EyePosition));
    vec3 h = normalize(v + LightDirection);
    vec3 ambAndDiff = La * Ka +
            Ld * Kd * max(dot(LightDirection, EyeNormal), 0.0);
    vec3 spec = Ls * Ks * pow(max(dot(h, EyeNormal), 0.0), Shininess);
    FragColor = vec4(ambAndDiff + spec, 1.0);
}



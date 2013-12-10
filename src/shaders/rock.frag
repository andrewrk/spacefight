#version 330 core

in vec3 EyePosition;
in vec3 EyeNormal;
in vec2 FragTexCoord;

out vec4 FragColor;

uniform sampler2D Tex;

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


void main()
{
    vec3 s = normalize(Position.xyz - EyePosition);
    vec3 v = normalize(vec3(-Position));
    vec3 r = reflect(-s, EyeNormal);
    vec3 ads = La * Ka +
            Ld * Kd * max(dot(s, EyeNormal), 0.0) * vec3(texture(Tex, FragTexCoord)) +
            Ls * Ks * pow(max(dot(r, v), 0.0), Shininess);
    FragColor = vec4(ads, 1.0);
}



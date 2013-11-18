#version 150 core

in vec2 texCoord;

uniform sampler2D tex;

out vec4 out_Color;


void main()
{
    out_Color = texture(tex, texCoord);
    //out_Color = vec4(1, 1, 1, 1);
}

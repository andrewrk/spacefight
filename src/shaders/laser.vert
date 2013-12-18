#version 330 core

in vec3 PosStart;
in vec3 PosEnd;
in float Offset;
out vec3 FragPosition;

uniform mat4 MVP; // projection * ModelViewMatrix


void main()
{
    // project into clip space
    vec4 startpos = MVP * vec4(PosStart, 1.0);
    vec4 endpos = MVP * vec4(PosEnd, 1.0);

    // project into screen space
    vec2 startpos2d = startpos.xy / startpos.w;
    vec2 endpos2d = endpos.xy / endpos.w;

    vec2 linedir2d = normalize(startpos2d - endpos2d) * Offset;
    startpos.x += linedir2d.y;
    startpos.y -= linedir2d.x;

    gl_Position = startpos;
}


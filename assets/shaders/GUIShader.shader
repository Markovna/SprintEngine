#type vertex
#version 330 core

layout (location = 0) in vec2 Position;
layout (location = 1) in vec2 UV;
layout (location = 2) in vec4 Color;

out vec2 Frag_UV;
out vec4 Frag_Color;

uniform mat4 ProjMtx;

void main()
{
    Frag_UV = UV;
    Frag_Color = Color;
    gl_Position = ProjMtx * vec4(Position.xy,0,1);
}

#type fragment
#version 330 core

in vec2 Frag_UV;
in vec4 Frag_Color;

out vec4 Out_Color;

uniform sampler2D Texture;

void main()
{
    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);
}
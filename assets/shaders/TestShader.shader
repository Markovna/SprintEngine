#type vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aCol;
layout (location = 2) in vec2 aTexCoord;

out vec4 vColor;
out vec2 vTexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    vColor = aCol;
    vTexCoord = aTexCoord;
}

#type fragment
#version 330 core

in vec4 vColor;
in vec2 vTexCoord;

out vec4 FragColor;

uniform vec4 mainColor;
uniform sampler2D Texture1;
uniform sampler2D Texture2;

void main()
{

    FragColor = mainColor * vColor * mix(texture(Texture1, vTexCoord), texture(Texture2, vTexCoord), 0.5);
}
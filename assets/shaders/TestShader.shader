#type vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aCol;

out vec4 vColor;

void main()
{
   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
   vColor = aCol;
}

#type fragment
#version 330 core

in vec4 vColor;
out vec4 FragColor;

uniform vec4 mainColor;

void main()
{
    FragColor = mainColor * vColor;
}
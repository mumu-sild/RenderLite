#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap;


void main()
{
    FragColor = texture(depthMap, TexCoords);
}

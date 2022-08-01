#version 450 core
//in vec2 TexCoords;

uniform vec3 color = vec3(0.5,0.5,0.5);

void main()
{
    gl_FragColor = vec4(color,1.0);
}

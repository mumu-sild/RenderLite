#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D RenderResult;
uniform sampler2D bloomBlur;
uniform bool addBloom = false;


void main()
{
    vec3 hdrColor = texture(RenderResult, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    if(addBloom) hdrColor += bloomColor;
    //if(addBloom) bloomColor += hdrColor;
    //vec3 result = vec3(1.0) - exp(-hdrColor * 1.0);
    FragColor = vec4(hdrColor, 1.0f);
}

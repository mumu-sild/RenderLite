#version 450 core
//in vec2 TexCoords;

//输出
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform vec3 color = vec3(0.5,0.5,0.5);

void main()
{
    FragColor = vec4(color,1.0);
    //检查该像素亮度是否高于阈值
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.2152, 0.0722));
    if(brightness > 1.0) BrightColor = vec4(FragColor.rgb, 1.0);
    else BrightColor = vec4(0,0,0, 1.0);
}

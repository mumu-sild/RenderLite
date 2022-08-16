#version 450 core

//输出
layout (location = 0) out vec4 gPositionDepth;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec4 BrightColor;

//顶点信息
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 color;
uniform bool islight;

const float near_plane = 0.001f;
const float far_plane = 1000.0f;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane))/far_plane;
}

void main()
{
    // 存储第一个G缓冲纹理中的片段位置向量(View)
    gPositionDepth.rgb = FragPos;
    // View视角深度值
    gPositionDepth.a = LinearizeDepth(gl_FragCoord.z);
    // 同样存储对每个逐片段法线到G缓冲中
    gNormal = normalize(Normal);
    // 和漫反射对每个逐片段颜色 , 存储镜面强度到gAlbedoSpec的alpha分量
    gAlbedoSpec= vec4(color,0.3);

    // 高光项存储在BrightColor
    if(islight){
        BrightColor = vec4(color, 1.0);
    }
    else BrightColor = vec4(0,0,0, 1.0);

}

#version 450 core

//���
layout (location = 0) out vec4 gPositionDepth;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec4 BrightColor;

struct Material {
        sampler2D texture_diffuse1;
        sampler2D texture_specular1;
        //float shiness;
};

//������Ϣ
in vec3 Normal;
in vec3 FragPos;//����������������ϵ�µ�����
in vec2 TexCoords;

uniform Material material;
uniform bool islight;

const float near_plane = 0.001f;
const float far_plane = 1000.0f;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main()
{
    // �洢��һ��G���������е�Ƭ��λ������
    gPositionDepth.rgb = FragPos;
    // View�ӽ����ֵ
    gPositionDepth.a = LinearizeDepth(gl_FragCoord.z);
    // ͬ���洢��ÿ����Ƭ�η��ߵ�G������
    gNormal = normalize(Normal);
    // ���������ÿ����Ƭ����ɫ
    gAlbedoSpec.rgb = texture(material.texture_diffuse1, TexCoords).rgb;
    // �洢����ǿ�ȵ�gAlbedoSpec��alpha����
    gAlbedoSpec.a = texture(material.texture_specular1, TexCoords).r;
    // �߹���洢��BrightColor
    if(islight){
        BrightColor = vec4(gAlbedoSpec.rgb, 1.0);
    }
    else BrightColor = vec4(0,0,0, 1.0);
}


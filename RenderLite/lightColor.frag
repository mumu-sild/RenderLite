#version 450 core

//���
layout (location = 0) out vec4 gPositionDepth;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec4 BrightColor;

//������Ϣ
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
    // �洢��һ��G���������е�Ƭ��λ������(View)
    gPositionDepth.rgb = FragPos;
    // View�ӽ����ֵ
    gPositionDepth.a = LinearizeDepth(gl_FragCoord.z);
    // ͬ���洢��ÿ����Ƭ�η��ߵ�G������
    gNormal = normalize(Normal);
    // ���������ÿ����Ƭ����ɫ , �洢����ǿ�ȵ�gAlbedoSpec��alpha����
    gAlbedoSpec= vec4(color,0.3);

    // �߹���洢��BrightColor
    if(islight){
        BrightColor = vec4(color, 1.0);
    }
    else BrightColor = vec4(0,0,0, 1.0);

}

#version 450 core

//����:Ԥ�˲�����
in vec3 WorldPos;
out vec4 FragColor;
//��Ҫ������
uniform samplerCube environmentMap;//������������ͼ
uniform float roughness;//����Ԥ�˲���ģ���̶�

//��������:
uniform uint sample_count;//ÿ���������
//uniform bool enMapHasMipmap;//�Ƿ���mipmap
//uniform int environmentMapSize;//������������ͼ��С

//��������
float RadicalInverse_VdC(uint bits);
vec2 Hammersley(uint i, uint N);
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness);
float DistributionGGX(vec3 N, vec3 H, float roughness);

//const
const float PI = 3.1415926535;


void main(void)
{
    vec3 N = normalize(WorldPos);

    //����������ΪUniform
    //const uint SAMPLE_COUNT = 1024u;
    const uint SAMPLE_COUNT = 1024u;
    //�����������
    vec3 prefilteredColor = vec3(0.0);
    float totalWeight = 0.0;

    //��Ҫ�Բ���
    for(uint i = 0;i<SAMPLE_COUNT;++i){
        //�õ���������
        vec2 randomVec2 = Hammersley(i,SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(randomVec2,N,roughness);
        vec3 L = normalize(2.0 * dot(N, H) * H - N);

        //���⣺�÷��䷽����ܻ������屳������,����Ҫȥ���������
        //��Ϊ�β�ֱ��ʹ��H��Ϊ���䷽���������Ա�����ߵ�ʧЧ�����Ӳ�����������L�ļ���ʱ��
        float NdotL = max(dot(N,L),0.0f);
        if(NdotL > 0.0f){
            //�������mipmap
            float D = DistributionGGX(N,H,roughness);
            float pdf = D / 4.0 + 0.0001;

            //һ���������Ӧ�ĸ��������أ�mipmap=1;
            //mipmap���� = 0.5 * log_2(һ�������������������) ;
            //һ������������������� = ÿ���������� / (�������� * �÷����������)
            // = 6 * res * res / (4 * PI * SAMPLE_COUNT * pdf)��
            //ÿ����ƽ������ǣ���ǰ��������ĸ���
            float resolution = 512.0; // ԭ�ռ�������� (per face)
            float TexPerSample = 4 * resolution * resolution / (6 * PI * SAMPLE_COUNT * pdf);

            float mipLevel = ( roughness == 0.0 ? 0.0 : 0.5 * log2(TexPerSample) );

            //��Ȩ
            prefilteredColor += textureLod(environmentMap, L, mipLevel).rgb * NdotL;
//            prefilteredColor += texture(environmentMap, L).rgb * NdotL;
            totalWeight      += NdotL;

//            prefilteredColor += texture(environmentMap, L).rgb;
//            totalWeight      += 1.0;
        }
    }

    prefilteredColor = prefilteredColor / totalWeight;
    FragColor = vec4(prefilteredColor, 1.0);
}


/*
 *  ���룺һ������
 *  �����������������Ƶķ�תС��
 *
 */
float RadicalInverse_VdC(uint bits)
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
/*
 *  ���룺
 *      i: �������
 *      N: ��������
 *  �����
 *      vec2�����ȷֲ���α�����ά������ֵ��Ϊ{[0,1],[0,1]};
 */
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}


/*
 *  ���룺
 *      Xi: �����ά����
 *      N: ������
 *      roughness: �ֲڶ�
 *  �����
 *      vec3���������ڵĲ�������
 *
 *  ���ܣ�
 *      ��α�����ά����ת��Ϊ��������������roughness�����Ƿ�ƫ�������ķ��ߣ���
*       ֮�󽫰�������ת�Ƶ���������N�£�����ò�������
 */

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
	float a = roughness*roughness;

	float phi = 2.0 * PI * Xi.x;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

	// from spherical coordinates to cartesian coordinates - halfway vector
	vec3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;

	// from tangent-space H vector to world-space sample vector
	vec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent   = normalize(cross(up, N));
	vec3 bitangent = cross(N, tangent);

	vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
	return normalize(sampleVec);
}


/*
 *  ���룺
 *      N: ��۱��淨��
 *      H: ΢�۱��淨��
 *      roughness: �ֲڶ�
 *  �����
 *      float����΢���淽���ڵ�λ����ķ���ռ��
 *
 */
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

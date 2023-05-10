#version 450 core

//输入:预滤波方向
in vec3 WorldPos;
out vec4 FragColor;
//主要参数：
uniform samplerCube environmentMap;//环境立方体贴图
uniform float roughness;//控制预滤波的模糊程度

//辅助参数:
uniform uint sample_count;//每方向采样数
//uniform bool enMapHasMipmap;//是否有mipmap
//uniform int environmentMapSize;//环境立方体贴图大小

//辅助函数
float RadicalInverse_VdC(uint bits);
vec2 Hammersley(uint i, uint N);
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness);
float DistributionGGX(vec3 N, vec3 H, float roughness);

//const
const float PI = 3.1415926535;


void main(void)
{
    vec3 N = normalize(WorldPos);

    //采样数可设为Uniform
    //const uint SAMPLE_COUNT = 1024u;
    const uint SAMPLE_COUNT = 1024u;
    //采样结果保存
    vec3 prefilteredColor = vec3(0.0);
    float totalWeight = 0.0;

    //重要性采样
    for(uint i = 0;i<SAMPLE_COUNT;++i){
        //得到采样方向
        vec2 randomVec2 = Hammersley(i,SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(randomVec2,N,roughness);
        vec3 L = normalize(2.0 * dot(N, H) * H - N);

        //问题：该反射方向可能会向物体背部反射,所以要去除背向光线
        //那为何不直接使用H作为反射方向，这样可以避免光线的失效，增加采样数，减少L的计算时间
        float NdotL = max(dot(N,L),0.0f);
        if(NdotL > 0.0f){
            //计算采样mipmap
            float D = DistributionGGX(N,H,roughness);
            float pdf = D / 4.0 + 0.0001;

            //一个采样点对应四个采样像素，mipmap=1;
            //mipmap级别 = 0.5 * log_2(一个采样点采样的像素数) ;
            //一个采样点采样的像素数 = 每方向像素数 / (采样数量 * 该方向采样概率)
            // = 6 * res * res / (4 * PI * SAMPLE_COUNT * pdf)；
            //每像素平均立体角，当前采样方向的概率
            float resolution = 512.0; // 原空间盒清晰度 (per face)
            float TexPerSample = 4 * resolution * resolution / (6 * PI * SAMPLE_COUNT * pdf);

            float mipLevel = ( roughness == 0.0 ? 0.0 : 0.5 * log2(TexPerSample) );

            //加权
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
 *  输入：一个整数
 *  输出：这个整数二进制的翻转小数
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
 *  输入：
 *      i: 采样编号
 *      N: 采样总数
 *  输出：
 *      vec2：均匀分布的伪随机二维向量。值域为{[0,1],[0,1]};
 */
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}


/*
 *  输入：
 *      Xi: 随机二维向量
 *      N: 法向量
 *      roughness: 粗糙度
 *  输出：
 *      vec3：半球方向内的采样向量
 *
 *  功能：
 *      将伪随机二维向量转化为半球向量（根据roughness觉得是否偏向于中心法线），
*       之后将半球向量转移到世界坐标N下，输出该采样方向
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
 *  输入：
 *      N: 宏观表面法线
 *      H: 微观表面法线
 *      roughness: 粗糙度
 *  输出：
 *      float：该微表面方向在单位区域的法线占比
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

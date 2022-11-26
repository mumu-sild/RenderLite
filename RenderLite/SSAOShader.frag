#version 450 core
out vec3 FragColor;
in vec2 TexCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];
uniform float width,height;

// parameters
int kernelSize = 64;
float radius = 3000.0;

// tile noise texture over screen based on screen dimensions divided by noise size
const vec2 noiseScale = vec2(width/4.0f, height/4.0f);

uniform mat4 projection;
uniform mat4 view;
uniform bool whichDepth;

const float near_plane = 0.001f;
const float far_plane = 1000.0f;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane))/far_plane;
}

void main()
{
    // Get input for SSAO algorithm
    vec3 fragPos = texture(gPositionDepth, TexCoords).xyz;//视口坐标
    vec3 normal = texture(gNormal, TexCoords).rgb;//视口坐标的normal
    //vec3 randomVec = texture(texNoise, TexCoords).xyz;
    vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz;
    randomVec = normalize(randomVec);
    // Create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    // Iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;

    // project sample position (to sample texture) (to get position on screen/texture)
//    vec4 offset = vec4(fragPos, 1.0);
//    offset = projection * offset; //屏幕坐标采样点位置
//    offset.xyz /= offset.w; // perspective divide
//    offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
//    offset.z = LinearizeDepth(offset.z);
//    if(whichDepth) FragColor = vec3(offset.z);
//    else FragColor = vec3(texture(gPositionDepth, TexCoords).w)/far_plane;
//    return;

    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 Tsample = TBN * samples[i]; // From tangent to view-space
        Tsample = fragPos + Tsample * radius;//视口坐标中的采样点位置

        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(Tsample, 1.0);
        offset = projection * offset; //屏幕坐标采样点位置
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        offset.z = LinearizeDepth(offset.z);

        //采样像素记录的深度
        float sampleDepth = texture(gPositionDepth, offset.xy).w/far_plane; // 屏幕坐标下的采样点深度

        //视口坐标下z轴坐标 - projection视角下坐标
//        if(offset.z - sampleDepth==0.0f){
//                FragColor = vec3(1,1,1);
//                return;
//        }
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(offset.z - sampleDepth ));
        occlusion += (sampleDepth >= offset.z ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / kernelSize);//遮蔽系数，遮蔽越多，颜色越白

    FragColor = vec3(occlusion,occlusion,occlusion);
}

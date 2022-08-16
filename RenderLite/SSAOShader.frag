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
float radius = 10.0;

// tile noise texture over screen based on screen dimensions divided by noise size
const vec2 noiseScale = vec2(width/4.0f, height/4.0f);

uniform mat4 projection;



void main()
{
//    FragColor = texture(gPositionDepth, TexCoords).xyz;
//    return ;
    // Get input for SSAO algorithm
    vec3 fragPos = texture(gPositionDepth, TexCoords).xyz;//世界坐标
    vec3 normal = texture(gNormal, TexCoords).rgb;
    vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz;
    // Create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    // Iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;

    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 Tsample = TBN * samples[i]; // From tangent to view-space
        Tsample = fragPos + Tsample * radius;//世界坐标中的采样点位置

        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(Tsample, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        // get sample depth
        float sampleDepth = -texture(gPositionDepth, offset.xy).w; // Get depth value of kernel sample

        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth ));
        occlusion += (sampleDepth >= Tsample.z ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / kernelSize);//遮蔽系数，遮蔽越多，颜色越白

    FragColor = vec3(occlusion);
}

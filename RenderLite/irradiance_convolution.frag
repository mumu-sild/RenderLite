#version 450 core
out vec4 FragColor;
in vec3 WorldPos;

layout(binding = 0)uniform samplerCube environmentMap;

const float PI = 3.14159265359;

void main()
{
    // The world vector acts as the normal of a tangent surface
    // from the origin, aligned to WorldPos. Given this normal, calculate all
    // incoming radiance of the environment. The result of this radiance
    // is the radiance of light coming from -Normal direction, which is what
    // we use in the PBR shader to sample irradiance.
    vec3 N = normalize(WorldPos);

    vec3 irradiance = vec3(0.0);

    // tangent space calculation from origin point
    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up         = normalize(cross(N, right));

     //采样点采集
    float sampleDelta = 0.025;
    float nrSamples = 0.0;
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)//cos角度
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)//sin角度
        {
            // 球面到笛卡尔坐标(切线空间)
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // 切线空间到世界空间
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;
            //乘以系数 cos(θ) ，因为较大角度的光较弱，而系数 sin(θ) 则用于权衡较高半球区域的较小采样区域的贡献度。
            irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));

    FragColor = vec4(irradiance, 1.0);
}

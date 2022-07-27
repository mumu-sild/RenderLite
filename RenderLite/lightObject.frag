#version 450 core

struct Material {
        sampler2D texture_diffuse1;
        sampler2D texture_specular1;
        float shiness;
};


struct DirLight {
    bool Activated;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    vec3 lightnormal;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

//顶点信息
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

in vec4 FragPosLightSpace;
uniform sampler2D shadowMap;
//输出
out vec4 FragColor;
//视点
uniform vec3 viewPos;
//平行光
uniform DirLight dirLight;

//点光源
uniform PointLight pointLights[16];
uniform int numPointLights;

uniform Material material;

#define PI 3.141592653589793
#define PI2 6.283185307179586

//采样数
#define NUM_SAMPLES 30
//采样圈数
#define NUM_RINGS 10

//函数申明
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light,vec3 normal, vec3 fragPos,vec3 viewDir);
float PCF(vec3 projCoords,int r);
float PCSS(vec3 projCoords);
float averageBlockDep(vec3 projCoords,vec2 texelSize);
void poissonDiskSamples(const in vec2 randomSeed);

//全局参数
vec2 poissonDisk[NUM_SAMPLES];

highp float rand_2to1(vec2 uv ) {//传入一个二维数，传出一个假随机数。
    // 0 - 1
    const highp float a = 12.9898, b = 78.233, c = 43758.5453;
    highp float dt = dot( uv.xy, vec2( a,b ) );
    highp float sn = mod( dt, PI );
    return fract(sin(sn) * c);//只取小数部分（取值范围0~1，若为负+1）
}

void poissonDiskSamples(const in vec2 randomSeed){
    float ANGLE_STEP = PI2 * float(NUM_RINGS)/float( NUM_SAMPLES);//角位移大小
    float INV_NUM_SAMPLES = 1.0 / float(NUM_SAMPLES); //采样数的倒数

    float angle = rand_2to1(randomSeed) * PI2;//初始角度（弧度）
    float radius = INV_NUM_SAMPLES;//初始半径
    float radiusStep = radius;     //半径增量

    for( int i = 0; i < NUM_SAMPLES; i ++ ) {
      poissonDisk[i] = vec2( cos( angle ), sin( angle ) ) * pow( radius, 0.75 );
      radius += radiusStep;//半径增加
      angle += ANGLE_STEP;//弧度增加
    }
}

void main()
{
//    //test1
//    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;//当前点在光源视角下的坐标
//    // 变换到[0,1]的范围
//    projCoords = projCoords * 0.5 + 0.5;
//    projCoords.z = texture(shadowMap, projCoords.xy).r;

//    FragColor = vec4(projCoords.z, projCoords.z, projCoords.z, 1.0);
//    return;


    // properties
       vec3 norm = normalize(Normal);
       vec3 viewDir = normalize(viewPos - FragPos);
       vec3 result = vec3(0,0,0);
       // phase 1: parallel lights
       if(dirLight.Activated){
            result += CalcDirLight(dirLight, norm, viewDir);
       }
       // phase 2: point lights
       for(int i = 0; i < numPointLights; i++){
            result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
       }
       FragColor = vec4(result,1.0);
}

float averageBlockDep(vec3 projCoords,vec2 texelSize){
    float blockerZ = 0.0;//遮挡物总深度
    int count = 0;
    int r=5;
    //在一定范围内判断是否有遮挡物
//    for(int x = -r; x < r; x++){
//        for(int y = -r; y < r; y++){
//            float depth = texture(shadowMap,projCoords.xy+vec2(x,y) * texelSize).r;
//            if(depth < projCoords.z){//如果为遮挡物
//                count++;
//                blockerZ +=depth;
//            }
//        }
//    }

    poissonDiskSamples(projCoords.xy+vec2(0.1314,0.351));
    for(int i=0;i<NUM_SAMPLES;++i){
        float depth = texture(shadowMap, projCoords.xy + r * poissonDisk[i] * texelSize).r;
        if(depth < projCoords.z){//如果为遮挡物
            count++;
            blockerZ +=depth;
        }
    }


    if(count == 0||count==(r*2+1)*(r*2+1))return 1.0f;
    return blockerZ / count;
}

float PCSS(vec3 projCoords){
    const float weightOfLight = 10.0;

    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片段是否在阴影中
    float bias = max(0.05 * (1.0 - dot(Normal, -dirLight.direction)), 0.005);
    //每像素偏移距离
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    //PCSS核心算法
    float visibility = 0.0;
    //第一步计算平均遮挡物深度
    float averBlocker = averageBlockDep(projCoords,texelSize);
    //第二步，计算半影半径
    float penumbra = (projCoords.z - averBlocker) * weightOfLight / averBlocker;
    //第三步 PCF
    visibility = PCF(projCoords,int(penumbra));

    return visibility;
}

float PCF(vec3 projCoords,int r)
{
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片段是否在阴影中
    float bias = max(0.05 * (1.0 - dot(Normal, -dirLight.direction)), 0.005);

    //PCF
    float shadow = 0.0;
    //const float lightWidth = 5;
    //float dis = (currentDepth - closestDepth)/closestDepth * lightWidth;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);//每像素偏移距离
//    for(int x = -r; x <= r; ++x)
//    {
//        for(int y = -r; y <= r; ++y)
//        {
//            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
//            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
//        }
//    }
//    shadow /= (2*r+1)*(2*r+1);

    poissonDiskSamples(projCoords.xy);
    for(int i=0;i<NUM_SAMPLES;i++){
        float pcfDepth = texture(shadowMap, projCoords.xy + r * poissonDisk[i] * texelSize).r;
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
    shadow /= float(NUM_SAMPLES);

    //shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    //shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    //远平面矫正
    if(projCoords.z > 1.0) shadow = 0.0;

    return shadow;
}

//float ShadowCalculation(vec4 fragPosLightSpace)
//{
//    // 执行透视除法
//    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
//    // 变换到[0,1]的范围
//    projCoords = projCoords * 0.5 + 0.5;
//    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
//    float closestDepth = texture(shadowMap, projCoords.xy).r;
//    // 取得当前片段在光源视角下的深度
//    float currentDepth = projCoords.z;
//    // 检查当前片段是否在阴影中
//    float bias = max(0.05 * (1.0 - dot(Normal, -dirLight.direction)), 0.005);
//    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
//    //float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

//    return shadow;
//}

//计算平行光源
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){
        //平行光反方向
        vec3 lightDir = normalize(-light.direction);
        //计算cos衰减
        float diff = max(dot(lightDir,normal),0.0);
        //反射方向
        vec3 reflectDir = reflect(-lightDir,normal);
        //计算镜面反射系数
        float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shiness);

        vec3 diffusecolor = vec3(texture2D(material.texture_diffuse1,TexCoords));
        vec3 ambient = light.ambient * diffusecolor;
        vec3 diffuse = light.diffuse * diff * diffusecolor;
        vec3 specular = light.specular * spec * vec3(texture2D(material.texture_specular1,TexCoords));


        // 执行透视除法
        vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
        // 变换到[0,1]的范围
        projCoords = projCoords * 0.5 + 0.5;
        // 计算阴影
        float shadow = PCSS(projCoords);
        //float shadow = PCF(projCoords,1);
        //float shadow = ShadowCalculation(FragPosLightSpace);
        //float shadow = 0.0;
        vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));
        return lighting;
}

vec3 CalcPointLight(PointLight light,vec3 normal, vec3 fragPos,vec3 viewDir){
        vec3 lightDir = normalize(light.position - fragPos);

        float angleDecay = 1.0f;
        if(any(notEqual(light.lightnormal,vec3(0,0,0)))){
            angleDecay = max(dot(-lightDir,normalize(light.lightnormal)),0.0f);
        }
        float diff = max(dot(lightDir,normal),0.0);
        vec3 reflectDir = reflect(-lightDir,normal);
        float spec = pow(max(dot(reflectDir,viewDir),0.0),material.shiness);

        float distance = length(light.position - fragPos);
        float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * (distance * distance));

        vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1,TexCoords));
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1,TexCoords));
        vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1,TexCoords));

        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;

        ambient *= angleDecay;
        diffuse *= angleDecay;
        specular *= angleDecay;
        return (ambient + diffuse + specular);
}

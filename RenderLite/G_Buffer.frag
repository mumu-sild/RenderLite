#version 450 core

//输出
layout (location = 0) out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gBrightColor;
uniform sampler2D SSAO;


struct DirLight {
    bool Activated;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    //阴影
    sampler2D shadowMap;
    mat4 lightSpaceMatrix;

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

    //阴影
    sampler2D shadowMap;
    mat4 lightSpaceMatrix;
    float width;
};

//视口坐标矩阵
uniform vec3 viewPos;
//平行光
uniform DirLight dirLight;

//点光源
uniform PointLight pointLights[16];
uniform int numPointLights;

//材质
uniform float shiness;

//光照
uniform bool blinn;
//色调映射
uniform float toneMapping;
//gamma
uniform bool gamma;


//输入信息
vec3 Normal;
vec3 FragPos;
vec3 diffusecolor;
vec3 specularcolor;
float depth;
float ambientOcclusion = 1;

//常量
const float PI = 3.141592653589793;
const float PI2 = 6.283185307179586;
float near_plane = 0.5f;
float far_plane = 100.5f;

//采样数
const int NUM_SAMPLES = 30;

//采样圈数
const int NUM_RINGS = 10;

//函数申明
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light,vec3 normal, vec3 fragPos,vec3 viewDir);
float PCF(vec3 projCoords,int r,sampler2D shadowMap);
float PCSS(vec3 projCoords,sampler2D shadowMap,float weightOfLight);
float averageBlockDep(vec3 projCoords,vec2 texelSize,sampler2D shadowMap);
void poissonDiskSamples(const in vec2 randomSeed);


float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane))/far_plane;
}

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
    //获取G—Buffer信息
    FragPos = texture2D(gPosition,TexCoords).rgb;//世界坐标信息
    Normal = texture2D(gNormal,TexCoords).rgb;
    diffusecolor = texture2D(gAlbedoSpec,TexCoords).rgb;
    specularcolor = diffusecolor * texture2D(gAlbedoSpec,TexCoords).a;
    ambientOcclusion = 1.0f - texture(SSAO, TexCoords).r;


    //反gamma矫正
    diffusecolor = pow(diffusecolor, vec3(2.2));
    specularcolor = pow(specularcolor , vec3(2.2));

    // 属性
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos-FragPos);//只修改这里
    vec3 result = vec3(0,0,0);
    // 平行光
    if(dirLight.Activated){
        result += CalcDirLight(dirLight, norm, viewDir);
    }
    // 点光源
    for(int i = 0; i < numPointLights; i++){
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }
    //色调映射
    if(toneMapping>0.0f){
        //result.rgb = result.rgb /(result.rgb+ vec3(1.0));
        result.rgb = vec3(1.0) - exp(-result.rgb * toneMapping);
    }


    //gamma矫正
    float gamma_ = 2.2;
    if(gamma){
        result.rgb = pow(result.rgb, vec3(1.0/gamma_));
    }
    //泛光
    result += texture2D(gBrightColor,TexCoords).rgb;
    FragColor = vec4(result,1.0);
}

float averageBlockDep(vec3 projCoords,vec2 texelSize,sampler2D shadowMap){
    float blockerZ = 0.0;//遮挡物总深度
    int count = 0;
    int r=5;

    poissonDiskSamples(projCoords.xy+vec2(0.1314,0.351));
    for(int i=0;i<NUM_SAMPLES;++i){
        float depth = texture2D(shadowMap, projCoords.xy + r * poissonDisk[i] * texelSize).r;
        if(depth < projCoords.z){//如果为遮挡物
            count++;
            blockerZ +=depth;
        }
    }


    if(count == 0||count==(r*2+1)*(r*2+1))return 1.0f;
    return blockerZ / count;
}

float PCSS(vec3 projCoords,sampler2D shadowMap,float weightOfLight){

    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    //float closestDepth = texture(shadowMap, projCoords.xy).r;
    // 取得当前片段在光源视角下的深度
    //float currentDepth = projCoords.z;
    // 检查当前片段是否在阴影中
    //float bias = max(0.05 * (1.0 - dot(Normal, -dirLight.direction)), 0.005);
    //每像素偏移距离
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    //PCSS核心算法
    float visibility = 0.0;
    //第一步计算平均遮挡物深度
    float averBlocker = averageBlockDep(projCoords,texelSize,shadowMap);
    //第二步，计算半影半径
    float penumbra = (projCoords.z - averBlocker) * weightOfLight / averBlocker;
    //第三步 PCF
    visibility = PCF(projCoords,int(penumbra),shadowMap);

    return visibility;
}

float PCF(vec3 projCoords,int r,sampler2D shadowMap)
{
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    //float closestDepth = texture(shadowMap, projCoords.xy).r;
    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片段是否在阴影中
    //float bias = max(0.05 * (1.0 - dot(Normal, -dirLight.ViewDirection)), 0.005);//?????

    //PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);//每像素偏移距离

    poissonDiskSamples(projCoords.xy);
    for(int i=0;i<NUM_SAMPLES;i++){
        float pcfDepth = texture(shadowMap, projCoords.xy + r * poissonDisk[i] * texelSize).r;
        shadow += currentDepth -  0.01f > pcfDepth ? 1.0 : 0.0;//bias
    }
    shadow /= float(NUM_SAMPLES);

    //远平面矫正
    if(projCoords.z > 1.0) shadow = 0.0;

    return shadow;
}

//计算平行光源
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){
        vec3 lightDir = -normalize(light.direction);//平行光反方向
        float diff = max(dot(lightDir,normal),0.0);//cos衰减系数
        float spec = 0.0;//反射系数
        if(blinn){
            vec3 halfwayDir = normalize(viewDir+lightDir);//半程向量
            spec = pow(max(dot(normal,halfwayDir),0.0),shiness*4);
        }
        else{
            vec3 reflectDir = reflect(-lightDir,normal); //反射方向
            spec = pow(max(dot(viewDir,reflectDir),0.0),shiness);//计算镜面反射系数
        }

        vec3 ambient = light.ambient * ambientOcclusion * diffusecolor;
        vec3 diffuse = light.diffuse * diff * diffusecolor;
        vec3 specular = light.specular * spec * specularcolor;

        //阴影计算
        float shadow = 0.0;
        {
            //光视角的点位置
            vec4 FragPosLightSpace = light.lightSpaceMatrix * vec4(FragPos, 1.0);
            // 执行透视除法
            vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
            // 变换到[0,1]的范围
            projCoords = projCoords * 0.5 + 0.5;
            // 计算阴影
            shadow = PCSS(projCoords,light.shadowMap,5);
            //shadow = PCF(projCoords,1,light.shadowMap);
        }

        vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));
        return lighting;
}

vec3 CalcPointLight(PointLight light,vec3 normal, vec3 fragPos,vec3 viewDir){
        vec3 lightDir = normalize(light.position - fragPos);//片元指向光源

        float angleDecay = 0.0f;
        if(any(notEqual(light.lightnormal,vec3(0,0,0)))){//不是（0，0，0）
            angleDecay = max(dot(-lightDir,normalize(light.lightnormal)),0.0f);
        }

        float diff = max(dot(lightDir,normal),0.0);
        float spec = 0.0;//反射系数
        if(blinn){
            vec3 halfwayDir = normalize(viewDir+lightDir);//半程向量
            spec = pow(max(dot(normal,halfwayDir),0.0),shiness*4);
        }
        else{
            vec3 reflectDir = reflect(-lightDir,normal); //反射方向
            spec = pow(max(dot(viewDir,reflectDir),0.0),shiness);//计算镜面反射系数
        }

        float distance = length(light.position - fragPos);
        float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * (distance * distance));

        vec3 ambient = light.ambient * ambientOcclusion * diffusecolor;
        vec3 diffuse = light.diffuse * diff * diffusecolor;
        vec3 specular = light.specular * spec * specularcolor;

        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;

        diffuse *= angleDecay;
        specular *= angleDecay;

        //阴影计算
        float shadow = 0.0;
        {
            //光视角的点位置
            vec4 FragPosLightSpace = light.lightSpaceMatrix * vec4(FragPos, 1.0);
            // 执行透视除法
            vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
            // 变换到[0,1]的范围
            projCoords = projCoords * 0.5 + 0.5;
            //转化为线性深度
            projCoords.z = LinearizeDepth(projCoords.z);

            // 计算阴影
            shadow = PCSS(projCoords,light.shadowMap,light.width);
            //shadow = PCF(projCoords,1,light.shadowMap);
        }

        vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));

        return lighting;
}

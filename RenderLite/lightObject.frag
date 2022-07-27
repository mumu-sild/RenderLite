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

//������Ϣ
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

in vec4 FragPosLightSpace;
uniform sampler2D shadowMap;
//���
out vec4 FragColor;
//�ӵ�
uniform vec3 viewPos;
//ƽ�й�
uniform DirLight dirLight;

//���Դ
uniform PointLight pointLights[16];
uniform int numPointLights;

uniform Material material;

#define PI 3.141592653589793
#define PI2 6.283185307179586

//������
#define NUM_SAMPLES 30
//����Ȧ��
#define NUM_RINGS 10

//��������
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light,vec3 normal, vec3 fragPos,vec3 viewDir);
float PCF(vec3 projCoords,int r);
float PCSS(vec3 projCoords);
float averageBlockDep(vec3 projCoords,vec2 texelSize);
void poissonDiskSamples(const in vec2 randomSeed);

//ȫ�ֲ���
vec2 poissonDisk[NUM_SAMPLES];

highp float rand_2to1(vec2 uv ) {//����һ����ά��������һ�����������
    // 0 - 1
    const highp float a = 12.9898, b = 78.233, c = 43758.5453;
    highp float dt = dot( uv.xy, vec2( a,b ) );
    highp float sn = mod( dt, PI );
    return fract(sin(sn) * c);//ֻȡС�����֣�ȡֵ��Χ0~1����Ϊ��+1��
}

void poissonDiskSamples(const in vec2 randomSeed){
    float ANGLE_STEP = PI2 * float(NUM_RINGS)/float( NUM_SAMPLES);//��λ�ƴ�С
    float INV_NUM_SAMPLES = 1.0 / float(NUM_SAMPLES); //�������ĵ���

    float angle = rand_2to1(randomSeed) * PI2;//��ʼ�Ƕȣ����ȣ�
    float radius = INV_NUM_SAMPLES;//��ʼ�뾶
    float radiusStep = radius;     //�뾶����

    for( int i = 0; i < NUM_SAMPLES; i ++ ) {
      poissonDisk[i] = vec2( cos( angle ), sin( angle ) ) * pow( radius, 0.75 );
      radius += radiusStep;//�뾶����
      angle += ANGLE_STEP;//��������
    }
}

void main()
{
//    //test1
//    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;//��ǰ���ڹ�Դ�ӽ��µ�����
//    // �任��[0,1]�ķ�Χ
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
    float blockerZ = 0.0;//�ڵ��������
    int count = 0;
    int r=5;
    //��һ����Χ���ж��Ƿ����ڵ���
//    for(int x = -r; x < r; x++){
//        for(int y = -r; y < r; y++){
//            float depth = texture(shadowMap,projCoords.xy+vec2(x,y) * texelSize).r;
//            if(depth < projCoords.z){//���Ϊ�ڵ���
//                count++;
//                blockerZ +=depth;
//            }
//        }
//    }

    poissonDiskSamples(projCoords.xy+vec2(0.1314,0.351));
    for(int i=0;i<NUM_SAMPLES;++i){
        float depth = texture(shadowMap, projCoords.xy + r * poissonDisk[i] * texelSize).r;
        if(depth < projCoords.z){//���Ϊ�ڵ���
            count++;
            blockerZ +=depth;
        }
    }


    if(count == 0||count==(r*2+1)*(r*2+1))return 1.0f;
    return blockerZ / count;
}

float PCSS(vec3 projCoords){
    const float weightOfLight = 10.0;

    // ȡ�����������(ʹ��[0,1]��Χ�µ�fragPosLight������)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // ȡ�õ�ǰƬ���ڹ�Դ�ӽ��µ����
    float currentDepth = projCoords.z;
    // ��鵱ǰƬ���Ƿ�����Ӱ��
    float bias = max(0.05 * (1.0 - dot(Normal, -dirLight.direction)), 0.005);
    //ÿ����ƫ�ƾ���
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    //PCSS�����㷨
    float visibility = 0.0;
    //��һ������ƽ���ڵ������
    float averBlocker = averageBlockDep(projCoords,texelSize);
    //�ڶ����������Ӱ�뾶
    float penumbra = (projCoords.z - averBlocker) * weightOfLight / averBlocker;
    //������ PCF
    visibility = PCF(projCoords,int(penumbra));

    return visibility;
}

float PCF(vec3 projCoords,int r)
{
    // ȡ�����������(ʹ��[0,1]��Χ�µ�fragPosLight������)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // ȡ�õ�ǰƬ���ڹ�Դ�ӽ��µ����
    float currentDepth = projCoords.z;
    // ��鵱ǰƬ���Ƿ�����Ӱ��
    float bias = max(0.05 * (1.0 - dot(Normal, -dirLight.direction)), 0.005);

    //PCF
    float shadow = 0.0;
    //const float lightWidth = 5;
    //float dis = (currentDepth - closestDepth)/closestDepth * lightWidth;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);//ÿ����ƫ�ƾ���
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

    //Զƽ�����
    if(projCoords.z > 1.0) shadow = 0.0;

    return shadow;
}

//float ShadowCalculation(vec4 fragPosLightSpace)
//{
//    // ִ��͸�ӳ���
//    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
//    // �任��[0,1]�ķ�Χ
//    projCoords = projCoords * 0.5 + 0.5;
//    // ȡ�����������(ʹ��[0,1]��Χ�µ�fragPosLight������)
//    float closestDepth = texture(shadowMap, projCoords.xy).r;
//    // ȡ�õ�ǰƬ���ڹ�Դ�ӽ��µ����
//    float currentDepth = projCoords.z;
//    // ��鵱ǰƬ���Ƿ�����Ӱ��
//    float bias = max(0.05 * (1.0 - dot(Normal, -dirLight.direction)), 0.005);
//    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
//    //float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

//    return shadow;
//}

//����ƽ�й�Դ
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){
        //ƽ�йⷴ����
        vec3 lightDir = normalize(-light.direction);
        //����cos˥��
        float diff = max(dot(lightDir,normal),0.0);
        //���䷽��
        vec3 reflectDir = reflect(-lightDir,normal);
        //���㾵�淴��ϵ��
        float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shiness);

        vec3 diffusecolor = vec3(texture2D(material.texture_diffuse1,TexCoords));
        vec3 ambient = light.ambient * diffusecolor;
        vec3 diffuse = light.diffuse * diff * diffusecolor;
        vec3 specular = light.specular * spec * vec3(texture2D(material.texture_specular1,TexCoords));


        // ִ��͸�ӳ���
        vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
        // �任��[0,1]�ķ�Χ
        projCoords = projCoords * 0.5 + 0.5;
        // ������Ӱ
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

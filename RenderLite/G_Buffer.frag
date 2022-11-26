#version 450 core

//���
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
    //��Ӱ
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

    //��Ӱ
    sampler2D shadowMap;
    mat4 lightSpaceMatrix;
    float width;
};

//�ӿ��������
uniform vec3 viewPos;
//ƽ�й�
uniform DirLight dirLight;

//���Դ
uniform PointLight pointLights[16];
uniform int numPointLights;

//����
uniform float shiness;

//����
uniform bool blinn;
//ɫ��ӳ��
uniform float toneMapping;
//gamma
uniform bool gamma;


//������Ϣ
vec3 Normal;
vec3 FragPos;
vec3 diffusecolor;
vec3 specularcolor;
float depth;
float ambientOcclusion = 1;

//����
const float PI = 3.141592653589793;
const float PI2 = 6.283185307179586;
float near_plane = 0.5f;
float far_plane = 100.5f;

//������
const int NUM_SAMPLES = 30;

//����Ȧ��
const int NUM_RINGS = 10;

//��������
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
    //��ȡG��Buffer��Ϣ
    FragPos = texture2D(gPosition,TexCoords).rgb;//����������Ϣ
    Normal = texture2D(gNormal,TexCoords).rgb;
    diffusecolor = texture2D(gAlbedoSpec,TexCoords).rgb;
    specularcolor = diffusecolor * texture2D(gAlbedoSpec,TexCoords).a;
    ambientOcclusion = 1.0f - texture(SSAO, TexCoords).r;


    //��gamma����
    diffusecolor = pow(diffusecolor, vec3(2.2));
    specularcolor = pow(specularcolor , vec3(2.2));

    // ����
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos-FragPos);//ֻ�޸�����
    vec3 result = vec3(0,0,0);
    // ƽ�й�
    if(dirLight.Activated){
        result += CalcDirLight(dirLight, norm, viewDir);
    }
    // ���Դ
    for(int i = 0; i < numPointLights; i++){
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }
    //ɫ��ӳ��
    if(toneMapping>0.0f){
        //result.rgb = result.rgb /(result.rgb+ vec3(1.0));
        result.rgb = vec3(1.0) - exp(-result.rgb * toneMapping);
    }


    //gamma����
    float gamma_ = 2.2;
    if(gamma){
        result.rgb = pow(result.rgb, vec3(1.0/gamma_));
    }
    //����
    result += texture2D(gBrightColor,TexCoords).rgb;
    FragColor = vec4(result,1.0);
}

float averageBlockDep(vec3 projCoords,vec2 texelSize,sampler2D shadowMap){
    float blockerZ = 0.0;//�ڵ��������
    int count = 0;
    int r=5;

    poissonDiskSamples(projCoords.xy+vec2(0.1314,0.351));
    for(int i=0;i<NUM_SAMPLES;++i){
        float depth = texture2D(shadowMap, projCoords.xy + r * poissonDisk[i] * texelSize).r;
        if(depth < projCoords.z){//���Ϊ�ڵ���
            count++;
            blockerZ +=depth;
        }
    }


    if(count == 0||count==(r*2+1)*(r*2+1))return 1.0f;
    return blockerZ / count;
}

float PCSS(vec3 projCoords,sampler2D shadowMap,float weightOfLight){

    // ȡ�����������(ʹ��[0,1]��Χ�µ�fragPosLight������)
    //float closestDepth = texture(shadowMap, projCoords.xy).r;
    // ȡ�õ�ǰƬ���ڹ�Դ�ӽ��µ����
    //float currentDepth = projCoords.z;
    // ��鵱ǰƬ���Ƿ�����Ӱ��
    //float bias = max(0.05 * (1.0 - dot(Normal, -dirLight.direction)), 0.005);
    //ÿ����ƫ�ƾ���
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    //PCSS�����㷨
    float visibility = 0.0;
    //��һ������ƽ���ڵ������
    float averBlocker = averageBlockDep(projCoords,texelSize,shadowMap);
    //�ڶ����������Ӱ�뾶
    float penumbra = (projCoords.z - averBlocker) * weightOfLight / averBlocker;
    //������ PCF
    visibility = PCF(projCoords,int(penumbra),shadowMap);

    return visibility;
}

float PCF(vec3 projCoords,int r,sampler2D shadowMap)
{
    // ȡ�����������(ʹ��[0,1]��Χ�µ�fragPosLight������)
    //float closestDepth = texture(shadowMap, projCoords.xy).r;
    // ȡ�õ�ǰƬ���ڹ�Դ�ӽ��µ����
    float currentDepth = projCoords.z;
    // ��鵱ǰƬ���Ƿ�����Ӱ��
    //float bias = max(0.05 * (1.0 - dot(Normal, -dirLight.ViewDirection)), 0.005);//?????

    //PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);//ÿ����ƫ�ƾ���

    poissonDiskSamples(projCoords.xy);
    for(int i=0;i<NUM_SAMPLES;i++){
        float pcfDepth = texture(shadowMap, projCoords.xy + r * poissonDisk[i] * texelSize).r;
        shadow += currentDepth -  0.01f > pcfDepth ? 1.0 : 0.0;//bias
    }
    shadow /= float(NUM_SAMPLES);

    //Զƽ�����
    if(projCoords.z > 1.0) shadow = 0.0;

    return shadow;
}

//����ƽ�й�Դ
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){
        vec3 lightDir = -normalize(light.direction);//ƽ�йⷴ����
        float diff = max(dot(lightDir,normal),0.0);//cos˥��ϵ��
        float spec = 0.0;//����ϵ��
        if(blinn){
            vec3 halfwayDir = normalize(viewDir+lightDir);//�������
            spec = pow(max(dot(normal,halfwayDir),0.0),shiness*4);
        }
        else{
            vec3 reflectDir = reflect(-lightDir,normal); //���䷽��
            spec = pow(max(dot(viewDir,reflectDir),0.0),shiness);//���㾵�淴��ϵ��
        }

        vec3 ambient = light.ambient * ambientOcclusion * diffusecolor;
        vec3 diffuse = light.diffuse * diff * diffusecolor;
        vec3 specular = light.specular * spec * specularcolor;

        //��Ӱ����
        float shadow = 0.0;
        {
            //���ӽǵĵ�λ��
            vec4 FragPosLightSpace = light.lightSpaceMatrix * vec4(FragPos, 1.0);
            // ִ��͸�ӳ���
            vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
            // �任��[0,1]�ķ�Χ
            projCoords = projCoords * 0.5 + 0.5;
            // ������Ӱ
            shadow = PCSS(projCoords,light.shadowMap,5);
            //shadow = PCF(projCoords,1,light.shadowMap);
        }

        vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));
        return lighting;
}

vec3 CalcPointLight(PointLight light,vec3 normal, vec3 fragPos,vec3 viewDir){
        vec3 lightDir = normalize(light.position - fragPos);//ƬԪָ���Դ

        float angleDecay = 0.0f;
        if(any(notEqual(light.lightnormal,vec3(0,0,0)))){//���ǣ�0��0��0��
            angleDecay = max(dot(-lightDir,normalize(light.lightnormal)),0.0f);
        }

        float diff = max(dot(lightDir,normal),0.0);
        float spec = 0.0;//����ϵ��
        if(blinn){
            vec3 halfwayDir = normalize(viewDir+lightDir);//�������
            spec = pow(max(dot(normal,halfwayDir),0.0),shiness*4);
        }
        else{
            vec3 reflectDir = reflect(-lightDir,normal); //���䷽��
            spec = pow(max(dot(viewDir,reflectDir),0.0),shiness);//���㾵�淴��ϵ��
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

        //��Ӱ����
        float shadow = 0.0;
        {
            //���ӽǵĵ�λ��
            vec4 FragPosLightSpace = light.lightSpaceMatrix * vec4(FragPos, 1.0);
            // ִ��͸�ӳ���
            vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
            // �任��[0,1]�ķ�Χ
            projCoords = projCoords * 0.5 + 0.5;
            //ת��Ϊ�������
            projCoords.z = LinearizeDepth(projCoords.z);

            // ������Ӱ
            shadow = PCSS(projCoords,light.shadowMap,light.width);
            //shadow = PCF(projCoords,1,light.shadowMap);
        }

        vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));

        return lighting;
}

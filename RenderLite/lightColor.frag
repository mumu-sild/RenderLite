#version 450 core

struct Material {
    vec3 color;
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

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light,vec3 normal, vec3 fragPos,vec3 viewDir);
float ShadowCalculation(vec4 fragPosLightSpace);

void main()
{
       // properties
       vec3 norm = normalize(Normal);
       vec3 viewDir = normalize(viewPos - FragPos);//ƬԪ��ָ���ӵ�
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

float ShadowCalculation(vec4 fragPosLightSpace)
{


    // ִ��͸�ӳ���
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // �任��[0,1]�ķ�Χ
    projCoords = projCoords * 0.5 + 0.5;

    // ȡ�����������(ʹ��[0,1]��Χ�µ�fragPosLight������)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // ȡ�õ�ǰƬ���ڹ�Դ�ӽ��µ����
    float currentDepth = projCoords.z;
    // ��鵱ǰƬ���Ƿ�����Ӱ��
    float bias = max(0.05 * (1.0 - dot(Normal, -dirLight.direction)), 0.005);

    float shadow = 0.0;
    const float lightWidth = 5;
    float dis = (currentDepth - closestDepth)/closestDepth * lightWidth;
    int r = int(dis);
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -r; x <= r; ++x)
    {
        for(int y = -r; y <= r; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= (2*r+1)*(2*r+1);

    //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    //float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

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

    vec3 ambient = light.ambient * material.color;
    vec3 diffuse = light.diffuse * diff * material.color;
    vec3 specular = light.specular * spec * material.color;

    // ������Ӱ
    float shadow = ShadowCalculation(FragPosLightSpace);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));

    return lighting;
}

//������Դ
vec3 CalcPointLight(PointLight light,vec3 normal, vec3 fragPos,vec3 viewDir){
    //��Դ������
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

    vec3 ambient = light.ambient * material.color;
    vec3 diffuse = light.diffuse * diff * material.color;
    vec3 specular = light.specular * spec * material.color;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    ambient *= angleDecay;
    diffuse *= angleDecay;
    specular *= angleDecay;

    return (ambient + diffuse + specular);
}

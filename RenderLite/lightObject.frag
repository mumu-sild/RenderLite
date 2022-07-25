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


in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

in vec4 FragPosLightSpace;
uniform sampler2D shadowMap;

out vec4 FragColor;

uniform vec3 viewPos;

uniform DirLight dirLight;

uniform int numPointLights;
uniform PointLight pointLights[16];

uniform Material material;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light,vec3 normal, vec3 fragPos,vec3 viewDir);
float ShadowCalculation(vec4 fragPosLightSpace);

void main()
{
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

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片段是否在阴影中
    float bias = max(0.05 * (1.0 - dot(Normal, -dirLight.direction)), 0.005);
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    //float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){
        vec3 lightDir = normalize(-light.direction);
        float diff = max(dot(lightDir,normal),0.0);

        vec3 reflectDir = reflect(-lightDir,normal);
        float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shiness);

        vec3 diffusecolor = vec3(texture2D(material.texture_diffuse1,TexCoords));



        vec3 ambient = light.ambient * diffusecolor;
        vec3 diffuse = light.diffuse * diff * diffusecolor;
        vec3 specular = light.specular * spec * vec3(texture2D(material.texture_specular1,TexCoords));

        // 计算阴影
        float shadow = ShadowCalculation(FragPosLightSpace);
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

#version 450 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
//in vec2 TexCoords;

 
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_height1;//��������
uniform float shininess;
uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;

  
struct Light {
    vec4 position;
    vec3 direction;//�۹ⷽ��
    vec3 color;
    float cutOff;//�ڹ�Ȧ
    float outerCutOff;//���Ȧ

    float constant;//������Kc,��Ϊ1����֤��ĸ��Զ�����1С
    float linear;//һ����Kl,�����ֵ��ˣ������Եķ�ʽ����ǿ�ȡ�
    float quadratic;//������Kq,������ƽ����ˣ��ù�Դ�Զ��εݼ��ķ�ʽ����ǿ�ȡ�
};

uniform Light light;

uniform vec3 viewPos;

//vec3 getResult(Light light){
//    //������ɫ
//    //vec3 objectColor = texture(texture_diffuse1, TexCoords).rgb;
//    //vec3 specularColor = texture(texture_height1, TexCoords).rgb;
//    vec3 objectColor = vec3(1,255,1);
//    vec3 specularColor = vec3(255,1,1);

//    // ambientȫ�ֹ���ֵ
//    vec3 ambient = (ambientStrength * light.color) * objectColor;
    
//    //�õ���߷��� �� ˥��ֵ
//    vec3 lightDir;
//    float attenuation = 1.0f;
//    if(light.position.w != 0.0f ){//���Դ
//        float distance = length(light.position.xyz - FragPos);//���Դ����
//        attenuation = 1.0 / (light.constant + light.linear * distance + \
//                    light.quadratic * (distance * distance));////���Դǿ��˥��ϵ������
//        lightDir = normalize(light.position.xyz - FragPos);//����Ⱦ��ָ���Դ
//    }
//    else{//ƽ�й�
//        lightDir = normalize(light.position.xyz);
//    }

//    // diffuse���������ֵ
//    vec3 norm = normalize(Normal);
//    float diff = max(dot(norm, lightDir), 0.0);//�������,�Ƕ�Խ����ֵԽС.(��Ϊ����)
//    vec3 diffuse = (diffuseStrength * light.color) * diff * objectColor;//������ǿ������ߵ��õ����ƽ��ļн��й�,�н�ԽС,����Խ��.
    
//    // specular���淴��ֵ
//    vec3 viewDir = normalize(viewPos - FragPos);//���Ƶ�ָ�������
//    vec3 reflectDir = reflect(-lightDir, norm); //reflect(�������,����),���ط�������
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);//��������˺��32�η�,�η�����--���г̶ȸ�     vec3 specular = (material.specularStrength * light.color) * spec * specularColor; //����ǿ��*����Ƕ�Ȩֵ*������ɫ
//    vec3 specular = specularStrength * spec * specularColor;

//    //�۹�
//    float costheta = dot(-lightDir,normalize(light.direction));//���ߵ�ƬԪ����߷���ļн�
//    float epsilon   = light.cutOff - light.outerCutOff;//�����Ȧcos��
//    float intensity = clamp((costheta - light.outerCutOff) / epsilon, 0.0, 1.0); //clamp GLSL���ú��������ؽ��ڲ���2�Ͳ���3֮�����
//    diffuse  *= intensity;
//    specular *= intensity;

//    vec3 result = (ambient + diffuse + specular) * attenuation;//������ɫ * ����˥��ϵ��
//    return result;
//}

void main()
{
    //vec3 I = normalize(FragPos - viewPos);
    //vec3 R = reflect(I, normalize(Normal)); 
    //vec3 result = getResult(light);
    //FragColor = vec4(texture(skybox, R).rgb, 0.5);//����Ч��
    vec3 result = Normal;
    FragColor =vec4(result, 0.5);
}

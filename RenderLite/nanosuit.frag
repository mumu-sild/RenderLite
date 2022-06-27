#version 450 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
//in vec2 TexCoords;

 
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_height1;//反射纹理
uniform float shininess;
uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;

  
struct Light {
    vec4 position;
    vec3 direction;//聚光方向
    vec3 color;
    float cutOff;//内光圈
    float outerCutOff;//外光圈

    float constant;//常数项Kc,恒为1，保证分母永远不会比1小
    float linear;//一次项Kl,与距离值相乘，以线性的方式减少强度。
    float quadratic;//二次项Kq,与距离的平方相乘，让光源以二次递减的方式减少强度。
};

uniform Light light;

uniform vec3 viewPos;

//vec3 getResult(Light light){
//    //纹理颜色
//    //vec3 objectColor = texture(texture_diffuse1, TexCoords).rgb;
//    //vec3 specularColor = texture(texture_height1, TexCoords).rgb;
//    vec3 objectColor = vec3(1,255,1);
//    vec3 specularColor = vec3(255,1,1);

//    // ambient全局光照值
//    vec3 ambient = (ambientStrength * light.color) * objectColor;
    
//    //该点光线方向 及 衰减值
//    vec3 lightDir;
//    float attenuation = 1.0f;
//    if(light.position.w != 0.0f ){//点光源
//        float distance = length(light.position.xyz - FragPos);//点光源距离
//        attenuation = 1.0 / (light.constant + light.linear * distance + \
//                    light.quadratic * (distance * distance));////点光源强度衰减系数计算
//        lightDir = normalize(light.position.xyz - FragPos);//该渲染点指向光源
//    }
//    else{//平行光
//        lightDir = normalize(light.position.xyz);
//    }

//    // diffuse漫反射光照值
//    vec3 norm = normalize(Normal);
//    float diff = max(dot(norm, lightDir), 0.0);//向量点乘,角度越大数值越小.(不为负数)
//    vec3 diffuse = (diffuseStrength * light.color) * diff * objectColor;//漫反射强度与光线到该点与该平面的夹角有关,夹角越小,光照越弱.
    
//    // specular镜面反射值
//    vec3 viewDir = normalize(viewPos - FragPos);//绘制点指向摄像机
//    vec3 reflectDir = reflect(-lightDir, norm); //reflect(入射光线,法线),返回反射向量
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);//两向量点乘后的32次方,次方数大--集中程度高     vec3 specular = (material.specularStrength * light.color) * spec * specularColor; //反光强度*反光角度权值*光照颜色
//    vec3 specular = specularStrength * spec * specularColor;

//    //聚光
//    float costheta = dot(-lightDir,normalize(light.direction));//光线到片元与光线方向的夹角
//    float epsilon   = light.cutOff - light.outerCutOff;//内外光圈cos差
//    float intensity = clamp((costheta - light.outerCutOff) / epsilon, 0.0, 1.0); //clamp GLSL内置函数，返回介于参数2和参数3之间的数
//    diffuse  *= intensity;
//    specular *= intensity;

//    vec3 result = (ambient + diffuse + specular) * attenuation;//最终颜色 * 距离衰减系数
//    return result;
//}

void main()
{
    //vec3 I = normalize(FragPos - viewPos);
    //vec3 R = reflect(I, normalize(Normal)); 
    //vec3 result = getResult(light);
    //FragColor = vec4(texture(skybox, R).rgb, 0.5);//反射效果
    vec3 result = Normal;
    FragColor =vec4(result, 0.5);
}

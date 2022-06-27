#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
//layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//����.geom�����out�����ֻ�ܴ���geom��������������
//out VS_OUT{
//    vec2 texCoords;//����ͬ��,!!!!!
//    vec3 normal;
//    vec3 fragPos;
//}vs_out;

out vec3 FragPos;//����ֱ�ӽ����ݿ��������ɫ����������������
out vec3 Normal;


void main()
{
    //vs_out.normal = aNormal;
    //vs_out.texCoords = aTexCoords;
    //Normal = mat3(transpose(inverse(model))) * aNormal;//����ͼ�α任
    Normal = aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));
    //vs_out.fragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

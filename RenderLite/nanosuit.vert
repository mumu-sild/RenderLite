#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
//layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//若有.geom，则该out输出块只能传给geom。？？？？？对
//out VS_OUT{
//    vec2 texCoords;//必须同名,!!!!!
//    vec3 normal;
//    vec3 fragPos;
//}vs_out;

out vec3 FragPos;//尝试直接将数据跨过几何着色器？？？？？不可
out vec3 Normal;


void main()
{
    //vs_out.normal = aNormal;
    //vs_out.texCoords = aTexCoords;
    //Normal = mat3(transpose(inverse(model))) * aNormal;//用于图形变换
    Normal = aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));
    //vs_out.fragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

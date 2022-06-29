#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
//layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//╚¶Ëð.geomú¼È‗©├out╩õ│÷┐ÚÍ╗─▄┤½©°geomíúú┐ú┐ú┐ú┐ú┐ÂÈ
//out VS_OUT{
//    vec2 texCoords;//▒ÏðÙ═¼├¹,!!!!!
//    vec3 normal;
//    vec3 fragPos;
//}vs_out;

out vec3 FragPos;//│ó╩ÈÍ▒¢Ë¢½╩²¥¦┐þ╣²╝©║╬Î┼╔½ã¸ú┐ú┐ú┐ú┐ú┐▓╗┐╔
out vec3 Normal;


void main()
{
    //vs_out.normal = aNormal;
    //vs_out.texCoords = aTexCoords;
    //Normal = mat3(transpose(inverse(model))) * aNormal;//Ë├Ë┌═╝ð╬▒õ╗╗
    Normal = aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));
    //vs_out.fragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

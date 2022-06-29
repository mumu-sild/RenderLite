#version 450 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT{
	vec2 texCoords;
    vec3 normal;
    vec3 fragPos;
} gs_in[];


out vec2 TexCoords; 
out vec3 Normal;
out vec3 FragPos;

uniform float time;

vec4 explode(vec4 position, vec3 normal) { 
    float magnitude = 0.0;//爆破程度
    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude; 
    return position + vec4(direction, 0.0);
}

void main() {    
    vec3 normal = gs_in[0].normal;
    //gs_in[0、1、2]都是同一个值（在初始赋值时便是如此），若是使用EBO则需通过三角形绘制方向计算

    gl_Position = explode(gl_in[0].gl_Position, normal);
    TexCoords = gs_in[0].texCoords;
    Normal = gs_in[0].normal;
    FragPos = gs_in[0].fragPos;
    EmitVertex();

    gl_Position = explode(gl_in[1].gl_Position, normal);
    TexCoords = gs_in[1].texCoords;
    Normal = gs_in[1].normal;
    FragPos = gs_in[1].fragPos;
    EmitVertex();

    gl_Position = explode(gl_in[2].gl_Position, normal);
    TexCoords = gs_in[2].texCoords;
    Normal = gs_in[2].normal;
    FragPos = gs_in[2].fragPos;
    EmitVertex();

    EndPrimitive();
}
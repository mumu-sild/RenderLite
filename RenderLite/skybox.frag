#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube skybox;
uniform float mipmapLevel = 0.0f;

void main()
{
    //FragColor = vec4(1.0,0.0,1.0,1.0);
//    FragColor = texture(skybox, WorldPos);
//    return ;

    vec3 envColor = textureLod(skybox, WorldPos, mipmapLevel).rgb;
    //vec3 envColor = texture(skybox, WorldPos).rgb;
    //HDR tonemap and gamma correct
    //envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2));

    FragColor = vec4(envColor, 1.0);
}



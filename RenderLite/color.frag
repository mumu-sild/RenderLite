#version 450 core

//Êä³ö
layout (location = 0) out vec4 gPositionDepth;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec4 BrightColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 color = vec3(0.5,0.5,0.5);
uniform bool islight;

const float near_plane = 0.001f;
const float far_plane = 1000.0f;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main()
{
    gPositionDepth.rgb = FragPos;
    gPositionDepth.a = LinearizeDepth(gl_FragCoord.z);
    gNormal = normalize(Normal);

    if(islight) {
        gAlbedoSpec = vec4(0,0,0,0.1);
        BrightColor = vec4(color, 1.0);
    }
    else{
        gAlbedoSpec = vec4(color,0.1);
        BrightColor = vec4(0,0,0, 1.0);
    }
}

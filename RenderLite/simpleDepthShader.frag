#version 450 core
out vec4 FragColor;

float near_plane = 0.5f;
float far_plane = 100.5f;

uniform bool isPerspective = false;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane))/ far_plane;
}

void main()
{
    gl_FragDepth = gl_FragCoord.z;

    float depth = gl_FragCoord.z;

    if(isPerspective){
        depth = LinearizeDepth(depth);
    }

    FragColor = vec4(vec3(depth), 1.0f);


}

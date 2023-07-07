#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 projectionView;

out vec2 TexCoords;
out vec3 CameraRay;
out vec3 FragPos;

uniform vec3 viewPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 

    FragPos = gl_Position.xyz;

    TexCoords = aTexCoord;

    vec4 worldPos = inverse(projectionView) * vec4(aPos.xy, 1.0, 1.0);
    CameraRay = worldPos.xyz / worldPos.w - viewPos;
}

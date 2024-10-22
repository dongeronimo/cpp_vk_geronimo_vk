#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 uv0; 

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec2 fragUV0;

layout(set=0, binding=0) uniform Camera
{
    mat4 view;
    mat4 proj;
    vec3 viewPos;
} camera;

layout(set=1, binding=0) uniform Model
{
    mat4 mat;
} model;

void main() 
{
    vec4 worldPosition = model.mat * vec4(inPosition, 1.0);
    fragPosition = worldPosition.xyz;
    fragUV0 = uv0;
    gl_Position = camera.proj * camera.view * worldPosition;
}

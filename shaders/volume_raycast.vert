#version 450
/*
Set 0: properties that constant in a frame.
Set 1: properties that vary from object to object
*/
layout(set=0, binding=0) uniform Camera 
{
    mat4 view;
    mat4 proj;
    vec3 viewPos;
}camera;
layout(set=1, binding=0) uniform Model 
{
    mat4 mat;
} model;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec3 rayOrigin;
layout(location = 2) out vec3 rayDirection;
void main() 
{
    vec4 worldPosition = model.mat * vec4(inPosition);
    rayOrigin = camera.viewPos;
    rayDirection = normalize(worldPosition.xyz - camera.viewPos);
    gl_Position = camera.proj * camera.view * worldPosition;
}
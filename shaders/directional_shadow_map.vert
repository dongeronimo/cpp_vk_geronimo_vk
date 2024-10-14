#version 450

layout(location = 0) in vec3 inPosition;

layout(set=0, binding=0) uniform Model
{
    mat4 mat;
} model;

layout(push_constant) uniform PushConstants {
    mat4 lightViewProjMatrix; // Light's view-projection matrix
} pc;

void main() {
    gl_Position = pc.lightViewProjMatrix * model.mat * vec4(inPosition, 1.0);
}

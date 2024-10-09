#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 uv0; //not used for now


layout(set=0, binding=0) uniform Camera
{
    mat4 view;
    mat4 proj;
    vec3 viewPos;
} camera;

void main() 
{
    gl_Position = camera.proj * camera.view * vec4(inPosition, 1.0);
}
//#version 450
//
//layout(location = 0) in vec3 inPosition;
//layout(location = 1) in vec3 inNormal;
//layout(location = 2) in vec2 uv0; //not used for now
//
//layout(location = 0) out vec3 fragPosition;
//layout(location = 1) out vec3 fragNormal;
//layout(location = 2) out vec4 fragPosLightSpace;
//
//layout(set = 0, binding = 0) uniform Camera {
//    mat4 view;
//    mat4 proj;
//    vec3 viewPos;
//} camera;
//
//layout(set = 0, binding = 1) uniform Model {
//    mat4 mat;
//} modelMatrix;
//
//layout(set = 0, binding = 2) uniform LightSpaceMatrix {
//    mat4 mat;
//} lightSpaceMatrix;
//
//void main() {
//    mat4 model = modelMatrix.mat;
//    mat4 light = lightSpaceMatrix.mat;
//    fragPosition = vec3(model * vec4(inPosition, 1.0));
//    fragNormal = mat3(transpose(inverse(model))) * inNormal;
//    fragPosLightSpace = light * model * vec4(inPosition, 1.0);
//    gl_Position = camera.proj * camera.view * vec4(fragPosition, 1.0);
//}
//
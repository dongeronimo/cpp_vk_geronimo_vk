#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 uv0; //todo textures: not used for now

layout(location = 0) out vec3 fragPosition;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragTexCoord;
layout(location = 3) out vec4 fragShadowCoord;

layout(set=0, binding=0) uniform Model
{
    mat4 mat;
} model;

layout(set=1, binding=0) uniform Camera
{
    mat4 view;
    mat4 proj;
    vec3 viewPos;
} camera;
layout(set = 3, binding = 0) uniform DirectionalLightProperties {
   vec3 test;
   vec3 direction;
   mat4 lightSpaceMatrix;
   vec4 colorAndIntensity;
} directionalLight;


mat3 extractRotation(mat4 m) {
    return mat3(m);
}

mat4 rotateX(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, c, -s, 0.0,
        0.0, s, c, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

mat4 rotateY(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(
        c, 0.0, s, 0.0,
        0.0, 1.0, 0.0, 0.0,
        -s, 0.0, c, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

mat4 rotateZ(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(
        c, -s, 0.0, 0.0,
        s, c, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}



mat4 rotateXYZ(float angleX, float angleY, float angleZ) {
    return rotateX(angleX) * rotateY(angleY) * rotateZ(angleZ);
}

void main() 
{
    fragPosition = vec3(model.mat * vec4(inPosition, 1.0));
    mat3 normalMatrix = transpose(inverse(mat3(model.mat)));
    fragNormal = normalize( normalMatrix * inNormal );
    fragTexCoord = uv0;
    mat4 gambi = rotateXYZ(radians(-90),0,0) * directionalLight.lightSpaceMatrix;
    fragShadowCoord = gambi * vec4(fragPosition, 1.0);
    gl_Position = camera.proj * camera.view * vec4(fragPosition, 1.0);
}

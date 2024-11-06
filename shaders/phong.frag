#version 450
layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec4 fragShadowCoord;

layout(location = 0) out vec4 outColor;

layout(set=1, binding=0) uniform Camera{
    mat4 view;
    mat4 proj;
    vec3 viewPos;
} camera;
layout(set = 2, binding = 0) uniform sampler2DShadow shadowMap;
layout(set = 3, binding = 0) uniform DirectionalLightProperties {
   vec3 direction;
   mat4 lightSpaceMatrix;
   vec4 colorAndIntensity;
} directionalLight;
layout(set = 4, binding = 0) uniform sampler phongSampler;

float calculateShadow(vec4 fragShadowCoord) {
    float bias = 0.001; //TODO shadows: use vk's bias infra. This exists due to shadow acne
    vec3 projCoords = fragShadowCoord.xyz / fragShadowCoord.w;
    projCoords = vec3(projCoords.xy * 0.5 + 0.5, projCoords.z-bias);
    return texture(shadowMap, vec3(projCoords.xyz));
}

void main()
{
    vec3 color = vec3(1,0,0);
    vec3 normal = normalize(fragNormal);
    vec3 lightDirection = normalize(-directionalLight.direction);
    float diff = max(dot(lightDirection, normal), 0.0f);
    vec3 diffuseColor = diff * 
        directionalLight.colorAndIntensity.rgb *
        directionalLight.colorAndIntensity.a *
        color;
    float shadow = calculateShadow(fragShadowCoord);
    vec3 lighting = shadow * diffuseColor;
    outColor = vec4(lighting, 1.0);

}
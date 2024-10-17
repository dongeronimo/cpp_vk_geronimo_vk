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
layout(set = 2, binding = 0) uniform sampler2D  shadowMap;
layout(set = 3, binding = 0) uniform DirectionalLightProperties {
   vec3 direction;
   mat4 lightSpaceMatrix;
   vec4 colorAndIntensity;
} directionalLight;

float calculateShadow(vec4 fragShadowCoord) {
    vec3 projCoords = fragShadowCoord.xyz / fragShadowCoord.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth  = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;  
    float shadow = (currentDepth ) > closestDepth  ? 1.0 : 0.0;  
    return 0;
}
//float calculateShadow(vec4 fragShadowCoord) {
//    vec3 projCoords = fragShadowCoord.xyz / fragShadowCoord.w;
//    projCoords = projCoords * 0.5 + 0.5;
//    float closestDepth = texture(shadowMap, projCoords.xy).r;
//    float currentDepth = projCoords.z;
//    float shadow = currentDepth > closestDepth + 0.005 ? 1.0 : 0.1;
//    return shadow;
//}

void main()
{
    vec3 color = vec3(1,0,0);
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(-directionalLight.direction);//normalize(-directionalLight.direction);
    vec3 viewDir = normalize(camera.viewPos - fragPosition);
    vec3 reflectDir = reflect(-lightDir, normal);
    //TODO ambient: Create a descriptor set for ambient
    vec3 ambient = 0.1 * color;
    // Diffuse
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * 
        directionalLight.colorAndIntensity.rgb *
        directionalLight.colorAndIntensity.a * 
        color; //TODO material: this will come from the material descriptor set

    // Specular
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
    vec3 specular = spec * 
        directionalLight.colorAndIntensity.rgb *
        directionalLight.colorAndIntensity.a *
        vec3(0.5);//TODO material: this will come from the material descriptor set

    // Shadow
    float shadow = calculateShadow(fragShadowCoord);

    vec3 lighting = ambient + /*(1.0 - shadow)* */  (diffuse);
    outColor = vec4(diffuse, 1.0);

}
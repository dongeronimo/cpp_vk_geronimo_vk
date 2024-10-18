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
layout(set = 2, binding = 0) uniform sampler2DShadow  shadowMap;
layout(set = 3, binding = 0) uniform DirectionalLightProperties {
   vec3 test;
   vec3 direction;
   mat4 lightSpaceMatrix;
   vec4 colorAndIntensity;
} directionalLight;
mat3 extractRotation(mat4 m) {
    return mat3(m);
}

mat3 rotateX(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat3(
        1.0, 0.0, 0.0,
        0.0, c, -s,
        0.0, s, c
    );
}
mat3 rotateY(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat3(
        c, 0.0, s,
        0.0, 1.0, 0.0,
        -s, 0.0, c
    );
}
mat3 rotateZ(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat3(
        c, -s, 0.0,
        s, c, 0.0,
        0.0, 0.0, 1.0
    );
}


mat3 rotateXYZ(float angleX, float angleY, float angleZ) {
    return rotateX(angleX) * rotateY(angleY) * rotateZ(angleZ);
}


float calculateShadow(vec4 fragShadowCoord) {
 // Perform perspective divide (NDC space)
    vec3 projCoords = fragShadowCoord.xyz / fragShadowCoord.w;

    // Transform to [0, 1] for texture lookup
    projCoords = projCoords * 0.5 + 0.5;

    // Perform shadow comparison; returns a value between 0 and 1 based on shadow
    return texture(shadowMap, vec3(projCoords.xy, projCoords.z));

}
//float calculateShadow(vec4 fragShadowCoord) {
//    vec3 projCoords = fragShadowCoord.xyz / fragShadowCoord.w;
//    projCoords = projCoords * 0.5 + 0.5;
//    float depthInShadowMap = texture(shadowMap, projCoords.xy).r;
//    float fragmentDepthInLightSpace = projCoords.z;
//    float shadow = fragmentDepthInLightSpace > depthInShadowMap + 0.005 ? 1.0 : 0.1;
//    return shadow;
//}

void main()
{
    //IDK why that was necessary - for some reason the light was being rendered in a rotated position, even if the shadow map is correct
    mat3 lightRotation = extractRotation(directionalLight.lightSpaceMatrix);
    mat3 gambiarra = rotateXYZ(radians(-90),0,0) * lightRotation;
    vec3 lightDirection = normalize(directionalLight.direction);//normalize(gambiarra[2]);//normalize(directionalLight.direction);
    vec3 color = vec3(1,0,0);
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = lightDirection;//normalize(-directionalLight.direction);
    vec3 viewDir = normalize(camera.viewPos - fragPosition);
    vec3 reflectDir = reflect(lightDir, normal);
    ////TODO ambient: Create a descriptor set for ambient
    vec3 ambient = 0.01 * color;
    // Diffuse
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * 
        directionalLight.colorAndIntensity.rgb *
        directionalLight.colorAndIntensity.a * 
        color; //TODO material: this will come from the material descriptor set

    // Specular
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = spec * 
        directionalLight.colorAndIntensity.rgb *
        directionalLight.colorAndIntensity.a *
        vec3(0.5);//TODO material: this will come from the material descriptor set

    // Shadow
    float shadow = calculateShadow(fragShadowCoord);

    vec3 lighting = ambient +  shadow *  (diffuse+specular);
    outColor = vec4(lighting, 1.0);

}
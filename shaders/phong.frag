#version 450
layout(location = 0) out vec4 outColor;
layout(location = 0) in vec3 fragPosition;
void main()
{
    outColor = vec4(fragPosition,1);
}
//
//layout(location = 0) in vec3 fragPosition;
//layout(location = 1) in vec3 fragNormal;
//layout(location = 2) in vec4 fragPosLightSpace;
//
//layout(location = 0) out vec4 outColor;
//
//layout(set = 0, binding = 3) uniform Material {
//    vec3 ambient;
//    vec3 diffuse;
//    vec3 specular;
//    float shininess;
//} material;
//
//layout(set = 0, binding = 0) uniform Camera {
//    mat4 view;
//    mat4 proj;
//    vec3 viewPos;
//} camera;
//
//layout(set = 0, binding = 4) uniform LightPosition {
//    vec3 lightPos;
//} lightPosition;
//
//layout(set = 0, binding = 5) uniform sampler2D shadowMap;
//
//float ShadowCalculation(vec4 fragPosLightSpace) {
//    // Perform perspective divide
//    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
//    // Transform to [0,1] range
//    projCoords = projCoords * 0.5 + 0.5;
//    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
//    float closestDepth = texture(shadowMap, projCoords.xy).r; 
//    // Get depth of current fragment from light's perspective
//    float currentDepth = projCoords.z;
//    // Check whether current frag pos is in shadow
//    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
//    return shadow;
//}
//
//void main() {
//    vec3 norm = normalize(fragNormal);
//    vec3 lightDir = normalize(lightPosition.lightPos - fragPosition);
//    vec3 viewDir = normalize(camera.viewPos - fragPosition);
//    vec3 reflectDir = reflect(-lightDir, norm);
//
//    // Ambient
//    vec3 ambient = material.ambient;
//
//    // Diffuse
//    float diff = max(dot(norm, lightDir), 0.0);
//    vec3 diffuse = diff * material.diffuse;
//
//    // Specular
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//    vec3 specular = spec * material.specular;
//
//    // Shadow
//    float shadow = ShadowCalculation(fragPosLightSpace);
//    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);
//
//    outColor = vec4(lighting, 1.0);
//}
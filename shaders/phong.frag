#version 450
layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragUV0;
layout(location = 3) in vec3 cameraPos;

layout(std140, set=0, binding=1) uniform PointLights
{
    vec4 positions[16];
    vec4 colorAndIntensity[16];
} pointLights;

layout(set=1, binding=1) uniform PhongMaterial
{
    vec4 diffuseColorAndIntensity;
    vec4 specularColorAndIntensity;
} phongMaterial;

layout(set = 2, binding = 0) uniform sampler2D texSampler;

void main()
{
    //initial values
    vec4 textureColor = texture(texSampler,fragUV0);
    vec3 ambient = vec3(0.05f) * textureColor.xyz;//TODO ambient: will be another binding at set #0
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    vec3 norm = normalize(fragNormal);
    for(int i=0; i<16; i++)
    {
        if(pointLights.positions[i].w > 0)
        {
            vec3 lightPos = pointLights.positions[i].xyz;
            vec3 lightColour = pointLights.colorAndIntensity[i].xyz;
            float lightIntensity = pointLights.colorAndIntensity[i].w;
            //from the fragment world coordinates to the light
            vec3 lightDir = normalize(lightPos - fragPosition);
            float diff = max(dot(norm, lightDir), 0.0f);
            diffuse += diff * 
                textureColor.xyz * 
                phongMaterial.diffuseColorAndIntensity.xyz *
                phongMaterial.diffuseColorAndIntensity.w * 
                lightColour * lightIntensity;
            
            vec3 viewDir = normalize(cameraPos - fragPosition);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir),0.0), 32.0);//TODO specular: will come from the material
            specular += spec * textureColor.xyz * 
                phongMaterial.specularColorAndIntensity.xyz *
                phongMaterial.specularColorAndIntensity.w *
                lightColour * lightIntensity;
        }
    }
    vec3 phong = ambient + diffuse + specular;
    outColor = vec4(phong, 1.0f);
}

#version 450
layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragUV0;
layout(location = 3) in vec3 cameraPos;

layout(set=0, binding=1) uniform PointLights
{
    vec3 positions[16];
    vec4 colorAndIntensity[16];
    uint isActive[16];
} pointLights;

layout(set = 2, binding = 0) uniform sampler2D texSampler;

void main()
{
    //initial values
    vec4 diffuseColor = texture(texSampler,fragUV0);
    vec3 ambient = vec3(0.05f) * diffuseColor.xyz;//TODO ambient: will be another binding at set #0
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    vec3 norm = normalize(fragNormal);
    for(int i=0; i<16; i++)
    {
        if(pointLights.isActive[i] == 0)
        {
            continue;
        }
        else
        {
            vec3 lightPos = pointLights.positions[i];
            vec3 lightColour = pointLights.colorAndIntensity[i].xyz;
            float lightIntensity = pointLights.colorAndIntensity[i].w;
            //from the fragment world coordinates to the light
            vec3 lightDir = normalize(lightPos - fragPosition);
            float diff = max(dot(norm, lightDir), 0.0f);
            diffuse += diff * diffuseColor.xyz * lightColour * lightIntensity;
            
            vec3 viewDir = normalize(cameraPos - fragPosition);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir),0.0), 32.0);//TODO specular: will come from the material
            specular += spec * diffuseColor.xyz * lightColour * lightIntensity;
        }
    }
    vec3 phong = ambient + diffuse + specular;
    outColor = vec4(phong, 1.0f);
}

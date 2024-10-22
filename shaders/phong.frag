#version 450
layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec2 fragUV0;

layout(set = 2, binding = 0) uniform sampler2D texSampler;
void main()
{
    outColor = texture(texSampler,fragUV0);
}

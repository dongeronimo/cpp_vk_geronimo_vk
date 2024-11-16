#version 450

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 rayOrigin;
layout(location = 2) in vec3 rayDirection;
layout(location = 0) out vec4 fragColor;

layout (set=0, binding=1) uniform sampler volumeSampler;
layout (set=0, binding=2) uniform texture3D volumeTexture;
layout(set=1, binding=1) uniform VolumeProperties 
{
    float stepSize;
    float maxRayLength;
} volumeProperties;

// Transfer function to map density values to color and opacity
vec4 transferFunction(int density) {
    // Map signed density value to color and opacity
    float normalizedDensity = clamp(float(density) / 32767.0, -1.0, 1.0); // Normalize int16 to [-1, 1]
    
    // Example: Grayscale color mapping with opacity scaling
    vec3 color = vec3(0.5) + 0.5 * vec3(normalizedDensity);
    float opacity = abs(normalizedDensity) * 0.8;
    
    return vec4(color, opacity);
}

void main() 
{
    vec3 pos = rayOrigin;
    vec3 step = normalize(rayDirection) * volumeProperties.stepSize;
    vec4 accumulatedColor = vec4(0.0);
    for(float t=0.0; t<volumeProperties.maxRayLength; t += volumeProperties.stepSize)
    {
        //In GLSL, integer textures (isampler3D, usampler3D, etc.) be sampled using 
        //texelFetch() because texelFetch() is specifically designed to handle integer 
        //samplers. GLSL expects integer texture sampling to be precise and without 
        //interpolation.
        // Convert normalized pos to texel-space coordinates
        ivec3 texelCoords = ivec3(pos * textureSize(volumeTexture, 0));
        // Fetch density value using texelFetch for isampler3D
        int density = texelFetch(volumeTexture, texelCoords, 0).r;
        // Apply the transfer function to map density to color and opacity
        vec4 color = transferFunction(density);
        // Accumulate color and opacity
        accumulatedColor.rgb += (1.0 - accumulatedColor.a) * color.rgb * color.a;
        accumulatedColor.a += (1.0 - accumulatedColor.a) * color.a;
        // Terminate if opacity is fully opaque
        if (accumulatedColor.a >= 1.0) break;
        // Advance along the ray
        pos += step;
    }
    fragColor = accumulatedColor;
}
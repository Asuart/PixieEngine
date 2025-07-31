#version 330 core

out float FragColor;

in vec2 fTexCoords;

uniform sampler2D gPositionRoughness;
uniform sampler2D gNormalMetallic;
uniform sampler2D noiseTexture;

const int SSAOKernelSize = 64;
uniform vec3 ssaoKernel[SSAOKernelSize];
uniform vec3 cameraPos;

// tile noise texture over screen based on screen dimensions divided by noise size
uniform vec2 noiseScale;

// parameters (you'd probably want to use them as uniforms to more easily tweak the effect)
float radius = 1.5f;
float bias = 0.05f;

uniform mat4 mProjection;
uniform mat4 mView;

void main() {
    // get input for SSAO algorithm
    vec3 fragPos = texture(gPositionRoughness, fTexCoords).xyz;
    vec3 normal = normalize(texture(gNormalMetallic, fTexCoords).xyz);
    vec3 randomVec = normalize(texture(noiseTexture, fTexCoords * noiseScale).xyz);

    // flip normal to face camera
    vec3 toCamera = normalize(fragPos - cameraPos);
    if(dot(toCamera, normal) > 0) {
        normal = -normal;
    }

    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0f;
    vec4 sampleProjection = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    for(int i = 0; i < 64; i++) {
        // get sample position
        vec3 sampleOffset = TBN * ssaoKernel[i] * radius; // from tangent to view-space
        vec3 samplePosition = fragPos + sampleOffset; 
        float sampleDepth = length(samplePosition - cameraPos);

        // project sample position (to sample texture) (to get position on screen/texture)
        sampleProjection = vec4(samplePosition, 1.0f);
        sampleProjection = mProjection * mView * sampleProjection; // from view to clip-space
        sampleProjection.xyz /= sampleProjection.w; // perspective divide
        sampleProjection.xyz = sampleProjection.xyz * 0.5f + 0.5f; // transform to range 0.0 - 1.0

        vec3 fragSample = texture(gPositionRoughness, sampleProjection.xy).xyz;
        float fragDepth = length(fragSample - cameraPos);
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0f, 1.0f, radius / abs(fragDepth - sampleDepth));
        occlusion += (sampleDepth >= fragDepth + bias ? 1.0f : 0.0f) * rangeCheck;  
    }
    occlusion = 1.0f - (occlusion / SSAOKernelSize);

    FragColor = occlusion;
}
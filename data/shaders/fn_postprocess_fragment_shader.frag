#version 330 core

layout (location = 0) out vec4 fragColor;

in vec2 textureCoords;

uniform sampler2D sceneTexture;

const float offset = 1.0 / 300.0;
const float blurFactor = 8.0;

void main()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    //all kernels should sum up to 1 if you add all the weights together. If they don't add up to 1 it means that the resulting texture color ends up brighter or darker than the original texture value
    float blur_kernel[9] = float[](
        1.0 / blurFactor, 2.0 / blurFactor, 1.0 / blurFactor,
        2.0 / blurFactor, 4.0 / blurFactor, 2.0 / blurFactor,
        1.0 / blurFactor, 2.0 / blurFactor, 1.0 / blurFactor  
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(sceneTexture, textureCoords.st + offsets[i]));
    }

    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * blur_kernel[i];

	//fragColor = vec4(vec3(1.0 - texture(sceneTexture, textureCoords)), 1.0);
	//fragColor = vec4(col, 1.0);
    fragColor = texture(sceneTexture, textureCoords);
}
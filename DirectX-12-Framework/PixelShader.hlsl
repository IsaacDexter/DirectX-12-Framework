struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

SamplerState g_sampler : register(s0);
Texture2D g_texture : register(t0);

float4 main(PSInput input) : SV_TARGET
{
    
    return g_texture.Sample(g_sampler, input.uv);
}
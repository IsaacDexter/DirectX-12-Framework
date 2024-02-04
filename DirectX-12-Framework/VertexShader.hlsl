cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 mvp;
    float4 padding[12];
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

PSInput main(float4 position : POSITION, float4 uv : TEXCOORD)
{
    PSInput result;

    result.position = mul(position, mvp);
    result.uv = uv;
    
    return result;
}
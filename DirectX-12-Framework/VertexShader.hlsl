cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 mvp;
    float4 padding[12];
};

struct VSInput
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

PSInput main(VSInput input)
{
    PSInput result;

    result.position = mul(float4(input.position, 1.0f), mvp);
    //result.position = mul(position, transpose(mvp));
    result.uv = input.uv;
    
    return result;
}
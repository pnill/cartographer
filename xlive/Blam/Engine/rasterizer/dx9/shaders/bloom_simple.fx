
struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 color;
};

float4 constants : register(c0);
float4 brightness : register(c1);
float4 bloom_alpha : register(c2);
sampler2D sm2d[1] : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    PS_OUTPUT output;
    
    float4 tex_color = tex2D(sm2d[0], input.texcoord);
    
    tex_color.a *= bloom_alpha.x;
    
    float3 color_clipped = tex_color.rgb - constants.w;
    color_clipped = saturate(color_clipped * 4.0f);
    tex_color.a *= 2.0f;
    
    output.color.rgb = brightness.rgb * color_clipped + tex_color.a;
    output.color.a = tex_color.a * brightness.w;
    return output.color;
}
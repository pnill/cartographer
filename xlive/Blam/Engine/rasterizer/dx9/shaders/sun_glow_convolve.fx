struct PS_INPUT
{
    float4 color : COLOR;
    float2 texcoord[4]: TEXCOORD;
};

struct PS_OUTPUT
{
    float4 color;
};

sampler2D sunshine : register(s0);

PS_OUTPUT main(PS_INPUT input) : COLOR
{
    float4 v1 = tex2D(sunshine, input.texcoord[3].xy) * 0.5f;
    float4 v2 = tex2D(sunshine, input.texcoord[2].xy) * 0.5f;
    v1 += v2;
    v1 *= input.color.w;
    
    float4 v3 = tex2D(sunshine, input.texcoord[1].xy) * 0.5f;
    float4 v4 = tex2D(sunshine, input.texcoord[0].xy) * 0.5f;
    v3 += v4;
    v3 *= input.color.w;

    PS_OUTPUT output;
    output.color = v1 + v3;
    return output;
}

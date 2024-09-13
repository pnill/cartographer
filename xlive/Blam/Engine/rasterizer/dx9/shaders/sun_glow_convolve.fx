struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color[2] : COLOR;
    float4 texcoord[7] : TEXCOORD;
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
    v1 *= input.color[0].a;
    
    float4 v3 = tex2D(sunshine, input.texcoord[1].xy) * 0.5f;
    float4 v4 = tex2D(sunshine, input.texcoord[0].xy) * 0.5f;
    v3 += v4;
    v3 *= input.color[0].a;

    PS_OUTPUT output;
    output.color = v1 + v3;
    return output;
}

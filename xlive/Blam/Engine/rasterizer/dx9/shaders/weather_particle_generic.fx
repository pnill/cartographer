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

sampler2D sm[4] : register(s0);

PS_OUTPUT main(PS_INPUT input) : COLOR
{
    float4 color = tex2D(sm[3], input.texcoord[3].xy);
    
    PS_OUTPUT output;
    output.color = color * input.color[0];
    return output;
}

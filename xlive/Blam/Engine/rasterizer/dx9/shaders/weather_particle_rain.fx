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

PS_OUTPUT main(PS_INPUT input) : COLOR
{
    PS_OUTPUT output;
    output.color = input.color[0];
    return output;
}

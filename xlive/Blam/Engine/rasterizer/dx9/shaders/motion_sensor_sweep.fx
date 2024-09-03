struct PS_INPUT
{
    float2 TexCoord0 : TEXCOORD0;
    float4 Color0 : COLOR0;
};

struct PS_OUTPUT
{
    float4 texColor;
};

float1 hud_fade : register(c0);
sampler2D s0 : register(s0);

PS_OUTPUT main(PS_INPUT input) : COLOR
{
    PS_OUTPUT output;
    
    output.texColor = tex2D(s0, input.TexCoord0);
    output.texColor *= hud_fade;
    // prevent the alpha to go below 0.5
    // which allows the points on the radar to be rendered properly
    // might need some fine-tuning
    output.texColor.a = max(output.texColor.a, 0.5f);
    output.texColor *= input.Color0;
    return output;
}
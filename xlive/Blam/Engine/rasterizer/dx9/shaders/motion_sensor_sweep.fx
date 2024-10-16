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

float1 hud_fade : register(c0);
sampler2D tex : register(s0);

PS_OUTPUT main(PS_INPUT input) : COLOR
{
    PS_OUTPUT output;
    
    output.color = tex2D(tex, input.texcoord[0].xy);
    output.color *= hud_fade;
    // prevent the alpha to go below 0.192160
    // which allows the points on the radar to be rendered properly
    // might need some fine-tuning
    output.color.a = max(output.color.a, 0.192160f);
    output.color *= input.color[0];
    return output;
}
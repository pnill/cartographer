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

float4 bumpenv : register(c0);
float4 c1 : register(c1);
float4 c2 : register(c2);
float4 c3 : register(c3);
sampler2D s1 : register(s0);

PS_OUTPUT main(PS_INPUT input) : COLOR
{
    float2 texcoords = input.texcoord[1].xy / input.texcoord[1].w;
    float2 color = tex2D(s1, texcoords).xy;
    
    color = saturate(color);
    color.y *= c2.w;
    
    color.x = saturate((color.x * c1.w) + color.y);

    PS_OUTPUT output;
    output.color.rgb = color.x * c3.xyz;
    output.color.a = color.x;
    return output;
}

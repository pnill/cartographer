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

float4 tint_colors[3] : register(c0);
float4 dot_factors[3] : register(c3);
sampler2D sm[4] : register(s0);

PS_OUTPUT main(PS_INPUT input) : COLOR
{   
    float4 plate_texture = tex2D(sm[1], input.texcoord[2].xy);
    float gradient = -dot(plate_texture.a, dot_factors[0]) + 1.0;

    plate_texture = tex2D(sm[2], input.texcoord[3].xy);
    gradient *= -dot(plate_texture.a, dot_factors[1]) + 1.0;
    
    plate_texture = tex2D(sm[3], input.texcoord[4].xy);
    float dp_result = -dot(plate_texture.a, dot_factors[2]) + 1.0;
    gradient = -gradient * dp_result + 1.0;

    float4 depth = tex2D(sm[0], input.texcoord[0].xy).argb;
    depth.g += depth.b * 0.003906;
    
    // TODO: name
    float3 unk;
    unk.x = saturate(depth.a * 0.000015 + depth.g);
    unk.yz = 1.0;
    
    gradient *= saturate(dot(unk, input.texcoord[1].xyz));
    
    PS_OUTPUT output;
    output.color.rgb = gradient * tint_colors[0].rgb;
    output.color.a = gradient;
    return output;
}

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

float4 fog_atmospheric_color : register(c0);
float4 fog_atmospheric_secondary_color : register(c1);
float4 fog_sky_color : register(c2);
sampler2D sm[3] : register(s0);

PS_OUTPUT main(PS_INPUT input) : COLOR
{    
    float4 depth = tex2D(sm[0], input.texcoord[0].xy);
    depth.r += (depth.g * 0.00390625);
    depth.r += (depth.b * 0.000015);
    depth.gb = 1.0;

    float2 proj_tc;
    proj_tc.x = dot(depth.rgg, input.texcoord[1].xyz);
    proj_tc.y = dot(depth.rgb, input.texcoord[2].xyz);

    float4 gradient = tex2D(sm[2], proj_tc);
    
    float4 adjusted_blend = fog_atmospheric_secondary_color * gradient.b;
    adjusted_blend+= fog_atmospheric_color * gradient.g;
    
    float4 res = -adjusted_blend + fog_sky_color;
    
    float lerp_factor = depth.r * depth.r * depth.r; // Linear interpolation factor
    
    // Final colour calculation is done here
    PS_OUTPUT output;  
    output.color = (lerp_factor * res) + adjusted_blend;
    return output;
}

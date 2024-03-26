// fxc /T ps_2_0 /Fo ../compiled/motion_sensor_sweep.pso ../motion_sensor_sweep.hlsl

float1 hud_fade : register(c0);
sampler2D s0 : register(s0);

float4 main(float2 texcoord : TEXCOORD0, float4 color : COLOR0) : COLOR
{
    float4 texColor = tex2D(s0, texcoord);
    texColor *= hud_fade;
    // prevent the alpha to go below 0.5
    // which allows the points on the radar to be rendered properly
    // might need some fine-tuning
    texColor.a = max(texColor.a, 0.5f);
    texColor *= color;
    return texColor;
}
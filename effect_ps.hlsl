#include "sprite.hlsli"

Texture2D color_map : register(t0);
SamplerState point_sampler_state : register(s0);
SamplerState linear_sampler_state : register(s1);
SamplerState anisotropic_sampler_state : register(s2);

float4 main(VS_OUT pin) : SV_TARGET
{
	if (pin.texcoord.y < 0.25)
	{
		return color_map.Sample(linear_sampler_state, pin.texcoord);
	}
	else if (pin.texcoord.y < 0.5)
	{
		return color_map.Sample(linear_sampler_state, pin.texcoord) * 0.75;
	}
	else if (pin.texcoord.y < 0.75)
	{
		return color_map.Sample(linear_sampler_state, pin.texcoord) * 0.5;
	}
	
	return color_map.Sample(linear_sampler_state, pin.texcoord) * 0.25;
}
#include "sprite.hlsli"
//Texture2D diffuse_map : register(t0);
//SamplerState diffuse_map_sampler_state : register(s0);
 Texture2D color_map : register(t0);
SamplerState point_sampler_state : register(s0);
SamplerState linear_sampler_state : register(s1);
SamplerState anisotropic_sampler_state : register(s2);
#if 0
float4 main(VS_OUT pin) : SV_TARGET
{
	return diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord) * pin.color;
}
#endif
// #include "sprite.hlsli"

 float4 main(VS_OUT pin) : SV_TARGET
 {
	 float4 color = color_map.Sample(anisotropic_sampler_state, pin.texcoord);
	 float alpha = color.a;
	 #if 1
	 // Inverse gamma process
		  const float GAMMA = 2.2;
	  color.rgb = pow(color.rgb, GAMMA);
	  #endif
		 return float4(color.rgb, alpha) * pin.color;
	  }
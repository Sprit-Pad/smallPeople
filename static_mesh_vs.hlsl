#include "static_mesh.hlsli"
#if 0
VS_OUT main(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD/*UNIT.14*/)
{
	VS_OUT vout;
	vout.position = mul(position, mul(world, view_projection));

	normal.w = 0;
	float4 N = normalize(mul(normal, world));
	float4 L = normalize(-light_direction);

	vout.color.rgb = material_color.rgb * max(0, dot(L, N));
	vout.color.a = material_color.a;

	// 
	vout.texcoord = texcoord;

	return vout;
}
#else
// 
VS_OUT main(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD/*UNIT.14*/)
{
	VS_OUT vout;
	vout.position = mul(position, mul(world, view_projection));

	vout.world_position = mul(position, world);
	normal.w = 0;
	vout.world_normal = normalize(mul(normal, world));

	vout.color = material_color;
	vout.texcoord = texcoord;

	return vout;
}
#endif
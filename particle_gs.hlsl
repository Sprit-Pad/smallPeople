#include "particle.hlsli"
//particle_gs.hlsl

StructuredBuffer<particle> particle_buffer : register(t9);

[maxvertexcount(4)]
void main(point VS_OUT input[1] : SV_POSITION, inout TriangleStream<GS_OUT> output)
{
	const float2 corners[] =
	{
		float2(-1.0f, -1.0f),
		float2(-1.0f, +1.0f),
		float2(+1.0f, -1.0f),
		float2(+1.0f, +1.0f),
	};
	const float2 texcoords[] =
	{
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f),
	};
	
	particle p = particle_buffer[input[0].vertex_id];

	const float aspect_ratio = 1280.0 / 720.0;
	const float length_factor = 2.0; // í∑Ç≥ÇÃî{ó¶
	const float width_factor = 0.5; // ïùÇÃî{ó¶
	float2 particle_scale = float2(particle_size, particle_size * aspect_ratio);


	[unroll]
	for (uint vertex_index = 0; vertex_index < 4; ++vertex_index)
	{
		GS_OUT element;

		// Transform to clip space
		element.position = mul(float4(p.position, 1), view_projection);
		// Make corner position as billboard		
		element.position.xy += corners[vertex_index] * particle_scale;

		element.color = p.color;
		element.texcoord = texcoords[vertex_index];
		output.Append(element);
	}

	output.RestartStrip();
}

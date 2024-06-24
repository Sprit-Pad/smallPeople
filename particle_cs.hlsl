#include "particle.hlsli"

RWStructuredBuffer<particle> particle_buffer : register(u0);

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
	uint id = dtid.x;

	particle p = particle_buffer[id];
	if (delta_time == 0)
	{
		p.position = emitter_position;
		
	}
	if (p.age > 10.0)
	{
		const float g = -0.5;
		p.velocity.y += g * delta_time;
		p.position += p.velocity * delta_time;

		if (p.position.y < 0)
		{
			p.velocity = 0;
			p.position.y = 0;
		}
	}
	p.age += delta_time;

	particle_buffer[id] = p;
}

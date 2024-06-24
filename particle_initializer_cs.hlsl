#include "particle.hlsli"

RWStructuredBuffer<particle> particle_buffer : register(u0);

float rand(float n)
{
	return frac(sin(n) * 43758.5453123);
}

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
	uint id = dtid.x;

	particle p = particle_buffer[id];

	const float noise_scale = 1.0;
	float f0 = rand(id * noise_scale);
	float f1 = rand(f0 * noise_scale);
	float f2 = rand(f1 * noise_scale);

	p.position = emitter_position;

	p.velocity.x = 0.5 * sin(2 * 3.14 * f0);
	p.velocity.y = 2.0 * f1;
	p.velocity.z = 0.5 * cos(2 * 3.14 * f0);

	p.color.x = 1.0;
	p.color.y = f0 * 0.5;
	p.color.z = f0 * 0.05;
	p.color.xyz *= 3.0;
	p.color.w = 1.0f;

	p.age = 10.0 * f2;
	p.state = 0;
	
	particle_buffer[id] = p;
}

// snowParticle.cs
// initParticle.cs
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

	p.position.x = emitter_position.x + 10.0 * (f0 - 0.5);
	p.position.y = emitter_position.y + 20.0 * (f1 - 0.5) + 20.0; // Emit from a higher position
	p.position.z = emitter_position.z + 10.0 * (f2 - 0.5);

	p.velocity.x = 0.1 * (f0 - 0.5);
	p.velocity.y = -0.05; // Constant downward velocity
	p.velocity.z = 0.05 * (f2 - 0.5);

	p.color = float4(1.0, 1.0, 1.0, 1.0);

	p.age = rand(f2) * 10.0;
	p.state = 0;

	particle_buffer[id] = p;
}
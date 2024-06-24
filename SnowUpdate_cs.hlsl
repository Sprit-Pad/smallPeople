// updateParticle.cs
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

	const float g = -0.08;
	p.velocity.y += (g/1.2)* delta_time;
	p.position += p.velocity * delta_time;

    if (p.position.y < emitter_position.y)
    {
        p.position.y = 0;  // Keep particle on the ground
        p.velocity = float3(0.0, 0.0, 0.0);  // Stop the particle

        // Start a timer for resetting the particle
        if (p.state == 0)
        {
            p.state = 1;  // Change state to indicate particle is waiting to be reset
            p.age = 0.0;  // Reset age to count the wait time
        }
    }

    if (p.state == 1 && p.age > 2.0)  // Wait for 2 seconds before resetting
    {
        // Reset particle to reappear from the top
        const float noise_scale = 1.0;
        float f0 = rand(id * noise_scale);
        float f1 = rand(f0 * noise_scale);
        float f2 = rand(f1 * noise_scale);

        p.position.x = emitter_position.x + 10.0 * (f0 - 0.5);
        p.position.y = emitter_position.y + 5.0 * (f1 - 0.5) + 5.0; // Emit from a higher position
        p.position.z = emitter_position.z + 10.0 * (f2 - 0.5);

        p.velocity.x = 0.1 * (f0 - 0.5); // Slow down the horizontal speed
        p.velocity.y = -0.05; // Slower constant downward velocity
        p.velocity.z = 0.05 * (f2 - 0.5); // Slow down the horizontal speed


        p.age = rand(f2) * 10.0;  // Reset age
        p.state = 0;  // Reset state
    }
	if(delta_time==0)
	{
		const float noise_scale = 1.0;
		float f0 = rand(id * noise_scale);
		float f1 = rand(f0 * noise_scale);
		float f2 = rand(f1 * noise_scale);
		p.velocity.x = 0;
		p.velocity.y = 0; // Constant downward velocity
		p.velocity.z = 0;
		p.position.x = emitter_position.x + 10.0 * (f0 - 0.5);
		p.position.y = emitter_position.y + 10.0 * (f1 - 0.5) + 10.0; // Emit from a higher position
		p.position.z = emitter_position.z + 10.0 * (f2 - 0.5);
	}
	p.age += delta_time;

	particle_buffer[id] = p;
}
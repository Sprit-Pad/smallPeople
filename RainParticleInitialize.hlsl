// RainParticleInitialize.hlsl
#include "particle.hlsli"

float rand(float n)
{
    return frac(sin(n) * 43758.5453123);
}

RWStructuredBuffer<particle> particles : register(u0);

[numthreads(NUMTHREADS_X, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    uint index = id.x;
    const float noise_scale = 1.0;
    float f0 = rand(id * noise_scale);
    float f1 = rand(f0 * noise_scale);
    float f2 = rand(f1 * noise_scale);
    // パーティクルを初期化
    particle p;
    p.position.x = emitter_position.x + 10.0 * (f0 - 0.5);
    p.position.y = emitter_position.y + 5.0 * (f1 - 0.5) + 5.0; // Emit from a higher position
    p.position.z = emitter_position.z + 10.0 * (f2 - 0.5);
    
    p.velocity = float3(0.0, -1.0, 0.0); // 雨のパーティクルは下方向に落ちる
    p.color = float4(0.5, 0.5, 1.0, 1.0); // 青い色
    p.age = 0.0;
    p.state = 1;


    // 初期化したパーティクルをバッファに書き込む
    particles[index] = p;
}
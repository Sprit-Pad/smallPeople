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

    // パーティクルを取得
    particle p = particles[index];

    // リセットフラグが立っている場合、初期化シェーダーを呼び出す
    if (reset_flag == 1 || p.state == 0)
    {
        const float noise_scale = 1.0;
        float f0 = rand(id.x * noise_scale);
        float f1 = rand(f0 * noise_scale);
        float f2 = rand(f1 * noise_scale);
        float f3 = rand(f2 * noise_scale);
        float f4 = rand(f3 * noise_scale);
        float f5 = rand(f4 * noise_scale);

        // パーティクルの初期化
        p.position.x = emitter_position.x + 10.0 * (f0 - 0.5);
        p.position.y = emitter_position.y + 5.0 * (f1 - 0.5) + 5.0; // Emit from a higher position
        p.position.z = emitter_position.z + 10.0 * (f2 - 0.5);

        p.velocity = float3((f3 - 0.5) * 2.0, (f4 - 0.5) * 2.0, (f5 - 0.5) * 2.0); // ランダムな速度
        p.color = float4(f0, f1, f2, 1.0); // ランダムな色
        p.age = 0.0;
        p.state = 1;
    }
    else
    {
        // パーティクルの更新
        p.velocity.y -= 0.8 * delta_time; // 重力加速度を適用
        p.position += p.velocity * delta_time;
        p.age += delta_time;

        // パーティクルが地面に到達した場合リセット
        if (p.position.y < emitter_position.y - 5.0) // 地面より下に落ちた場合
        {
            p.state = 0;
        }
    }

    // 更新したパーティクルをバッファに書き戻す
    particles[index] = p;
}
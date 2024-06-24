// RainParticleUpdate.hlsl
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

    // �p�[�e�B�N�����擾
    particle p = particles[index];

    // ���Z�b�g�t���O�������Ă���ꍇ�A�������V�F�[�_�[���Ăяo��
    if (reset_flag == 1 || p.state == 0)
    {
        const float noise_scale = 1.0;
        float f0 = rand(id * noise_scale);
        float f1 = rand(f0 * noise_scale);
        float f2 = rand(f1 * noise_scale);
        // �p�[�e�B�N���̏�����
        p.position.x = emitter_position.x + 10.0 * (f0 - 0.5);
        p.position.y = emitter_position.y + 5.0 * (f1 - 0.5) + 5.0; // Emit from a higher position
        p.position.z = emitter_position.z + 10.0 * (f2 - 0.5);
       
        p.velocity = float3(0.0, -1.0, 0.0); // �J�̃p�[�e�B�N���͉������ɗ�����
        p.color = float4(0.5, 0.5, 1.0, 1.0); // ���F
        p.age = 0.0;
        p.state = 1;

    }
    else
    {
        // �p�[�e�B�N���̍X�V
        p.velocity.y -= 9.8 * delta_time; // �d�͉����x��K�p
        p.position += p.velocity * delta_time;
        p.age += delta_time;

        // �p�[�e�B�N�����n�ʂɓ��B�����ꍇ���Z�b�g
        if (p.position.y < emitter_position.y)
        {
            p.state = 0;
        }
    }

    // �X�V�����p�[�e�B�N�����o�b�t�@�ɏ����߂�
    particles[index] = p;
}
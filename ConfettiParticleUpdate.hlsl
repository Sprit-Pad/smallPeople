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
        float f0 = rand(id.x * noise_scale);
        float f1 = rand(f0 * noise_scale);
        float f2 = rand(f1 * noise_scale);
        float f3 = rand(f2 * noise_scale);
        float f4 = rand(f3 * noise_scale);
        float f5 = rand(f4 * noise_scale);

        // �p�[�e�B�N���̏�����
        p.position.x = emitter_position.x + 10.0 * (f0 - 0.5);
        p.position.y = emitter_position.y + 5.0 * (f1 - 0.5) + 5.0; // Emit from a higher position
        p.position.z = emitter_position.z + 10.0 * (f2 - 0.5);

        p.velocity = float3((f3 - 0.5) * 2.0, (f4 - 0.5) * 2.0, (f5 - 0.5) * 2.0); // �����_���ȑ��x
        p.color = float4(f0, f1, f2, 1.0); // �����_���ȐF
        p.age = 0.0;
        p.state = 1;
    }
    else
    {
        // �p�[�e�B�N���̍X�V
        p.velocity.y -= 0.8 * delta_time; // �d�͉����x��K�p
        p.position += p.velocity * delta_time;
        p.age += delta_time;

        // �p�[�e�B�N�����n�ʂɓ��B�����ꍇ���Z�b�g
        if (p.position.y < emitter_position.y - 5.0) // �n�ʂ�艺�ɗ������ꍇ
        {
            p.state = 0;
        }
    }

    // �X�V�����p�[�e�B�N�����o�b�t�@�ɏ����߂�
    particles[index] = p;
}
#include "sky_box.hlsli"

VS_OUT main(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD)
{
    VS_OUT vout;
    position.z = 1.0f; //  ��ԉ��̐[�x�ŕ`��
    vout.position = position;
    vout.color = color;

    //  NDC���W���烏�[���h���W�֕ϊ�
    vout.world_position = mul(position, inverse_view_projection);
    vout.world_position /= vout.world_position.w;

    return vout;
}

#include "sky_box.hlsli"

VS_OUT main(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD)
{
    VS_OUT vout;
    position.z = 1.0f; //  一番奥の深度で描画
    vout.position = position;
    vout.color = color;

    //  NDC座標からワールド座標へ変換
    vout.world_position = mul(position, inverse_view_projection);
    vout.world_position /= vout.world_position.w;

    return vout;
}

//sky_box.hlsli
struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float4 world_position : WORLD_POSITION;
};

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 view_projection;
    float4 light_direction; //ƒ‰ƒCƒg‚ÌŒü‚«
    row_major float4x4 view;
    float4 options;
    float4 camera_position;
    row_major float4x4 inverse_view_projection;
};

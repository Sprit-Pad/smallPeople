//particle.hlsli
cbuffer PARTICLE_CONSTANTS : register(b9)
{
	float3 emitter_position;
	float particle_size;
	float time;
	float delta_time;
	int reset_flag; // New flag to trigger reset
};

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
	row_major float4x4 view_projection;
	float4 light_direction;
	float4 camera_position;
};

struct VS_OUT
{
	uint vertex_id : VERTEXID;
};
struct GS_OUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

struct particle
{
	float4 color;
	float3 position;
	float3 velocity;
	float age;
	int state;
	//float scale; // スケールパラメータ
	//float length; // 長さパラメータ
};

#define NUMTHREADS_X 16

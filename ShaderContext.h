
#pragma once

#include <DirectXMath.h>

// ポストエフェクト用定数バッファ
struct parametric_constants
{
	float luminance_extraction_lower_edge{ 0.6f };
	float luminance_extraction_higher_edge{ 0.8f };
	float gaussian_sigma{ 1.0f };
	float bloom_intensity{ 1.0f };
	float exposure{ 1.0f };
	DirectX::XMFLOAT3 dummy2;

};
struct PixelShaderConstants
{
	float bloomIntensity;
};
// WATER_SIMULATION
struct wave_parameter
{
	float A{ 0.01f }; // Amplitude 
	float L{ 0.1f }; // Wavelength 
	float S{ 0.1f }; // Speed
	float Q{ 0.5f }; // Steepness
	DirectX::XMFLOAT2 D{ 1, 0 }; // Direction
	float padding[2];
};
struct wave_simulation
{
	float time;
	float time_delta;
	float padding[2];
	wave_parameter wave_parameters[3];
};

struct ShaderContext{
// レンダーコンテキスト
struct scene_constants
{
	DirectX::XMFLOAT4X4 view_projection; //ビュー・プロジェクション変換行列
	DirectX::XMFLOAT4 light_direction; //ライトの向き
	DirectX::XMFLOAT4X4		view;
	DirectX::XMFLOAT4 options;	//	xy : マウスの座標値, z : タイマー, w : フラグ
	// 
	DirectX::XMFLOAT4 camera_position;//カメラ位置
	DirectX::XMFLOAT4X4 inverse_view_projection;
	//

};
parametric_constants parametric_constant;

PixelShaderConstants pixselShaderconstans;
wave_simulation wave_simulation_data
{
	0.0f, 0.f, { 0.0f, 0.0f },
	{
		{ 0.01f, 0.437f, 0.1f, 0.5f, { 0.245f, 0.065f } },
		{ 0.043f, 0.9f, 0.2f, 0.5f, { 1.0f, -1.0f } },
		{ 0.03f, 0.571f, 0.3f, 0.5f, { 0.208f, -0.402f } },
	}
};
};
#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

#include <vector>
#include "skinned_mesh.h"


#define NUMTHREADS_X 16

struct Lparticle
{
	DirectX::XMFLOAT4 color{ 1, 1, 1, 1 };
	DirectX::XMFLOAT3 position{ 0, 0, 0 };
	DirectX::XMFLOAT3 velocity{ 0, 0, 0 };
	float age{};
	int state{};
};
struct particles
{
	const size_t max_particle_count;
	struct particle_constants
	{
		DirectX::XMFLOAT3 emitter_position{};
		float particle_size{ 0.02f };
		float time{};
		float delta_time{};
		float something[2];
	};
	particle_constants particle_data;

	Microsoft::WRL::ComPtr<ID3D11Buffer> particle_buffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particle_buffer_uav;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particle_buffer_srv;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> particle_vs;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> particle_ps;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> particle_gs;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> particle_cs[8];
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> particle_initializer_cs[8];
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

	particles(ID3D11Device* pDevice, size_t particle_count);
	particles(const particles&) = delete;
	particles& operator=(const particles&) = delete;
	particles(particles&&) noexcept = delete;
	particles& operator=(particles&&) noexcept = delete;
	virtual ~particles() = default;

	void integrate(ID3D11DeviceContext* immediate_context, float delta_time, int set,int boolset, DirectX::XMFLOAT3 position);
	void initialize(ID3D11DeviceContext* immediate_context, float delta_time, int set, DirectX::XMFLOAT3 position);
	void render(ID3D11DeviceContext* immediate_context);
};
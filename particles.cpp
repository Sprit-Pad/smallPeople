#include "particles.h"

#include "shader.h"
#include "misc.h"

using namespace DirectX;

particles::particles(ID3D11Device* device, size_t particle_count) : max_particle_count(particle_count)
{
	HRESULT hr{ S_OK };
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(Lparticle) * particle_count);
	buffer_desc.StructureByteStride = sizeof(Lparticle);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	hr = device->CreateBuffer(&buffer_desc, NULL, particle_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc;
	shader_resource_view_desc.Format = DXGI_FORMAT_UNKNOWN;
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	shader_resource_view_desc.Buffer.ElementOffset = 0;
	shader_resource_view_desc.Buffer.NumElements = static_cast<UINT>(particle_count);
	hr = device->CreateShaderResourceView(particle_buffer.Get(), &shader_resource_view_desc, particle_buffer_srv.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_UNORDERED_ACCESS_VIEW_DESC unordered_access_view_desc;
	unordered_access_view_desc.Format = DXGI_FORMAT_UNKNOWN;
	unordered_access_view_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	unordered_access_view_desc.Buffer.FirstElement = 0;
	unordered_access_view_desc.Buffer.NumElements = static_cast<UINT>(particle_count);
	unordered_access_view_desc.Buffer.Flags = 0;
	hr = device->CreateUnorderedAccessView(particle_buffer.Get(), &unordered_access_view_desc, particle_buffer_uav.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	buffer_desc.ByteWidth = sizeof(particle_constants); 
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	// ここでパーティクルデータの初期化
	particle_data = {};
	create_vs_from_cso(device, "particle_vs.cso", particle_vs.ReleaseAndGetAddressOf(), NULL, NULL, 0);
	create_ps_from_cso(device, "particle_ps.cso", particle_ps.ReleaseAndGetAddressOf());
	create_gs_from_cso(device, "particle_gs.cso", particle_gs.ReleaseAndGetAddressOf());
	create_cs_from_cso(device, "particle_cs.cso", particle_cs[0].ReleaseAndGetAddressOf());
	create_cs_from_cso(device, "particle_initializer_cs.cso", particle_initializer_cs[0].ReleaseAndGetAddressOf());
	create_cs_from_cso(device, "snowParticle.cso", particle_initializer_cs[1].ReleaseAndGetAddressOf());
	create_cs_from_cso(device, "SnowUpdate_cs.cso", particle_cs[1].ReleaseAndGetAddressOf());
	create_cs_from_cso(device, "RainParticleInitialize.cso", particle_initializer_cs[2].ReleaseAndGetAddressOf());
	create_cs_from_cso(device, "RainParticleUpdate.cso", particle_cs[2].ReleaseAndGetAddressOf());
	create_cs_from_cso(device, "ConfettiParticleInitialize.cso", particle_initializer_cs[3].ReleaseAndGetAddressOf());
	create_cs_from_cso(device, "ConfettiParticleUpdate.cso", particle_cs[3].ReleaseAndGetAddressOf());
}

UINT align1(UINT num, UINT alignment)
{
	return (num + (alignment - 1)) & ~(alignment - 1);
}
void particles::integrate(ID3D11DeviceContext* immediate_context, float delta_time, int set,int boolset, DirectX::XMFLOAT3 position)
{
	immediate_context->CSSetUnorderedAccessViews(0, 1, particle_buffer_uav.GetAddressOf(), NULL);

	immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &particle_data, 0, 0);
	immediate_context->CSSetConstantBuffers(9, 1, constant_buffer.GetAddressOf());
	if (!set == boolset) 
	{
		particle_data = {};

	}
	else {
	    particle_data.time += delta_time;
	particle_data.delta_time = delta_time;
	particle_data.emitter_position = position;
		immediate_context->CSSetShader(particle_cs[set].Get(), NULL, 0);
	}
	const UINT thread_group_count_x = align1(static_cast<UINT>(max_particle_count), NUMTHREADS_X) / NUMTHREADS_X;
	immediate_context->Dispatch(thread_group_count_x, 1, 1);

	ID3D11UnorderedAccessView* null_unordered_access_view{};
	immediate_context->CSSetUnorderedAccessViews(0, 1, &null_unordered_access_view, NULL);
}
void particles::initialize(ID3D11DeviceContext* immediate_context, float delta_time,int set, DirectX::XMFLOAT3 position)
{
	immediate_context->CSSetUnorderedAccessViews(0, 1, particle_buffer_uav.GetAddressOf(), NULL);

	particle_data.time = delta_time;
	particle_data.delta_time = delta_time;
	particle_data.emitter_position = position;
	immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &particle_data, 0, 0);
	immediate_context->CSSetConstantBuffers(9, 1, constant_buffer.GetAddressOf());
	
	immediate_context->CSSetShader(particle_initializer_cs[set].Get(), NULL, 0); 


	const UINT thread_group_count_x = align1(static_cast<UINT>(max_particle_count), NUMTHREADS_X) / NUMTHREADS_X;
	immediate_context->Dispatch(thread_group_count_x, 1, 1);

	ID3D11UnorderedAccessView* null_unordered_access_view{};
	immediate_context->CSSetUnorderedAccessViews(0, 1, &null_unordered_access_view, NULL);
}
void particles::render(ID3D11DeviceContext* immediate_context)
{
	immediate_context->VSSetShader(particle_vs.Get(), NULL, 0);
	immediate_context->PSSetShader(particle_ps.Get(), NULL, 0);
	immediate_context->GSSetShader(particle_gs.Get(), NULL, 0);
	immediate_context->GSSetShaderResources(9, 1, particle_buffer_srv.GetAddressOf());

	immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &particle_data, 0, 0);
	immediate_context->VSSetConstantBuffers(9, 1, constant_buffer.GetAddressOf());
	immediate_context->PSSetConstantBuffers(9, 1, constant_buffer.GetAddressOf());
	immediate_context->GSSetConstantBuffers(9, 1, constant_buffer.GetAddressOf());

	immediate_context->IASetInputLayout(NULL);
	immediate_context->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	immediate_context->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	immediate_context->Draw(static_cast<UINT>(max_particle_count), 0);

	ID3D11ShaderResourceView* null_shader_resource_view{};
	immediate_context->GSSetShaderResources(9, 1, &null_shader_resource_view);
	immediate_context->VSSetShader(NULL, NULL, 0);
	immediate_context->PSSetShader(NULL, NULL, 0);
	immediate_context->GSSetShader(NULL, NULL, 0);
}

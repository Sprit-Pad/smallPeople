
#include "sprite_batch.h"
#include "misc.h"
#include <sstream>
#include <WICTextureLoader.h>
#include "texture.h"
#include "shader.h"


sprite_batch::sprite_batch(ID3D11Device* device, const wchar_t* filename, size_t max_sprites)
	: max_vertices(max_sprites * 6)
{
	//vertex vertices[]
	//{
	//	{ {-0.5,+0.5, 0}, {1,1,1,1}, {0, 0} },
	//	{ {+0.5,+0.5, 0}, {1,0,0,1}, {1, 0} },
	//	{ {-0.5,-0.5, 0}, {0,1,0,1}, {0, 1} },
	//	{ {+0.5,-0.5, 0}, {0,0,1,1}, {1, 1} },
	//};
	std::unique_ptr<vertex[]> vertices{ std::make_unique<vertex[]>(max_vertices) };

	HRESULT hr{ S_OK };

	// 頂点バッファオブジェクトの設定を行うための構造体
	D3D11_BUFFER_DESC buffer_desc{};
	// 頂点情報のサイズを指定
	buffer_desc.ByteWidth = sizeof(vertex) * max_vertices;
	// バッファの読み取りと書き込みの方法を特定。GPU からしかアクセスできないよう設定
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	// バッファがパイプラインにバインドされる方法を特定
	// このバッファオブジェクトを頂点バッファとして設定
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	// CPU へアクセスする際のフラグ。CPU のアクセスを使ってないのでここでは０
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;

	// サブリソースデータ
	D3D11_SUBRESOURCE_DATA subresource_data{};
	// 初期化データへのポインタ
	// 頂点情報を設定
	subresource_data.pSysMem = vertices.get();
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;

	// デバイスを使って、頂点バッファのサブリソースとして頂点情報を設定して頂点バッファを生成
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	// 入力レイアウトオブジェクトの設定を行うための構造体
	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	create_vs_from_cso(device, "sprite_vs.cso", vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));

	create_ps_from_cso(device, "sprite_ps.cso", pixel_shader.GetAddressOf());

	load_texture_from_file(device, filename, shader_resource_view.GetAddressOf(), &texture2d_desc);
}

sprite_batch::~sprite_batch()
{
}

void sprite_batch::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy, float dw, float dh)
{
	render(immediate_context,
		dx, dy, dw, dh,
		1, 1, 1, 1,
		0);
}

void sprite_batch::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy, float dw, float dh,
	float r, float g, float b, float a,
	float angle
)
{
	render(immediate_context,
		dx, dy, dw, dh,
		r, g, b, a,
		angle,
		0, 0,
		static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void sprite_batch::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy, float dw, float dh,
	float r, float g, float b, float a,
	float angle,
	float sx, float sy, float sw, float sh)
{
	D3D11_VIEWPORT viewport{};
	UINT num_viewports{ 1 };
	immediate_context->RSGetViewports(&num_viewports, &viewport);

	// 左上の頂点
	float x0(dx);
	float y0(dy);
	// 右上の頂点
	float x1(dx + dw);
	float y1(dy);
	// 左下の頂点
	float x2(dx);
	float y2(dy + dh);
	// 右下の頂点
	float x3(dx + dw);
	float y3(dy + dh);

	// 回転の計算
	float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
	float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
	float cx = dx + dw * 0.5f;
	float cy = dy + dh * 0.5f;
	rotate(x0, y0, cx, cy, sin, cos);
	rotate(x1, y1, cx, cy, sin, cos);
	rotate(x2, y2, cx, cy, sin, cos);
	rotate(x3, y3, cx, cy, sin, cos);

	// スクリーン座標系から NDC 座標系に変換
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;
	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;
	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;
	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;

	float u0{ sx / texture2d_desc.Width };
	float v0{ sy / texture2d_desc.Height };
	float u1{ (sx + sw) / texture2d_desc.Width };
	float v1{ (sy + sh) / texture2d_desc.Height };

	vertices.push_back({ { x0, y0, 0 }, { r,g,b,a }, { u0,v0 } });
	vertices.push_back({ { x1, y1, 0 }, { r,g,b,a }, { u1,v0 } });
	vertices.push_back({ { x2, y2, 0 }, { r,g,b,a }, { u0,v1 } });
	vertices.push_back({ { x2, y2, 0 }, { r,g,b,a }, { u0,v1 } });
	vertices.push_back({ { x1, y1, 0 }, { r,g,b,a }, { u1,v0 } });
	vertices.push_back({ { x3, y3, 0 }, { r,g,b,a }, { u1,v1 } });
}

void sprite_batch::begin(ID3D11DeviceContext* immediate_context, ID3D11PixelShader* replaced_pixel_shader, ID3D11ShaderResourceView* replaced_shader_resource_view)
{
	vertices.clear();
	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);

	replaced_pixel_shader ? immediate_context->PSSetShader(replaced_pixel_shader, nullptr, 0) : immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

	if (replaced_shader_resource_view)
	{
		// 差し替えようのテクスチャ情報を取得
		HRESULT hr{ S_OK };
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		replaced_shader_resource_view->GetResource(resource.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		hr = resource->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		texture2d->GetDesc(&texture2d_desc);

		// 差し替えようのテクスチャを設定
		immediate_context->PSSetShaderResources(0, 1, &replaced_shader_resource_view);
	}
	else
	{
		immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
	}
}

void sprite_batch::end(ID3D11DeviceContext* immediate_context)
{
	HRESULT hr{ S_OK };

	// GPU 側のメモリにある頂点バッファを取り出す
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	size_t vertex_count = vertices.size();
	_ASSERT_EXPR(max_vertices >= vertex_count, "Buffer overflow");
	vertex* data{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
	if (data != nullptr)
	{
		const vertex* p = vertices.data();
		memcpy_s(data, max_vertices * sizeof(vertex), p, vertex_count * sizeof(vertex));
	}

	immediate_context->Unmap(vertex_buffer.Get(), 0);

	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	immediate_context->IASetInputLayout(input_layout.Get());

	immediate_context->Draw(static_cast<UINT>(vertex_count), 0);
}

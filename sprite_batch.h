#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <wrl.h>



class sprite_batch
{
	// 頂点フォーマット
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

private:
	// 頂点シェーダー
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	// ピクセルシェーダー
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;

	// GPU 側でテクスチャの色情報を利用できるようにする
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	// テクスチャの一般情報（幅とか高さとか）
	D3D11_TEXTURE2D_DESC texture2d_desc;

	const size_t max_vertices;
	std::vector<vertex> vertices;

public:
	sprite_batch(ID3D11Device* device, const wchar_t* filename, size_t max_sprites = 1);
	virtual ~sprite_batch();

	void render(ID3D11DeviceContext* immediate_context,
		float dx, float dy, float dw, float dh);

	// 描画
	void render(ID3D11DeviceContext* immediate_context,
		float dx, float dy, float dw, float dh,
		float r, float g, float b, float a,
		float angle
	);

	// 描画
	void render(ID3D11DeviceContext* immediate_context,
		float dx, float dy, float dw, float dh,
		float r, float g, float b, float a,
		float angle,
		float sx, float sy, float sw, float sh
	);
	
	void begin(ID3D11DeviceContext* immediate_context, ID3D11PixelShader* replaced_pixel_shader = nullptr, ID3D11ShaderResourceView* replaced_shader_resource_view = nullptr);
	void end(ID3D11DeviceContext* immediate_context);

private:
	// 頂点１つ分のアフィン行列回転の計算
	static void rotate(float& x, float& y, float cx, float cy, float sin, float cos)
	{
		x -= cx;
		y -= cy;

		float tx{ x }, ty{ y };
		x = cos * tx + -sin * ty;
		y = sin * tx + cos * ty;

		x += cx;
		y += cy;
	};
};
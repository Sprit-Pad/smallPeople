#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>

#include <string>
#include <unordered_map>
#include <fstream>
class sprite
{
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

public:
	// 頂点フォーマット
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

	sprite(ID3D11Device* device, const wchar_t* filename, bool skyBox);
	virtual ~sprite();

	void render(ID3D11DeviceContext* immediate_context,
		float dx, float dy, float dw, float dh);

	void textout(ID3D11DeviceContext* immediate_context, std::string s/*std::u32string s*/, float x, float y, float w, float h, float r, float g, float b, float a);

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
	void loadFontData(const std::string& fntFile);

	// 他のメンバ変数と関数...
	// 文字情報を格納する構造体
	struct FontChar {
		float u, v, w, h;
	};
	std::unordered_map<char32_t, FontChar> fontMap;
};
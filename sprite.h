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
	// ���_�V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	// �s�N�Z���V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	// ���̓��C�A�E�g
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	// ���_�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;

	// GPU ���Ńe�N�X�`���̐F���𗘗p�ł���悤�ɂ���
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	// �e�N�X�`���̈�ʏ��i���Ƃ������Ƃ��j
	D3D11_TEXTURE2D_DESC texture2d_desc;

public:
	// ���_�t�H�[�}�b�g
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

	// �`��
	void render(ID3D11DeviceContext* immediate_context,
		float dx, float dy, float dw, float dh,
		float r, float g, float b, float a,
		float angle
	);

	// �`��
	void render(ID3D11DeviceContext* immediate_context,
		float dx, float dy, float dw, float dh,
		float r, float g, float b, float a,
		float angle,
		float sx, float sy, float sw, float sh
	);
	void loadFontData(const std::string& fntFile);

	// ���̃����o�ϐ��Ɗ֐�...
	// ���������i�[����\����
	struct FontChar {
		float u, v, w, h;
	};
	std::unordered_map<char32_t, FontChar> fontMap;
};
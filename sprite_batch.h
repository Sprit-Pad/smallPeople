#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <wrl.h>



class sprite_batch
{
	// ���_�t�H�[�}�b�g
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

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

	const size_t max_vertices;
	std::vector<vertex> vertices;

public:
	sprite_batch(ID3D11Device* device, const wchar_t* filename, size_t max_sprites = 1);
	virtual ~sprite_batch();

	void render(ID3D11DeviceContext* immediate_context,
		float dx, float dy, float dw, float dh);

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
	
	void begin(ID3D11DeviceContext* immediate_context, ID3D11PixelShader* replaced_pixel_shader = nullptr, ID3D11ShaderResourceView* replaced_shader_resource_view = nullptr);
	void end(ID3D11DeviceContext* immediate_context);

private:
	// ���_�P���̃A�t�B���s���]�̌v�Z
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
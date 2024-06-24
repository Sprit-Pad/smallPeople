
#include "sprite.h"
#include "misc.h"
#include <sstream>
#include <WICTextureLoader.h>

#include "texture.h"
#include "shader.h"
#include "imgui/imgui.h"



sprite::sprite(ID3D11Device* device, const wchar_t* filename, bool skyBox)
{
	vertex vertices[]
	{
		{ {-0.5,+0.5, 0}, {1,1,1,1}, {0, 0} },
		{ {+0.5,+0.5, 0}, {1,0,0,1}, {1, 0} },
		{ {-0.5,-0.5, 0}, {0,1,0,1}, {0, 1} },
		{ {+0.5,-0.5, 0}, {0,0,1,1}, {1, 1} },
	};

	HRESULT hr{ S_OK };

	// ���_�o�b�t�@�I�u�W�F�N�g�̐ݒ���s�����߂̍\����
	D3D11_BUFFER_DESC buffer_desc{};
	// ���_���̃T�C�Y���w��
	buffer_desc.ByteWidth = sizeof(vertices);
	// �o�b�t�@�̓ǂݎ��Ə������݂̕��@�����BGPU ���炵���A�N�Z�X�ł��Ȃ��悤�ݒ�
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	// �o�b�t�@���p�C�v���C���Ƀo�C���h�������@�����
	// ���̃o�b�t�@�I�u�W�F�N�g�𒸓_�o�b�t�@�Ƃ��Đݒ�
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	// CPU �փA�N�Z�X����ۂ̃t���O�BCPU �̃A�N�Z�X���g���ĂȂ��̂ł����ł͂O
			buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;

	// �T�u���\�[�X�f�[�^
	D3D11_SUBRESOURCE_DATA subresource_data{};
	// �������f�[�^�ւ̃|�C���^
	// ���_����ݒ�
	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;

	// �f�o�C�X���g���āA���_�o�b�t�@�̃T�u���\�[�X�Ƃ��Ē��_����ݒ肵�Ē��_�o�b�t�@�𐶐�
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// ���̓��C�A�E�g�I�u�W�F�N�g�̐ݒ���s�����߂̍\����
	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	if(!skyBox) {
		create_vs_from_cso(device, "sprite_vs.cso", vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));

		create_ps_from_cso(device, "sprite_ps.cso", pixel_shader.GetAddressOf());
	}
	if (skyBox) {
		//	��`��p�V�F�[�_�[
		create_vs_from_cso(device, "sky_box_vs.cso", vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, _countof(input_element_desc));
		create_ps_from_cso(device, "sky_box_ps.cso", pixel_shader.GetAddressOf());
	}
	load_texture_from_file(device, filename, shader_resource_view.GetAddressOf(), &texture2d_desc);
	
	
}

sprite::~sprite()
{
}

void sprite::render(ID3D11DeviceContext* immediate_context, 
	float dx, float dy, float dw, float dh)
{
	render(immediate_context,
		dx, dy, dw, dh,
		1, 1, 1, 1,
		0);
}

void sprite::textout(ID3D11DeviceContext* immediate_context, std::string s, float x, float y, float w, float h, float r, float g, float b, float a)
{

	// �ꕶ�����̕��ƍ������v�Z
	float sw = static_cast<float>(texture2d_desc.Width / 16);
	float sh = static_cast<float>(texture2d_desc.Height / 16);
	//// ���݂̕����ʒu(���Έʒu)
	float carriage = 0;

	// ������������ render() ���Ăяo���B
	for (const char c : s) {

		LONG sx = c % 0x0F;
		// ������\���B�A�X�L�[�R�[�h�̈ʒu�ɂ��镶���ʒu��؂蔲���ĕ\��
		render(immediate_context, x + carriage, y, w, h, r, g, b, a, 0, sw * (c & 0x0F), sh * (c >> 4), sw, sh);
		// �����ʒu�𕝕����炷
		carriage += w;
		/*ImGui::InputText("string", "", IM_ARRAYSIZE(&carriage));*/
	}
	/*float carriage = 0;

	for (const char32_t c : s) {
		if (fontMap.find(c) != fontMap.end()) {
			const FontChar& fontChar = fontMap[c];
			float sw = fontChar.w;
			float sh = fontChar.h;
			render(immediate_context, x + carriage, y, w, h, r, g, b, a, 0, fontChar.u, fontChar.v, sw, sh);
			carriage += w;
		}
	}*/

}





void sprite::render(ID3D11DeviceContext* immediate_context,
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

void sprite::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy, float dw, float dh, 
	float r, float g, float b, float a, 
	float angle, 
	float sx, float sy, float sw, float sh)
{
	D3D11_VIEWPORT viewport{};
	UINT num_viewports{ 1 };
	immediate_context->RSGetViewports(&num_viewports, &viewport);

	// ����̒��_
	float x0(dx);
	float y0(dy);
	// �E��̒��_
	float x1(dx + dw);
	float y1(dy);
	// �����̒��_
	float x2(dx);
	float y2(dy + dh);
	// �E���̒��_
	float x3(dx + dw);
	float y3(dy + dh);

	// ���_�P���̃A�t�B���s���]�̌v�Z
	auto rotate = [](float& x, float& y, float cx, float cy, float angle)
	{
		x -= cx;
		y -= cy;

		float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
		float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
		float tx{ x }, ty{ y };
		x = cos * tx + -sin * ty;
		y = sin * tx + cos * ty;

		x += cx;
		y += cy;
	};

	float cx = dx + dw * 0.5f;
	float cy = dy + dh * 0.5f;
	rotate(x0, y0, cx, cy, angle);
	rotate(x1, y1, cx, cy, angle);
	rotate(x2, y2, cx, cy, angle);
	rotate(x3, y3, cx, cy, angle);

	// �X�N���[�����W�n���� NDC ���W�n�ɕϊ�
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;
	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;
	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;
	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;

	HRESULT hr{ S_OK };

	// GPU ���̃������ɂ��钸�_�o�b�t�@�����o��
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// void* �� vertex* �ɃL���X�g
	vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
	if (vertices != nullptr)
	{
		vertices[0].position = { x0, y0, 0 };
		vertices[1].position = { x1, y1, 0 };
		vertices[2].position = { x2, y2, 0 };
		vertices[3].position = { x3, y3, 0 };
		vertices[0].color = vertices[1].color =
			vertices[2].color = vertices[3].color = { r,g,b,a };

		vertices[0].texcoord = { sx / texture2d_desc.Width, sy / texture2d_desc.Height };
		vertices[1].texcoord = { (sx + sw) / texture2d_desc.Width, sy / texture2d_desc.Height };
		vertices[2].texcoord = { sx / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
		vertices[3].texcoord = { (sx + sw) / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
	}

	immediate_context->Unmap(vertex_buffer.Get(), 0);

	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	immediate_context->IASetInputLayout(input_layout.Get());

	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

	immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());

	immediate_context->Draw(4, 0);
}

void sprite::loadFontData(const std::string& fntFile)
{
	/*std::ifstream file(fntFile);
	std::string line;
	while (std::getline(file, line)) {
		if (line.find("char id=") != std::string::npos) {
			FontChar fontChar;
			char32_t id;
			sscanf(line.c_str(), "char id=%u x=%f y=%f width=%f height=%f", &id, &fontChar.u, &fontChar.v, &fontChar.w, &fontChar.h);
			fontMap[id] = fontChar;
		}
	}*/
}

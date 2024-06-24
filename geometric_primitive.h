#pragma once
#include <d3d11.h>
#include "sprite.h"
#include "sprite_batch.h"
#include <wrl.h>
#include <memory>
#include "shader.h"
#include "texture.h"
#include "misc.h"
#include <vector>
 class geometric_primitive
 {
	 public:
		 struct vertex
			 {
			 DirectX::XMFLOAT3 position;
			 DirectX::XMFLOAT3 normal;
			 };
		 struct constants
			 {
		
			
		 DirectX::XMFLOAT4X4 world;
		 DirectX::XMFLOAT4 material_color;
		 };
		
		 private:
			 Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
			Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
			
			 Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
			Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
			Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
			Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
			
			 public:
			
				 virtual ~geometric_primitive() = default;
				
				 void render(ID3D11DeviceContext * immediate_context,
				 const DirectX::XMFLOAT4X4 & world, const DirectX::XMFLOAT4 & material_color);
				
				 protected:
					 geometric_primitive(ID3D11Device * device);
					 void create_com_buffers(ID3D11Device * device, vertex * vertices, size_t vertex_count,
						 uint32_t * indices, size_t index_count);
				 };
   /// <summary>
	    /// 正立方体専用幾何プリミティブ
	    /// </summary>
	 class geometric_cube : public geometric_primitive
 {
	 public:
	geometric_cube(ID3D11Device* device);
	
 };
class geometric_cylinder : public geometric_primitive
{
public:
 // 第２引数はシリンダーの円を何分割するかの数を指定
 geometric_cylinder(ID3D11Device* device, uint32_t slices);
};

class geometric_sphere : public geometric_primitive
{
public:
	geometric_sphere(ID3D11Device* device, uint32_t slices, uint32_t stacks);
};

class geometric_capsule : public geometric_primitive
{
public:
	geometric_capsule(ID3D11Device* device, float mantle_height, const DirectX::XMFLOAT3& radius, uint32_t slices, uint32_t ellipsoid_stacks, uint32_t mantle_stacks);
};


















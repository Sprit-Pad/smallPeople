#include "framework.h"
#include "SceneManager.h"
#include "SceneTitle.h"
#include "SceneGame.h"
// 垂直同期間隔設定
static const int syncInterval = 1;

framework::framework(HWND hwnd) : hwnd(hwnd),graphics(hwnd), input(hwnd)
{
	
}

bool framework::initialize()
{

	//シーン初期化
	SceneManager::Instance().ChangeScene(std::make_unique<SceneTitle>());

	return true;
}

void framework::update(float elapsed_time/*Elapsed seconds from last frame*/)
{
	
	

	// 入力更新処理
	input.Update();

	// シーン更新処理
	SceneManager::Instance().Update(elapsed_time);
}

int pageNo = 0;
int counter = 0;

void framework::render(float elapsed_time/*Elapsed seconds from last frame*/)
{
	
	std::lock_guard<std::mutex>lock(graphics.GetMutex());

	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	//graphics.SetSamplar();
	
	// シーン描画処理
	SceneManager::Instance().Render();

	// IMGUIデモウインドウ描画（IMGUI機能テスト用）
	//ImGui::ShowDemoWindow();

	
	// バックバッファに描画した画を画面に表示する。
	graphics.GetSwapChain()->Present(syncInterval, 0);
}

bool framework::uninitialize()
{
	return true;
}

void framework::calculate_frame_stats()
{
	{
		if (++frames, (tictoc.time_stamp() - elapsed_time) >= 1.0f)
		{
			float fps = static_cast<float>(frames);
			std::wostringstream outs;
			outs.precision(6);
			outs << APPLICATION_NAME << L" : FPS : " << fps << L" / " << L"Frame Time : " << 1000.0f / fps << L" (ms)";
			SetWindowTextW(hwnd, outs.str().c_str());

			frames = 0;
			elapsed_time += 1.0f;
		}
	}
}

framework::~framework()
{
	//シーン終了化
	SceneManager::Instance().Clear();

}

int framework::run()
{
	{
		MSG msg{};

		if (!initialize())
		{
			return 0;
		}
#ifdef USE_IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 14.0f, nullptr, glyphRangesJapanese);
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(graphics.GetDevice(), graphics.GetDeviceContext());
		ImGui::StyleColorsDark();
#endif



		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				tictoc.tick();
				calculate_frame_stats();

				float elapsedTime = syncInterval == 0
					? tictoc.time_interval()
					: syncInterval / 60.0f
					;
				update(elapsedTime);
				render(elapsedTime);
			}
		}

#ifdef USE_IMGUI
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
#endif


		return graphics.uninitialize() ? static_cast<int>(msg.wParam) : 0;
	}
}

LRESULT framework::handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	{
#ifdef USE_IMGUI
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }
#endif
		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps{};
			BeginPaint(hwnd, &ps);

			EndPaint(hwnd, &ps);
		}
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE)
			{
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			}
			break;
		case WM_ENTERSIZEMOVE:
			tictoc.stop();
			break;
		case WM_EXITSIZEMOVE:
			tictoc.start();
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;
	}
}





//bool framework::createDeviceAndSwapChain(HWND hwnd, UINT screenWidth, UINT screenHeight, bool fullscreen) {
//	HRESULT hr = S_OK;
//	UINT create_device_flags = 0;
//
//#ifdef _DEBUG
//	create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
//#endif
//
//	D3D_FEATURE_LEVEL feature_levels = D3D_FEATURE_LEVEL_11_0;
//
//	DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
//	swap_chain_desc.BufferCount = 1;
//	swap_chain_desc.BufferDesc.Width = screenWidth;
//	swap_chain_desc.BufferDesc.Height = screenHeight;
//	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
//	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
//	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//	swap_chain_desc.OutputWindow = hwnd;
//	swap_chain_desc.SampleDesc.Count = 1;
//	swap_chain_desc.SampleDesc.Quality = 0;
//	swap_chain_desc.Windowed = !fullscreen;
//
//	hr = D3D11CreateDeviceAndSwapChain(
//		NULL,
//		D3D_DRIVER_TYPE_HARDWARE,
//		NULL,
//		create_device_flags,
//		&feature_levels,
//		1,
//		D3D11_SDK_VERSION,
//		&swap_chain_desc,
//		swap_chain.GetAddressOf(),
//		device.GetAddressOf(),
//		NULL,
//		immediate_context.GetAddressOf());
//
//	return SUCCEEDED(hr);
//}
//
//bool framework::createRenderTargetView() {
//	HRESULT hr = S_OK;
//	Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer;
//	hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(back_buffer.GetAddressOf()));
//	if (FAILED(hr)) return false;
//
//	hr = device->CreateRenderTargetView(back_buffer.Get(), NULL, render_target_view.GetAddressOf());
//	return SUCCEEDED(hr);
//}
//
//bool framework::createDepthStencilView(UINT screenWidth, UINT screenHeight) {
//	HRESULT hr = S_OK;
//	Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer;
//	D3D11_TEXTURE2D_DESC texture2d_desc = {};
//	texture2d_desc.Width = screenWidth;
//	texture2d_desc.Height = screenHeight;
//	texture2d_desc.MipLevels = 1;
//	texture2d_desc.ArraySize = 1;
//	texture2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
//	texture2d_desc.SampleDesc.Count = 1;
//	texture2d_desc.SampleDesc.Quality = 0;
//	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
//	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
//
//	hr = device->CreateTexture2D(&texture2d_desc, NULL, depth_stencil_buffer.GetAddressOf());
//	if (FAILED(hr)) return false;
//
//	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc = {};
//	depth_stencil_view_desc.Format = texture2d_desc.Format;
//	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
//	depth_stencil_view_desc.Texture2D.MipSlice = 0;
//
//	hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc, depth_stencil_view.GetAddressOf());
//	return SUCCEEDED(hr);
//}
//
//void framework::setupViewport(UINT screenWidth, UINT screenHeight) {
//	D3D11_VIEWPORT viewport = {};
//	viewport.TopLeftX = 0;
//	viewport.TopLeftY = 0;
//	viewport.Width = static_cast<float>(screenWidth);
//	viewport.Height = static_cast<float>(screenHeight);
//	viewport.MinDepth = 0.0f;
//	viewport.MaxDepth = 1.0f;
//	immediate_context->RSSetViewports(1, &viewport);
//	
//}
//
//bool framework::createSamplerStates() {
//	HRESULT hr = S_OK;
//	D3D11_SAMPLER_DESC sampler_desc = {};
//	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
//	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
//	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
//	sampler_desc.MipLODBias = 0;
//	sampler_desc.MaxAnisotropy = 16;
//	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
//	sampler_desc.BorderColor[0] = 0;
//	sampler_desc.BorderColor[1] = 0;
//	sampler_desc.BorderColor[2] = 0;
//	sampler_desc.BorderColor[3] = 0;
//	sampler_desc.MinLOD = 0;
//	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
//
//	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
//	hr = device->CreateSamplerState(&sampler_desc, sampler_states[0].GetAddressOf());
//	if (FAILED(hr)) return false;
//
//	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
//	hr = device->CreateSamplerState(&sampler_desc, sampler_states[1].GetAddressOf());
//	if (FAILED(hr)) return false;
//
//	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
//	hr = device->CreateSamplerState(&sampler_desc, sampler_states[2].GetAddressOf());
//	if (FAILED(hr)) return false;
//
//	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
//	hr = device->CreateSamplerState(&sampler_desc, sampler_states[3].GetAddressOf());
//	if (FAILED(hr)) return false;
//
//	sampler_desc.BorderColor[0] = 1;
//	sampler_desc.BorderColor[1] = 1;
//	sampler_desc.BorderColor[2] = 1;
//	sampler_desc.BorderColor[3] = 1;
//	hr = device->CreateSamplerState(&sampler_desc, sampler_states[4].GetAddressOf());
//	return SUCCEEDED(hr);
//}
//
//bool framework::createDepthStencilStates() {
//	HRESULT hr = S_OK;
//	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
//	depth_stencil_desc.DepthEnable = TRUE;
//	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
//	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
//
//	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states[0].GetAddressOf());
//	if (FAILED(hr)) return false;
//
//	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
//	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states[1].GetAddressOf());
//	if (FAILED(hr)) return false;
//
//	depth_stencil_desc.DepthEnable = FALSE;
//	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
//	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states[2].GetAddressOf());
//	if (FAILED(hr)) return false;
//
//	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
//	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states[3].GetAddressOf());
//	return SUCCEEDED(hr);
//}
//
//bool framework::createBlendStates() {
//	HRESULT hr = S_OK;
//	D3D11_BLEND_DESC blend_desc = {};
//	blend_desc.AlphaToCoverageEnable = FALSE;
//	blend_desc.IndependentBlendEnable = FALSE;
//	blend_desc.RenderTarget[0].BlendEnable = FALSE;
//	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
//	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
//	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
//	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
//	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
//	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
//	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
//
//	hr = device->CreateBlendState(&blend_desc, blend_states[0].GetAddressOf());
//	if (FAILED(hr)) return false;
//
//	blend_desc.RenderTarget[0].BlendEnable = TRUE;
//	hr = device->CreateBlendState(&blend_desc, blend_states[1].GetAddressOf());
//	if (FAILED(hr)) return false;
//
//	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
//	hr = device->CreateBlendState(&blend_desc, blend_states[2].GetAddressOf());
//	if (FAILED(hr)) return false;
//
//	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
//	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
//	hr = device->CreateBlendState(&blend_desc, blend_states[3].GetAddressOf());
//	return SUCCEEDED(hr);
//}
//
//bool framework::createRasterizerStates() {
//	HRESULT hr = S_OK;
//	D3D11_RASTERIZER_DESC rasterizer_desc = {};
//	rasterizer_desc.FrontCounterClockwise = FALSE;
//	rasterizer_desc.DepthBias = 0;
//	rasterizer_desc.DepthBiasClamp = 0;
//	rasterizer_desc.SlopeScaledDepthBias = 0;
//	rasterizer_desc.DepthClipEnable = TRUE;
//	rasterizer_desc.ScissorEnable = FALSE;
//	rasterizer_desc.MultisampleEnable = FALSE;
//
//	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
//	rasterizer_desc.CullMode = D3D11_CULL_BACK;
//	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[0].GetAddressOf());
//	if (FAILED(hr)) return false;
//
//	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
//	rasterizer_desc.AntialiasedLineEnable = TRUE;
//	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[1].GetAddressOf());
//	if (FAILED(hr)) return false;
//
//	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
//	rasterizer_desc.CullMode = D3D11_CULL_NONE;
//	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[2].GetAddressOf());
//	if (FAILED(hr)) return false;
//
//	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
//	rasterizer_desc.AntialiasedLineEnable = TRUE;
//	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states[3].GetAddressOf());
//	return SUCCEEDED(hr);
//}
//
//bool framework::createConstantBuffers() {
//	HRESULT hr = S_OK;
//	D3D11_BUFFER_DESC buffer_desc = {};
//	buffer_desc.ByteWidth = sizeof(scene_constants);
//	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
//	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//
//	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffers[0].GetAddressOf());
//	if (FAILED(hr)) return false;
//
//	buffer_desc.ByteWidth = sizeof(parametric_constants);
//	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffers[1].GetAddressOf());
//	return SUCCEEDED(hr);
//}
//
//HRESULT framework::hr_trace(HRESULT hr) {
//	// Implement error tracing or logging here
//	return hr;
//}
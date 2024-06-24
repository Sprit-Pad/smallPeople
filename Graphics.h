#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <mutex>


// Framework クラスの前方宣言
class framework;
enum class ShaderId {
    Default,
    BLUR,
    Phong,
    Max
};
    enum class SAMPLER_STATE { POINT, LINEAR, ANISOTROPIC, LINEAR_BORDER_BLACK, LINEAR_BORDER_WHITE };
  

    enum class DEPTH_STATE { ZT_ON_ZW_ON, ZT_ON_ZW_OFF, ZT_OFF_ZW_ON, ZT_OFF_ZW_OFF };
  

    enum class BLEND_STATE { NONE, ALPHA, ADD, MULTIPLY };
  

    enum class RASTER_STATE { SOLID, WIREFRAME, CULL_NONE, WIREFRAME_CULL_NONE };
//#if 1
class Graphics {
public:
    Graphics(HWND hWnd);
    ~Graphics();

    // インスタンス取得
    static Graphics& Instance() { return *instance; }

    // デバイス取得
    ID3D11Device* GetDevice() const { return device.Get(); }
   
    // デバイスコンテキスト取得
    ID3D11DeviceContext* GetDeviceContext() const { return immediate_context.Get(); }

    // スワップチェーン取得
    IDXGISwapChain* GetSwapChain() const { return swap_chain.Get(); }

    // レンダーターゲットビュー取得
    ID3D11RenderTargetView* GetRenderTargetView() const { return render_target_view.Get(); }

    // デプスステンシルビュー取得
    ID3D11DepthStencilView* GetDepthStencilView() const { return depth_stencil_view.Get(); } 
   
    ID3D11Buffer* GetBuffer(ShaderId id) const { return constant_buffers[static_cast<int>(id)].Get(); } 
    
    ID3D11DepthStencilState* GetDepthStencilState(int id) const { return depth_stencil_states[static_cast<int>(id)].Get(); }

    ID3D11SamplerState* GetSamplerState(int id) const { return sampler_states[static_cast<int>(id)].Get(); }

    ID3D11BlendState* GetBlendState(int id) const { return blend_states[static_cast<int>(id)].Get(); }

    ID3D11RasterizerState* GetRasterizerState(int id) const { return rasterizer_states[static_cast<int>(id)].Get(); }

    

    // シェーダー取得
    
    // スクリーン幅取得
    float GetScreenWidth() const { return screenWidth; }

    // スクリーン高さ取得
    float GetScreenHeight() const { return screenHeight; }


    std::mutex& GetMutex() { return mutex; }

    void createDeviceAndSwapChain(HWND hwnd, UINT screenWidth, UINT screenHeight, bool fullscreen);
    void createRenderTargetView();
    void createDepthStencilView(UINT screenWidth, UINT screenHeight);
    void setupViewport(UINT screenWidth, UINT screenHeight);
    void createSamplerStates();
    void createDepthStencilStates();
    void createBlendStates();
    void createRasterizerStates();
    template<typename T>
    void createConstantBuffers(ID3D11Buffer**ffer)
    {
        HRESULT hr = S_OK;
        D3D11_BUFFER_DESC buffer_desc = {};
       
        //// WATER_SIMULATION
        buffer_desc.ByteWidth = sizeof(T);
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        buffer_desc.CPUAccessFlags = 0;
        buffer_desc.MiscFlags = 0;
        buffer_desc.StructureByteStride = 0;
        hr = device->CreateBuffer(&buffer_desc, nullptr, ffer);
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    };
    HRESULT hr_trace(HRESULT hr);
    void createAll();
    bool uninitialize();
 
    void SetSamplar();

private:
    static Graphics* instance;
   
     // DirectX11 で利用する様々なリソースを作成する大元
     Microsoft::WRL::ComPtr<ID3D11Device> device;
     // 様々な描画命令を GPU に伝える
     Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context;
     // キャンバスに描いた画を額に入れる
     Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain;
     // 色を書き込む先のキャンバス
     Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
     // 奥行情報を書き込むキャンバス
     Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;
     // サンプラーステート
     Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_states[5];
     // 深度ステンシルステート
     Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[4];
     // ブレンドステート(0...無し、1...アルファ、2...加算、3...乗算)
     Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[4];
     // 0...ソリッド裏面非表示 1...ワイヤーフレーム裏面非表示 2...ソリッド両面表示 3...ワイヤーフレーム両面表示
     Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[4];
     //
     Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffers[8];
    //std::unique_ptr<DebugRenderer> debugRenderer;
    //std::unique_ptr<LineRenderer> lineRenderer;
    //std::unique_ptr<ImGuiRenderer> imguiRenderer;

    float screenWidth;
    float screenHeight;
    std::mutex mutex;

};

//#endif  //GRAPHICS_H
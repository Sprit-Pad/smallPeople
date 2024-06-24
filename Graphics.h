#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <mutex>


// Framework �N���X�̑O���錾
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

    // �C���X�^���X�擾
    static Graphics& Instance() { return *instance; }

    // �f�o�C�X�擾
    ID3D11Device* GetDevice() const { return device.Get(); }
   
    // �f�o�C�X�R���e�L�X�g�擾
    ID3D11DeviceContext* GetDeviceContext() const { return immediate_context.Get(); }

    // �X���b�v�`�F�[���擾
    IDXGISwapChain* GetSwapChain() const { return swap_chain.Get(); }

    // �����_�[�^�[�Q�b�g�r���[�擾
    ID3D11RenderTargetView* GetRenderTargetView() const { return render_target_view.Get(); }

    // �f�v�X�X�e���V���r���[�擾
    ID3D11DepthStencilView* GetDepthStencilView() const { return depth_stencil_view.Get(); } 
   
    ID3D11Buffer* GetBuffer(ShaderId id) const { return constant_buffers[static_cast<int>(id)].Get(); } 
    
    ID3D11DepthStencilState* GetDepthStencilState(int id) const { return depth_stencil_states[static_cast<int>(id)].Get(); }

    ID3D11SamplerState* GetSamplerState(int id) const { return sampler_states[static_cast<int>(id)].Get(); }

    ID3D11BlendState* GetBlendState(int id) const { return blend_states[static_cast<int>(id)].Get(); }

    ID3D11RasterizerState* GetRasterizerState(int id) const { return rasterizer_states[static_cast<int>(id)].Get(); }

    

    // �V�F�[�_�[�擾
    
    // �X�N���[�����擾
    float GetScreenWidth() const { return screenWidth; }

    // �X�N���[�������擾
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
   
     // DirectX11 �ŗ��p����l�X�ȃ��\�[�X���쐬����匳
     Microsoft::WRL::ComPtr<ID3D11Device> device;
     // �l�X�ȕ`�施�߂� GPU �ɓ`����
     Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context;
     // �L�����o�X�ɕ`��������z�ɓ����
     Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain;
     // �F���������ސ�̃L�����o�X
     Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
     // ���s�����������ރL�����o�X
     Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;
     // �T���v���[�X�e�[�g
     Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_states[5];
     // �[�x�X�e���V���X�e�[�g
     Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[4];
     // �u�����h�X�e�[�g(0...�����A1...�A���t�@�A2...���Z�A3...��Z)
     Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[4];
     // 0...�\���b�h���ʔ�\�� 1...���C���[�t���[�����ʔ�\�� 2...�\���b�h���ʕ\�� 3...���C���[�t���[�����ʕ\��
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
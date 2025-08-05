#pragma once

#include "application_interface.h"
#include "window.h"

#include "DirectXMath.h"

class Application : public IApplication {
    public:
        using super = IApplication;

        Application(
            const std::wstring& name,
            int width,
            int height,
            bool vsync = false
        );

        virtual bool loadContent() override;

        virtual void unloadContent() override;

    protected:
        virtual void onUpdate(UpdateEventArgs& e) override;
        virtual void onRender(RenderEventArgs& e) override;
        virtual void onKeyPressed(KeyEventArgs& e) override;
        virtual void onMouseWheel(MouseWheelEventArgs& e) override;
        virtual void onResize(ResizeEventArgs& e) override;
        
    private:
        void transitionResource(
            ComPtr<ID3D12GraphicsCommandList2> commandList,
            ComPtr<ID3D12Resource> resource,
            D3D12_RESOURCE_STATES beforeState, 
            D3D12_RESOURCE_STATES afterState
        );

        void clearRTV(
            ComPtr<ID3D12GraphicsCommandList2> commandList,
            D3D12_CPU_DESCRIPTOR_HANDLE rtv, 
            FLOAT* clearColor
        );

        void clearDepth(
            ComPtr<ID3D12GraphicsCommandList2> commandList,
            D3D12_CPU_DESCRIPTOR_HANDLE dsv, 
            FLOAT depth = 1.0f 
        );

        // this function creates the gpu buffer
        void updateBufferResource(
            ComPtr<ID3D12GraphicsCommandList2> commandList,
            ID3D12Resource** pDestinationResource, 
            ID3D12Resource** pIntermediateResource,
            size_t numElements, 
            size_t elementSize, 
            const void* bufferData, 
            D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE 
        );

        void resizeDepthBuffer(
            int width, 
            int height
        );

        uint64_t fenceValues[Window::BufferCount] {};

        // vertex
        ComPtr<ID3D12Resource> vertexBuffer;
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

        // index
        ComPtr<ID3D12Resource> indexBuffer;
        D3D12_INDEX_BUFFER_VIEW indexBufferView;

        // Depth buffer.
        ComPtr<ID3D12Resource> depthBuffer;
        // Descriptor heap for depth buffer.
        ComPtr<ID3D12DescriptorHeap> dsvHeap;

        // Root signature
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;

        // Pipeline state object.
        Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
    
        // from vulkan and opengl -> this make so much SENSEEEEEE
        D3D12_VIEWPORT viewport;
        D3D12_RECT scissorRect;

        float fov; // field of view but i call it pov for some weird reason

        XMMATRIX modelMatrix;
        XMMATRIX viewMatrix;
        XMMATRIX projectionMatrix;

        bool isResourceLoaded;
};
#pragma once

#include "utils/pch.h"

class VertexBuffer {
    public:
        VertexBuffer(
            ComPtr<ID3D12Device2> device, 
            const std::vector<VertexStruct>& vertices
        );
        ~VertexBuffer() = default;

        ComPtr<ID3D12Resource> getBuffer() const { 
            return buffer; 
        }

        UINT getSize() const { 
            return sizeInBytes; 
        }

    private:
        D3D12_VERTEX_BUFFER_VIEW bufferView{};
        ComPtr<ID3D12Resource> buffer;
        UINT sizeInBytes = 0;
};
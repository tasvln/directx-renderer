struct VertexStruct {
    float3 position: POSITION;
    float3 color: COLOR;
};

struct VertexShaderOutput {
    float4 position : SV_POSITION; // -> Required by rasterizer
    float4 color    : COLOR;
};

// Constant buffer for MVP
cbuffer ModelViewProjectionCB : register(b0)
{
    matrix mvp;
}

VertexShaderOutput vsmain(VertexStruct input) {
    VertexShaderOutput output;

    output.position = mul(float4(input.position, 1.0f), mvp);

    output.color = float4(input.color, 1.0f);

    return output;
}
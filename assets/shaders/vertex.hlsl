struct VertexPositionColor {
    float3 position: POSITION;
    float3 color: COLOR;
};

struct ModelViewProjection {
    // 4x4 matrix
    matrix mvp;
};

struct VertexShaderOutput {
    float4 color: COLOR;
    float4 position: SV_POSITION; // required by rasterizer
};

// MVP constant buffer
ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);

VertexShaderOutput main(VertexPositionColor inVpc) {
    VertexShaderOutput vsOut;

    vsOut.position = mul(
        ModelViewProjectionCB.mvp,
        float4(inVpc.position, 1.0f)
    );

    vsOut.color = float4(inVpc.color, 1.0f);

    return vsOut;
}
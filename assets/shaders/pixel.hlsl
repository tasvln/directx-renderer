struct PixelInput {
    float4 color: COLOR;
};

float4 psmain(PixelInput input) : SV_TARGET {
    return input.color;
};
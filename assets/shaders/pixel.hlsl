struct PixelInput {
    float4 color: COLOR;
};

float4 main(PixelInput pIn) : SV_TARGET {
    return pIn.color;
};
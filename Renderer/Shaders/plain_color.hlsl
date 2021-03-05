struct PushConstants {
	uint width;
	uint height;
	float center_x;
	float center_y;
	float scale;
};

[[vk::binding(0, 0)]] RWTexture2D<float4> outputImg;
[[vk::push_constant]] ConstantBuffer<PushConstants> pushC;

float2 gen_cord(uint2 pix_cord) {
	float2 uv = (float2(pix_cord) + float2(0.5, 0.5)) / uint2(pushC.width, pushC.height);
	uv = 2 * uv - float2(1, 1);
	float aspect = float(pushC.height) / pushC.width;
	float2 cord = float2(pushC.center_x, pushC.center_y) + (float2(pushC.scale, pushC.scale * aspect) * uv);
	return cord;
}

float3 Mandelbrot(float2 c) {
	float2 z = float2(0, 0);
	int iter_cnt = 0;
	while (iter_cnt < 127 && z.x * z.x + z.y * z.y < 4) {
		z = float2(z.x * z.x - z.y * z.y, 2 * z.x * z.y) + c;
		++iter_cnt;
	}
	
	float col = iter_cnt / 128.0;
	return float3(col, col, col);
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint Gind : SV_GroupIndex) {
	outputImg[DTid.xy] = float4(Mandelbrot(gen_cord(DTid.xy)), 1.0);
}
struct PushConstants {
	uint width;
	uint height;
	double center_x;
	double center_y;
	double scale;
};

[[vk::binding(0, 0)]] RWTexture2D<float4> outputImg;
[[vk::push_constant]] ConstantBuffer<PushConstants> pushC;

double2 gen_cord(uint2 pix_cord) {
	double2 uv = (double2(pix_cord) + double2(0.5, 0.5)) / uint2(pushC.width, pushC.height);
	uv = 2 * uv - float2(1, 1);
	double aspect = double(pushC.height) / pushC.width;
	double2 cord = double2(pushC.center_x, pushC.center_y) + (double2(pushC.scale, pushC.scale * aspect) * uv);
	return cord;
}

float3 Mandelbrot(double2 c) {
	double2 z = double2(0, 0);
	int iter_cnt = 0;
	while (iter_cnt < 128 && z.x * z.x + z.y * z.y < 4) {
		z = double2(z.x * z.x - z.y * z.y, 2 * z.x * z.y) + c;
		++iter_cnt;
	}
	
	float val = iter_cnt / 128.0;
	
	if (iter_cnt == 128) {
		val = 0;
	}
	
	if (val < 0.5) {
		return float3(0, val * 2, 0);
	} else {
		return float3((val - 0.5) * 2, 1.0, (val - 0.5) * 2);
	}
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint Gind : SV_GroupIndex) {
	outputImg[DTid.xy] = float4(Mandelbrot(gen_cord(DTid.xy)), 1.0);
}
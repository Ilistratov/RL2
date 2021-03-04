struct PushConstants {
	float t;
};

[[vk::binding(0, 0)]] RWTexture2D<float4> outputImg;
[[vk::push_constant]] ConstantBuffer<PushConstants> pushC;

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint Gind : SV_GroupIndex) {
	outputImg[DTid.xy] = float4(cos(pushC.t), sin(pushC.t), -sin(pushC.t), 0.5);
}
#include <iostream>
#include <chrono>
#include <thread>
#include <tuple>
#include "Utill\Logger.h"
#include "Renderer\Core.h"
#include "Renderer\ShaderHandler\Compute.h"
#include "Renderer\CmdExecutor.h"
#include "Renderer\ShaderBindable\StructuredBuffer.h"
#include "Renderer\ShaderBindable\RenderTarget.h"
#include "Renderer\ShaderBindable\PushConstant.h"
#include "Renderer\CmdExecutor.h"

//#pragma warning(push, 0)
//#include <vulkan\vulkan.hpp>
//#pragma warning(pop)
//
//#include "CompileTimeDefined.h"
//
//#include "Rendering\Renderer.h"
//#include "Rendering\Utill\Logger.h"
//
//Logger GlobalLog;
//const uint8_t Logger::LogLvl = 0;
//
//void initLogger() {
//	std::stringstream log_file;
//	log_file << "log";
//	//Logger::addTime(log_file);
//	log_file << ".txt";
//	GlobalLog.startSession(log_file.str().c_str());
//	GlobalLog.infoMsg("Started session");
//	std::string start_msg = "Running version ";
//	start_msg += PROJECT_VER;
//	GlobalLog.infoMsg(start_msg);
//}
//
//void deinitLogger() {
//	GlobalLog.infoMsg("Ending session");
//	GlobalLog.stopSession();
//}
//
//int main() {
//	initLogger();
//
//	try {
//		Rendering::Renderer renderer;
//
//		std::chrono::high_resolution_clock clock;
//		auto prv = clock.now();
//
//		while (renderer.renderFrame()) {
//			auto cur = clock.now();
//			std::cout << "update took:" << std::chrono::duration_cast<std::chrono::milliseconds>(cur - prv).count() << "ms\n";
//			std::swap(cur, prv);
//		}
//	} catch (const std::exception& e) {
//		GlobalLog.errorMsg(e.what());
//	}
//
//	deinitLogger();
//}

using Renderer::ShaderBindable::RenderTarget;
using Renderer::Pipeline::SetBindable;
using Renderer::ShaderHandler::Compute;
using Renderer::Pipeline::DPoolHandler;

struct PushConstant {
	float t;
};

/*
ToDo:
there is still some bugs with PushConstants, they don't work/update properly
*/

int main(int argc, char* argv[]) {
	Renderer::core.init();
	int dDimX = Renderer::core.swapchain().extent().width / 8;
	int dDimY = Renderer::core.swapchain().extent().height / 8;
	int dDimZ = 1;

	PushConstant releventPc;
	releventPc.t = 0;

	std::vector<RenderTarget> rt;
	std::vector<Compute> ppln;
	std::vector<Renderer::ShaderBindable::PushConstantController> pcc;
	std::vector<Renderer::ShaderBindable::PushConstant<PushConstant>> pc;
	std::vector<Renderer::CmdExecutor> cmd;
	std::vector<vk::Semaphore> rdyToPresent;
	
	rt.reserve(Renderer::core.swapchain().imageCount());
	ppln.reserve(Renderer::core.swapchain().imageCount());
	cmd.reserve(Renderer::core.swapchain().imageCount());
	pc.reserve(Renderer::core.swapchain().imageCount());
	pcc.reserve(Renderer::core.swapchain().imageCount());
	rdyToPresent.reserve(Renderer::core.swapchain().imageCount());

	for (uint32_t i = 0; i < Renderer::core.swapchain().imageCount(); i++) {
		rt.push_back(std::move(RenderTarget(
			Renderer::core.swapchain().getImageData(i),
			Renderer::core.swapchain().genInit(i),
			Renderer::core.swapchain().genPreBlit(i),
			Renderer::core.swapchain().genPostBlit(i),
			vk::ShaderStageFlagBits::eCompute))
		);
		
		SetBindable bnd({ &rt[i] });
		pc.emplace_back(vk::ShaderStageFlagBits::eCompute, releventPc);
		pcc.emplace_back(std::vector{ (Renderer::ShaderBindable::IPushConstant*)&pc[i] });
		
		ppln.push_back(Compute(std::vector{ bnd }, pcc[i], std::string("Shaders\\plain_color.spv"), std::string("main"), { dDimX, dDimY, dDimZ }));
		
		rdyToPresent.push_back(Renderer::core.device().createSemaphore({}));
		
		cmd.push_back(
			Renderer::CmdExecutor{
				std::vector<Renderer::ExecutionStageDescription>{
					Renderer::ExecutionStageDescription{
						&pcc[i],
						{},
						{},
						{},
						{},
						{}
					},
					Renderer::ExecutionStageDescription{
						&ppln[i],
						{ 2, 0 },
						{ vk::PipelineStageFlags(vk::PipelineStageFlagBits::eComputeShader), vk::PipelineStageFlags(vk::PipelineStageFlagBits::eComputeShader) },
						{},
						{ Renderer::core.swapchain().imageAvaliable() },
						{ vk::PipelineStageFlags(vk::PipelineStageFlagBits::eComputeShader) }
					},
					Renderer::ExecutionStageDescription{
						&rt[i],
						{ 1 },
						{ vk::PipelineStageFlags(vk::PipelineStageFlagBits::eTransfer) },
						{ rdyToPresent[i] },
						{},
						{}
					}
				}
			}
		);
	}
	
	std::chrono::high_resolution_clock clock;
	auto prv = clock.now();
	

	while (!glfwWindowShouldClose(Renderer::core.apiBase().window())) {
		auto cur = clock.now();
		releventPc.t = std::chrono::duration_cast<std::chrono::milliseconds>(cur - prv).count() / 1000.0;
		glfwPollEvents();
		Renderer::core.swapchain().acquireNextImage();
		uint64_t ind = Renderer::core.swapchain().activeImage();
		pc[ind].update(releventPc);
		cmd[ind].submit();
		Renderer::core.swapchain().present({ rdyToPresent[ind] });
	}

	for (auto& item : rdyToPresent) {
		Renderer::core.device().destroySemaphore(item);
	}
	return 0;
}

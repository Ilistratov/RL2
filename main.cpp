#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
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
	uint32_t width;
	uint32_t height;
	double center_x;
	double center_y;
	double scale;
};

std::atomic<uint64_t> render_time;
bool waitForInput = true;
std::mutex n_pc_accsess;

void readInput(PushConstant& n_pc) {
	std::string s;
	double n_val;
	while (waitForInput && std::cin >> s >> n_val) {
		if (s == "cx") {
			std::scoped_lock lock(n_pc_accsess);
			n_pc.center_x = n_val;
		} else if (s == "cy") {
			std::scoped_lock lock(n_pc_accsess);
			n_pc.center_y = n_val;
		} else if (s == "s") {
			std::scoped_lock lock(n_pc_accsess);
			n_pc.scale = n_val;
		} else if (s == "mvx") {
			n_pc.center_x += n_pc.scale * n_val;
		} else if (s == "mvy") {
			n_pc.center_y += n_pc.scale * n_val;
		} else if (s == "mvs") {
			n_pc.scale *= n_val;
		} else if (s == "t") {
			std::cout << "Render took " << render_time << " ms\n";
		} else {
			GlobalLog.warningMsg("command " + s + " - not recognised");
		}
	}
}

void update_val(double& val, double d) {
	val += d * 0.01;
}

void updateRelevantPc(PushConstant& releventPc, const PushConstant& n_pc) {
	std::scoped_lock lock(n_pc_accsess);
	double dcx = n_pc.center_x - releventPc.center_x;
	double dcy = n_pc.center_y - releventPc.center_y;
	double ds = n_pc.scale - releventPc.scale;
	update_val(releventPc.center_x, dcx);
	update_val(releventPc.center_y, dcy);
	update_val(releventPc.scale, ds);
}

int main(int argc, char* argv[]) {
	Renderer::core.init();
	int dDimX = Renderer::core.swapchain().extent().width / 8;
	int dDimY = Renderer::core.swapchain().extent().height / 8;
	int dDimZ = 1;

	PushConstant releventPc;
	PushConstant n_pc;

	releventPc.width = Renderer::core.swapchain().extent().width;
	releventPc.height = Renderer::core.swapchain().extent().height;
	releventPc.scale = 2;
	releventPc.center_x = -1.5;
	releventPc.center_y = 0;

	n_pc = releventPc;

	std::vector<RenderTarget> rt;
	std::vector<Compute> ppln;
	std::vector<Renderer::ShaderBindable::PushConstant<PushConstant>> pc;
	std::vector<Renderer::CmdExecutor> cmd;
	std::vector<vk::Semaphore> rdyToPresent;
	
	rt.reserve(Renderer::core.swapchain().imageCount());
	ppln.reserve(Renderer::core.swapchain().imageCount());
	cmd.reserve(Renderer::core.swapchain().imageCount());
	pc.reserve(Renderer::core.swapchain().imageCount());
	rdyToPresent.reserve(Renderer::core.swapchain().imageCount());

	std::thread t(readInput, std::ref(n_pc));

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
		pc[i].update(releventPc);
		
		ppln.push_back(Compute(std::vector{ bnd }, { &pc[i] }, std::string("Shaders\\plain_color.spv"), std::string("main"), { dDimX, dDimY, dDimZ }));
		
		rdyToPresent.push_back(Renderer::core.device().createSemaphore({}));
		
		cmd.push_back(
			Renderer::CmdExecutor{
				std::vector<Renderer::ExecutionStageDescription>{
					Renderer::ExecutionStageDescription{
						&ppln[i],
						{ 1 },
						{ vk::PipelineStageFlags(vk::PipelineStageFlagBits::eComputeShader) },
						{},
						{},
						{}
					},
					Renderer::ExecutionStageDescription{
						&rt[i],
						{ 0 },
						{ vk::PipelineStageFlags(vk::PipelineStageFlagBits::eTransfer) },
						{ rdyToPresent[i] },
						{ Renderer::core.swapchain().imageAvaliable() },
						{ vk::PipelineStageFlags(vk::PipelineStageFlagBits::eTransfer) }
					}
				}
			}
		);
	}
	
	std::chrono::high_resolution_clock clock;
	auto prv = clock.now();

	while (!glfwWindowShouldClose(Renderer::core.apiBase().window())) {
		auto cur = clock.now();
		render_time = std::chrono::duration_cast<std::chrono::milliseconds>(cur - prv).count();
		std::swap(cur, prv);

		glfwPollEvents();
		Renderer::core.swapchain().acquireNextImage();
		uint64_t ind = Renderer::core.swapchain().activeImage();
		
		updateRelevantPc(releventPc, n_pc);
		pc[ind].update(releventPc);
		
		cmd[ind].submit();
		Renderer::core.swapchain().present({ rdyToPresent[ind] });
	}

	waitForInput = false;
	t.join();

	Renderer::core.device().waitIdle();

	for (auto& item : rdyToPresent) {
		Renderer::core.device().destroySemaphore(item);
	}
	return 0;
}

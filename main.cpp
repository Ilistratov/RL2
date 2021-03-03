#include <iostream>
#include <chrono>
#include <thread>
#include "Renderer\Core.h"
#include "Renderer\CmdExecutor.h"
#include "Renderer\ShaderBindable\StructuredBuffer.h"
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

int main(int argc, char* argv[]) {
	Renderer::ShaderBindable::StructuredBuffer sb(std::move(Renderer::ResourceHandler::StructuredBuffer(5, 1)));
	std::unique_ptr<Renderer::ResourceHandler::StagingBuffer> stg = std::make_unique<Renderer::ResourceHandler::StagingBuffer>(5);
	std::vector<char> data(5, 'X');
	stg->copyFrom(data);
	sb.pendCopy(std::move(stg));
	Renderer::CmdExecutor cmd{
		std::vector<Renderer::ExecutionStageDescription>{
			Renderer::ExecutionStageDescription{
				&sb
			}
		}
	};

	cmd.submit();
	cmd.submit();

	return 0;
}

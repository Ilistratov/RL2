#pragma once

#include <mutex>
#include <vector>

#include "Renderer\ICmdRecorder.h"

namespace Renderer::ShaderBindable {

class IPushConstant {
public:
	virtual void setOffset(uint64_t n_offset) = 0;
	virtual vk::PushConstantRange getPCR() = 0;
	virtual uint32_t getSize() = 0;
	virtual void recordPush(vk::CommandBuffer cmd, vk::PipelineLayout layt) = 0;
};

template<typename T>
class PushConstant : public IPushConstant {
	vk::ShaderStageFlags stage;
	uint32_t offset = 0;
	
	std::mutex dataAccess;
	bool upd = false;
	T data;

public:
	PushConstant(vk::ShaderStageFlags stage = {}, const T& data = {}) : stage(stage), upd(true), data(data) {}
	PushConstant(const PushConstant& other) {
		*this = other;
	}

	PushConstant& operator = (const PushConstant& other) {
		stage = other.stage;
		offset = other.offset;
		upd = other.upd;
		data = other.data;

		return *this;
	}

	void setOffset(uint64_t n_offset) override {
		offset = n_offset;
	}

	vk::PushConstantRange getPCR() override {
		return vk::PushConstantRange{
			stage,
			offset,
			sizeof(T)
		};
	}

	uint32_t getSize() override {
		return sizeof(T);
	}

	void recordPush(vk::CommandBuffer cmd, vk::PipelineLayout layt) override {
		std::scoped_lock lock(dataAccess);
		if (upd) {
			cmd.pushConstants(layt, stage, offset, sizeof(T), &data);
			upd = false;
		}
	}

	void update(const T& n_data) {
		std::scoped_lock lock(dataAccess);
		data = n_data;
		upd = true;
	}
};

class PushConstantController : public ICmdRecorder {
	std::vector<IPushConstant*> pushConstants;
	vk::PipelineLayout boundLayt;
	vk::Pipeline boundPpln;
public:
	PushConstantController(std::vector<IPushConstant*> pushConstants = {});

	void bindLayout(vk::PipelineLayout layt);
	void bindPipeline(vk::Pipeline ppln);
	std::vector<vk::PushConstantRange> getPCR();
	
	void recordDynamic(vk::CommandBuffer cmd) override;
	void recordInit(vk::CommandBuffer cmd) override;
};

}
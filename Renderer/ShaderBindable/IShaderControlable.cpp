#include "IShaderControlable.h"

namespace Renderer::ShaderBindable {

void IShaderInput::recordInit(vk::CommandBuffer cmd) {}

void IShaderInput::recordRegular(vk::CommandBuffer cmd) {}

void IShaderInput::recordDynamic(vk::CommandBuffer cmd) {}

void IShaderOutput::recordInit(vk::CommandBuffer cmd) {}

void IShaderOutput::recordRegular(vk::CommandBuffer cmd) {}

void IShaderOutput::recordDynamic(vk::CommandBuffer cmd) {}

}
